#version 330 core
out vec4 FragColour;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform float alpha;
uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct LightProperties {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;
    Attenuation attenuation;
    LightProperties lightProp;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    Attenuation attenuation;
    LightProperties lightProp;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;
    Attenuation attenuation;
    LightProperties lightProp;
};
uniform SpotLight spotLight;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine effects
    vec3 ambient = light.lightProp.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.lightProp.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.lightProp.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attentuation = 1.0f / (light.attenuation.constant +
                                 light.attenuation.linear * distance +
                                 light.attenuation.quadratic * (distance * distance));
    // combine effects
    vec3 ambient = light.lightProp.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.lightProp.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.lightProp.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attentuation = 1.0f / (light.attenuation.constant +
                                       light.attenuation.linear * distance +
                                       light.attenuation.quadratic * (distance * distance));
    // spotlight calc
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    // combine effects
    vec3 ambient = light.lightProp.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.lightProp.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.lightProp.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;
    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 1. directional lighting
    vec3 result = calcDirLight(dirLight, norm, viewDir);
    // 2. point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    // 3. spot light
    result += calcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColour = vec4(result, alpha);
}
