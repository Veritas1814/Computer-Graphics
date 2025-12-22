#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

uniform bool useNormalMap;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform PointLight pointLights[2];
uniform DirLight dirLight;
uniform SpotLight spotLight;

uniform vec3 viewPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalculatePBR(vec3 L, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic, vec3 radiance) {
    vec3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main()
{
    vec3 albedo = pow(texture(albedoMap, fs_in.TexCoords).rgb, vec3(2.2));
    float metallic = texture(metallicMap, fs_in.TexCoords).r;
    float roughness = texture(roughnessMap, fs_in.TexCoords).r;

    roughness = max(roughness, 0.05);

    vec3 N;
    if (useNormalMap) {
        vec3 map = texture(normalMap, fs_in.TexCoords).rgb;
        map = map * 2.0 - 1.0;

        float distToPole = min(fs_in.TexCoords.y, 1.0 - fs_in.TexCoords.y);
        float fade = smoothstep(0.0, 0.05, distToPole);

        map = mix(vec3(0.0, 0.0, 1.0), map, fade);

        N = normalize(fs_in.TBN * map);
    } else {
        N = normalize(fs_in.TBN[2]);
    }

    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    if(length(dirLight.color) > 0.0 && length(dirLight.direction) > 0.01) {
        vec3 L = normalize(-dirLight.direction);
        Lo += CalculatePBR(L, V, N, F0, albedo, roughness, metallic, dirLight.color);
    }

    for(int i = 0; i < 2; ++i) {
        if(length(pointLights[i].color) > 0.0) {
            vec3 L = normalize(pointLights[i].position - fs_in.FragPos);
            float distance = length(pointLights[i].position - fs_in.FragPos);
            float bot = pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance;
            float attenuation = 1.0 / max(bot, 0.0001);
            vec3 radiance = pointLights[i].color * attenuation;
            Lo += CalculatePBR(L, V, N, F0, albedo, roughness, metallic, radiance);
        }
    }

    if(length(spotLight.color) > 0.0 && length(spotLight.direction) > 0.01) {
        vec3 L = normalize(spotLight.position - fs_in.FragPos);
        float distance = length(spotLight.position - fs_in.FragPos);
        float bot = spotLight.constant + spotLight.linear * distance + spotLight.quadratic * distance * distance;
        float attenuation = 1.0 / max(bot, 0.0001);

        float theta = dot(L, normalize(-spotLight.direction));
        float epsilon = spotLight.cutOff - spotLight.outerCutOff;
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 radiance = spotLight.color * attenuation * intensity;
        Lo += CalculatePBR(L, V, N, F0, albedo, roughness, metallic, radiance);
    }

    vec3 ambient = vec3(0.0001) * albedo;
    FragColor = vec4(ambient + Lo, 1.0);
}