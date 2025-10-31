#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define NR_POINT_LIGHTS 2
struct DirLight { vec3 direction; vec3 color; };
struct PointLight { vec3 position; vec3 color; float constant; float linear; float quadratic; };
struct SpotLight { vec3 position; vec3 direction; vec3 color; float cutOff; float outerCutOff; float constant; float linear; float quadratic; };

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;
uniform int  blinn;
uniform float shininess;
uniform vec3 specularColor;

uniform sampler2D texture_diffuse1;

vec3 blinnSpec(vec3 N, vec3 L, vec3 V, float sh){
    vec3 H = normalize(L+V);
    return pow(max(dot(N,H),0.0), sh) * specularColor;
}
vec3 phongSpec(vec3 N, vec3 L, vec3 V, float sh){
    vec3 R = reflect(-L,N);
    return pow(max(dot(R,V),0.0), sh) * specularColor;
}
vec3 calcDir(DirLight Lgt, vec3 N, vec3 V, vec3 albedo){
    vec3 L = normalize(-Lgt.direction);
    float diff = max(dot(N,L),0.0);
    vec3 spec = (blinn==1)? blinnSpec(N,L,V,shininess) : phongSpec(N,L,V,shininess);
    return (albedo*diff + spec) * Lgt.color;
}
vec3 calcPoint(PointLight Lgt, vec3 N, vec3 V, vec3 albedo){
    vec3 L = normalize(Lgt.position - FragPos);
    float diff = max(dot(N,L),0.0);
    vec3 spec = (blinn==1)? blinnSpec(N,L,V,shininess) : phongSpec(N,L,V,shininess);
    float d = length(Lgt.position - FragPos);
    float att = 1.0/(Lgt.constant + Lgt.linear*d + Lgt.quadratic*d*d);
    return (albedo*diff + spec) * Lgt.color * att;
}
vec3 calcSpot(SpotLight Lgt, vec3 N, vec3 V, vec3 albedo){
    vec3 L = normalize(Lgt.position - FragPos);
    float theta = dot(L, normalize(-Lgt.direction));
    float eps = Lgt.cutOff - Lgt.outerCutOff;
    float falloff = clamp((theta - Lgt.outerCutOff)/eps, 0.0, 1.0);
    float diff = max(dot(N,L),0.0);
    vec3 spec = (blinn==1)? blinnSpec(N,L,V,shininess) : phongSpec(N,L,V,shininess);
    float d = length(Lgt.position - FragPos);
    float att = 1.0/(Lgt.constant + Lgt.linear*d + Lgt.quadratic*d*d);
    return (albedo*diff + spec) * Lgt.color * att * falloff;
}

void main(){
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 ambient = 0.05 * albedo;

    vec3 color = ambient;
    color += calcDir(dirLight, N, V, albedo);
    for(int i=0;i<NR_POINT_LIGHTS;++i) color += calcPoint(pointLights[i], N, V, albedo);
    color += calcSpot(spotLight, N, V, albedo);

    FragColor = vec4(color,1.0);
}
