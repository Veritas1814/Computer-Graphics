#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture; // The rendered scene color
uniform sampler2D depthTexture;  // The rendered scene depth

uniform float depthThreshold; // Sensitivity of the outline

// Linearize depth to make the math easier
float LinearizeDepth(float depth)
{
    float near = 1.0;
    float far  = 60.0; // Must match your projection matrix zFar
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // 1. Get the Color of the scene
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // 2. Edge Detection (Sobel-like kernel on Depth)
    // We check the depth of the current pixel vs its neighbors.
    float dx = 1.0 / 1280.0; // 1 pixel X offset (assuming 1280 width)
    float dy = 1.0 / 720.0;  // 1 pixel Y offset (assuming 720 height)

    float d0 = LinearizeDepth(texture(depthTexture, TexCoords).r);
    float d1 = LinearizeDepth(texture(depthTexture, TexCoords + vec2(dx, 0.0)).r); // Right
    float d2 = LinearizeDepth(texture(depthTexture, TexCoords - vec2(dx, 0.0)).r); // Left
    float d3 = LinearizeDepth(texture(depthTexture, TexCoords + vec2(0.0, dy)).r); // Up
    float d4 = LinearizeDepth(texture(depthTexture, TexCoords - vec2(0.0, dy)).r); // Down

    // Calculate the difference in depth
    float diff = abs(d1 - d2) + abs(d3 - d4);

    // 3. Draw Outline
    // If the depth difference is large, it's an edge -> Draw Black
    if (diff > depthThreshold) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black Outline
    } else {
        FragColor = vec4(color, 1.0); // Normal Scene Color
    }
}