#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

struct DirLight {
    vec3 direction;
    vec3 color;
};
struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
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

uniform DirLight  dirLight;
uniform PointLight pointLights[2];
uniform SpotLight  spotLight;
uniform sampler2D  texture_diffuse1;
uniform sampler2D  texture_specular1;
uniform sampler2D  shadowMap;
uniform sampler2DShadow shadowMapCmp;

uniform vec3  viewPos;
uniform int   blinn;
uniform float shininess;
uniform vec3  specularColor;

uniform bool  usePCF;
uniform bool  useComparisonSampler;
uniform float shadowBias;
uniform float materialAlpha;

uniform bool cellShading;

float computeShadowVisibility(vec4 fragPosLightSpace, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 1.0;

    float cosTheta = max(dot(normalize(normal), -normalize(dirLight.direction)), 0.0);
    float bias = shadowBias * max(0.05, 1.0 - cosTheta);
    float currentDepth = projCoords.z - bias;

    if (!usePCF) {
        if (!useComparisonSampler) {
            float closestDepth = texture(shadowMap, projCoords.xy).r;
            return (currentDepth > closestDepth) ? 0.0 : 1.0;
        } else {
            return texture(shadowMapCmp, vec3(projCoords.xy, currentDepth));
        }
    }

    float result = 0.0;
    int samples = 0;
    ivec2 texSize;
    if (useComparisonSampler) texSize = textureSize(shadowMapCmp, 0);
    else texSize = textureSize(shadowMap, 0);
    vec2 texelSize = 1.0 / vec2(texSize);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            if (!useComparisonSampler) {
                float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
                result += (currentDepth > closestDepth) ? 0.0 : 1.0;
            } else {
                result += texture(shadowMapCmp, vec3(projCoords.xy + offset, currentDepth));
            }
            ++samples;
        }
    }
    return result / float(samples);
}

vec3 applyCellShading(float diff, float spec, vec3 normal, vec3 viewDir, vec3 lightColor, vec3 albedo, bool isMainLight) {
    float intensity = diff;
    float steps = 4.0;
    float cellDiff = ceil(intensity * steps) / steps;
    // cellDiff = smoothstep(cellDiff - 0.01, cellDiff + 0.01, intensity) * cellDiff;


    //float cellSpec = step(0.5, spec);
    vec3 specular = vec3(0.0);
    vec3 rimColor = vec3(0.0);
    if (isMainLight) {
           float rim = 1.0 - max(dot(viewDir, normal), 0.0);
           float rimThreshold = 0.6;
           float rimIntensity = smoothstep(rimThreshold - 0.05, rimThreshold + 0.05, rim);
           rimColor = rimIntensity * lightColor * albedo * 0.8;
        }

    vec3 ambient = 0.1 * albedo * lightColor;
    vec3 diffuse = cellDiff * albedo * lightColor;
    //vec3 specular = cellSpec * specularColor * lightColor;

    return ambient + diffuse + specular + rimColor;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float visibility)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = 0.0;
    if (diff > 0.0) {
        if (blinn == 1) spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        else {
            vec3 reflectDir = reflect(-lightDir, normal);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }
    }

    if (cellShading) {
        return applyCellShading(diff, spec, normal, viewDir, light.color * visibility, albedo, true);
    }

    vec3 ambient  = 0.1 * albedo * light.color;
    vec3 diffuse  = diff * albedo * light.color;
    vec3 specular = spec * specularColor * light.color;
    return ambient + visibility * (diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = 0.0;
    if (diff > 0.0) {
        if (blinn == 1) spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        else {
            vec3 reflectDir = reflect(-lightDir, normal);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }
    }

    float dist  = length(light.position - fragPos);
    float atten = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    if (cellShading) {
        return applyCellShading(diff, spec, normal, viewDir, light.color * atten, albedo, false);
    }

    vec3 ambient  = 0.05 * albedo * light.color;
    vec3 diffuse  = diff * albedo * light.color;
    vec3 specular = spec * specularColor * light.color;

    return (ambient + diffuse + specular) * atten;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = 0.0;
    if (diff > 0.0) {
        if (blinn == 1) spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        else {
            vec3 reflectDir = reflect(-lightDir, normal);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }
    }

    float dist  = length(light.position - fragPos);
    float atten = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    float theta   = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    if (cellShading) {
        return applyCellShading(diff, spec, normal, viewDir, light.color * atten * intensity, albedo, false);
    }

    vec3 ambient  = 0.05 * albedo * light.color;
    vec3 diffuse  = diff * albedo * light.color;
    vec3 specular = spec * specularColor * light.color;
    return (ambient + diffuse + specular) * atten * intensity;
}

void main()
{
    vec3 albedo = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    if (albedo == vec3(0.0)) albedo = vec3(1.0);

    vec3 normal  = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    float dirVisibility = computeShadowVisibility(fs_in.FragPosLightSpace, normal);

    vec3 color = vec3(0.0);
    color += calcDirLight(dirLight, normal, viewDir, albedo, dirVisibility);
    color += calcPointLight(pointLights[0], normal, fs_in.FragPos, viewDir, albedo);
    color += calcPointLight(pointLights[1], normal, fs_in.FragPos, viewDir, albedo);
    color += calcSpotLight(spotLight,  normal, fs_in.FragPos, viewDir, albedo);

    FragColor = vec4(color, materialAlpha);
}