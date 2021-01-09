#version 460 core
//in vec3 normal;
//in vec2 texCoords;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoords;

layout (binding = 0) uniform sampler2D imagePoint;
layout (binding = 1) uniform sampler2D imageNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out float Height;
out vec3 aNormal;
out vec3 FragPos;

void main(){
	//ivec2 storePos = ivec2(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y));
    vec3 pos = texture(imagePoint, texCoords.xy).xyz;
    vec3 normal = texture(imageNormal, texCoords.xy).xyz;

    aNormal = mat3(transpose(inverse(model))) * normal;
    Height = vec3(model * vec4( pos, 1.0f)).y;
    FragPos = vec3(model * vec4( pos, 1.0f));
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}
