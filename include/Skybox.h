#ifndef PROJECT_SKYBOX_H
#define PROJECT_SKYBOX_H

#include <iostream>
#include <vector>
#include <string>

#include "Shader.h"

class Skybox
{
public:
    Skybox(std::vector<std::string> paths);

    void Draw(Shader &shader, glm::mat4 view, glm::mat4 projection);

    unsigned int getCubeMap() { return texture; }
private:
    GLfloat lightIntensity;
    unsigned int VAO, VBO, texture;

    unsigned int GenCubeMap(std::vector<std::string> facePaths);

    GLfloat getLightIntensity();

    void setLightIntensity(GLfloat intensity);
};


#endif //PROJECT_SKYBOX_H
