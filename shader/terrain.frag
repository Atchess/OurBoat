#version 460 core

in float Height;
in vec3 Normal;
in vec3 FragPos;

uniform float hei_Lim;

layout (binding = 0) uniform sampler2D imagePoint;
layout (binding = 1) uniform sampler2D imageNormal;
layout (binding = 2) uniform samplerCube skybox;

out vec4 FragColor;

const float ambientStrength = 0.1f;
const float specularStrength = 0.5f;
const vec3 sunPos = vec3(0,50,100);
uniform vec3 viewPos;

const vec3 lightColor = vec3(0.0, 0.65, 0.75);
const vec3 ambient_color  = vec3(0.0, 0.65, 0.75);
const vec3 diffuse_color  = vec3(0.5, 0.65, 0.75);
const vec3 swallow = vec3(0.275,0.855,1.0);
const vec3 skyColor = vec3(0.65, 0.80, 0.95);

const vec3 SEA_BASE = vec3(0.1,0.19,0.22);
const vec3 SEA_WATER_COLOR = vec3(0.1,0.19,0.22);

float CalculateFresnel (vec3 I, vec3 N)
{
    float _FresnelPower=10.0;
    float R_0 =0.04;
    //R_0 *= R_0;
    return  R_0 + (1.0 - R_0) * pow(1.0 - max(dot(I, N),0.0), _FresnelPower);
}

void main(){

    // ambient
    vec3 ambient = ambientStrength * lightColor * SEA_WATER_COLOR;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(sunPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * SEA_WATER_COLOR;

    // specular 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor * SEA_WATER_COLOR;

    // reflect
    vec3 reflectVec = normalize(reflect(-viewDir, norm));
    vec4 reflectColor = texture(skybox, reflectVec);


    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);//mix(vec4(color, 1.0) , reflectColor, 0.5);//reflectColor + vec4(color, 1.0); 

    //vec3 Normal = normalize(aNormal);
    //vec3 lightDir = normalize(sunPos - FragPos);
    //vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 re = normalize(reflect(- viewDir, Normal));
//
    //vec3 diffuse = pow( dot(Normal, viewDir) * 0.4 + 0.6, 3.0) * vec3(1.0f);
    ////vec3 reflected = skyColor * 0.7;
    //vec3 reflected = texture(skybox, re).rgb * 0.8;
    //vec3 refracted = SEA_BASE + diffuse * (0.3 * SEA_WATER_COLOR + 0.7 * swallow) * 0.12;
    //float fresnel = CalculateFresnel(viewDir, aNormal);
    //float spec = pow(max(dot(Normal, halfwayDir), 0.0), 128);
    //vec3 specular = (0.2*swallow+0.8*vec3(1,1,1)) * spec * 0.4;
    ////reflected+=specular;
    //refracted+=swallow*pow(max(Height*hei_Lim*1700,0),2);
    //vec3 color=mix(refracted,reflected,fresnel);
    ////color+=2.5*swallow*pow(Height,2);
    //FragColor=vec4(color+specular,1.0);
}
