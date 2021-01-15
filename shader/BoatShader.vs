#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 Ambient;
out vec4 Diffuse;
out vec4 Specular;
out float Shine;

uniform vec4 aDiffuse;
uniform vec4 aAmbient;
uniform vec4 aSpecular;
uniform float aShine;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos=vec3(model*vec4(aPos,1.0));
	TexCoord=vec2(aTexCoord.x,aTexCoord.y);
	Normal=mat3(transpose(inverse(model)))*aNormal;
	Ambient=aAmbient;
	Diffuse = aDiffuse;
	Specular = aSpecular;
    Shine = aShine;

	//gl_Position = projection * view * vec4(FragPos,1.0);
	gl_Position=projection*view*model*vec4(aPos,1.0);
}