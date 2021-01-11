#version 460 core

out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube skybox;

uniform float lightIntensity;

void main() {
	fragColor = texture(skybox, texCoord) * lightIntensity;
}
