#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D boatTexture;

void main(){
	FragColor = texture(boatTexture,TexCoord);
}