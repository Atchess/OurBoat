#ifndef _OCEAN_H_
#define _OCEAN_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

using namespace std;
using namespace glm;
class Ocean
{
private:
    GLfloat g; // gravity constant
    GLfloat A;  // phillips spectrum parameter -- affects heights of waves
    GLfloat length;
    glm::vec2 windDirection;

    GLint N;
    GLint VN;
    
    GLfloat *vertices;                 // vertices for vertex buffer object
    GLfloat *vert;
    GLfloat *texCoords;
    GLfloat *point;
    GLfloat *dx;
    GLfloat *dz;
    GLuint *indices;                  // indicies for vertex buffer object
    GLuint indicesNum;             // number of indices to render
    GLuint verticesNum;
    GLuint VAO, VBO, EBO, TBO, FBO;             // vertex buffer objects

    GLuint textureH0, textureHt[3], textureNormal, textureIndices, texturePoint[2];  // store the result computed by compute shader
    GLuint textureHtSx[3], textureHtSz[3], textureHtDx[3], textureHtDz[3];
    GLuint vertexLocation ,texCoordsLocation;

    Shader *oceanShader;
    Shader *updateHtShader, *fftShader, *updateNormalShader;
	//声明进行渲染的顶点和像素着色器
    GLint vertex, normal, texCooed; // attributes and uniforms
    glm::vec3 deepWaterColorSunny = glm::vec3(powf(0.11f, 2.2f),
                                         powf(0.18f, 2.2f),
                                         powf(0.35f, 2.2f));
public:
    Ocean(GLfloat N, GLfloat A, GLfloat length, vec2 windDirection);
    ~Ocean();
    void FourierButterflyShuffleFFTi(GLint* src, GLint N);
    GLfloat PhillipsSpectrum(GLfloat A, GLfloat length, glm::vec2 waveDirection, glm::vec2 windDirection);
    GLfloat PhillipsSpectrum(int n, int m);
    void CreateGridPlane();
    GLboolean Init();
    GLboolean Update(GLfloat time, glm::mat4 Model, glm::mat4 View, glm::mat4 Perspective, glm::vec3 CamPos, GLuint ReflectText);
    void release();
};

#endif