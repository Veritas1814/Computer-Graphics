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

uniform DirLight dirLight;
uniform sampler2D       shadowMap;
uniform sampler2DShadow shadowMapCmp;

uniform vec3  viewPos;
uniform vec3  tintColor;
uniform bool  usePCF;
uniform bool  useComparisonSampler;
uniform float shadowBias;
uniform float materialAlpha;

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
    if (useComparisonSampler)
        texSize = textureSize(shadowMapCmp, 0);
    else
        texSize = textureSize(shadowMap, 0);

    vec2 texelSize = 1.0 / vec2(texSize);

    for (int x=-1; x<=1; ++x) {
        for (int y=-1; y<=1; ++y) {
            vec2 offset = vec2(x,y) * texelSize;
            if (!useComparisonSampler) {
                float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
                result += (currentDepth > closestDepth) ? 0.0 : 1.0;
            } else {
                result += texture(shadowMapCmp,
                                  vec3(projCoords.xy + offset, currentDepth));
            }
            ++samples;
        }
    }
    return result / float(samples);
}

void main()
{
    vec3 normal   = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-dirLight.direction);

    float diff    = max(dot(normal, lightDir), 0.0);
    float dirVis  = computeShadowVisibility(fs_in.FragPosLightSpace, normal);

    vec3 ambient  = 0.1 * tintColor * dirLight.color;
    vec3 diffuse  = diff * tintColor * dirLight.color;

    vec3 color = ambient + dirVis * diffuse;
    FragColor = vec4(color, materialAlpha);
}
c