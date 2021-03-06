#version 460 core
//in vec3 normal;
//in vec2 texCoords;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoords;

layout (binding = 0) uniform sampler2D imagePoint;
layout (binding = 1) uniform sampler2D imageNormal;
layout (binding = 2) uniform samplerCube skybox;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 modelInv;

out float Height;
out vec3 Normal;
out vec3 FragPos;

void main(){
	//ivec2 storePos = ivec2(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y));
    vec3 pos = texture(imagePoint, texCoords.xy).xyz;
    //pos.x = pos.x * 10;
    //pos.z = pos.z * 10;
    //pos.y = pos.y * 2;
    vec3 normal = texture(imageNormal, texCoords.xy).xyz;

    Normal = mat3(modelInv) * normal;
    Height = vec3(model * vec4( pos, 1.0f)).y;
    FragPos = vec3(model * vec4( pos, 1.0f));
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}
