#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<iostream>
#include"Shader.h"
/// <summary>
/// For a objModel,only two important
/// vertices texture
/// </summary>
class objModel {
private:
	//VAO VBO 顶点绑定 EBO 顺序绑定 texture纹理
	unsigned int VAO, VBO, EBO, texture;
	glm::vec3 center;
	int x_max, x_min, y_max, y_min, z_max, z_min;//边缘索引
	std::vector<int> vertexIndices, textureIndices, vertexAndTextures,normalIndices;
	std::vector<GLfloat> final_vertices; //glm::vec3
	std::vector<GLfloat> final_textures; //glm:vec2 
	std::vector<GLfloat> final_verticesTextures; //glm::vec3 and glm::vec2
	std::vector<GLfloat> final_normals; //glm::vec3 双面绘制，不必担心法向量
public:
	objModel(const char* objFileName, const char* textureFileName, float factor);
	void draw(Shader& shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
	GLfloat getEdgePoint(int k);
	glm::vec3 getCenter();
	float getXMax();
	float getXMin();
	float getYMax();
	float getYMin();
	float getZMax();
	float getZMin();
};

