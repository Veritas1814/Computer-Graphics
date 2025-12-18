#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;
uniform bool hdr;
uniform int toneMappingMode;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 mapped;

    if(hdr)
    {
        hdrColor *= exposure;

        if (toneMappingMode == 0)
        {
            // Reinhard tone mapping
            mapped = hdrColor / (hdrColor + vec3(1.0));
        }
        else
        {
            // Filmic (ACES approximation) - Formerly Mode 2
            vec3 color = hdrColor * 0.6;
            float a = 2.51;
            float b = 0.03;
            float c = 2.43;
            float d = 0.59;
            float e = 0.14;
            mapped = clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
        }

        // Gamma correction
        mapped = pow(mapped, vec3(1.0 / gamma));
        FragColor = vec4(mapped, 1.0);
    }
    else
    {
        // No HDR Tone Mapping enabled
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}