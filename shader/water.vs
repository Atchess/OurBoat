#version 460 core

#define SMALL 0.001 
#define BIG 0.999

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoords;

layout (binding = 0) uniform sampler2D imagePoint;
layout (binding = 1) uniform sampler2D imageNormal;
layout (binding = 2) uniform samplerCube skybox;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 modelInv;

out VS_OUT {
    vec4 fragPos;
    vec3 normal;
    vec2 texCoord;
} vs_out;

void main()
{
    vec3 pos = texture(imagePoint, texCoords.xy).xyz;
    vec3 n = texture(imageNormal, texCoords.xy).xyz;

    gl_Position = projection * view * model * vec4(pos, 1.0);

    vs_out.fragPos = model * vec4(pos, 1.0);
    vs_out.texCoord = vec2(texCoords.x - 0.5, texCoords.y - 0.5);
 	vs_out.normal = mat3(modelInv) * n;
}