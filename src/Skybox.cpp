//
// Created by 何昊 on 01/03/2018.
//

#include "../include/Skybox.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

// Initialize skybox
static float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

Skybox::Skybox(std::vector<std::string> paths)
{
    texture = GenCubeMap(paths);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    lightIntensity = 1.0;
}

void Skybox::Draw(Shader &shader, glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0, -0.5, 0));
    shader.use();
    shader.setMat4("view", skyboxView);
    shader.setMat4("projection", projection);
    shader.setMat4("model", model);
    shader.setInt("skybox", 0);
    shader.setFloat("lightIntensity", lightIntensity);
    glDepthMask(GL_FALSE);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

unsigned int Skybox::GenCubeMap(std::vector<std::string> facePaths)
{
    if (facePaths.size() < 6) {
        std::cout << "Cube map texture not enough! " << std::endl;
    }

    unsigned int tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < facePaths.size(); ++i) {
        unsigned char *image = stbi_load(facePaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (image) {
            if (nrChannels == 4) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            } else if (nrChannels == 3) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            } else {
                std::cout << "The channel of this image is not 4 or 3: " << facePaths[i] << std::endl;
            }
            stbi_image_free(image);
        }
        else {
            std::cout << "Cube map texture failed to load at path: " << facePaths[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return tid;
}

GLfloat Skybox::getLightIntensity()
{
	return lightIntensity;
}

void Skybox::setLightIntensity(GLfloat intensity)
{
	lightIntensity = intensity;
}