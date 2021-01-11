#define _CRT_SECURE_NO_WARNINGS
#include"../include/objModel.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
void loadTexture(const char* textureFileName, unsigned int& texture);
objModel::objModel(const char* objFileName, const char* textureFileName,float factor) {
    //首先将obj文件转化为点集
    FILE* file = fopen(objFileName, "r");
    if (file == NULL) {
        cout << "Failed to open the obj file!" << endl;
        exit(0);
    }
    unsigned int vertexIndex[3], textureIndex[3], normalIndex[3];
    GLfloat x, y, z;
    GLfloat maxx, minx, maxy, miny, maxz, minz;
    maxx = minx = maxy = miny = maxz = minz = 0.0;

    while (1) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        if (strcmp(lineHeader, "v") == 0) {
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            final_vertices.push_back(x * factor);
            final_vertices.push_back(y * factor);
            final_vertices.push_back(z * factor);
            if (x > maxx) {
                maxx = x;
                x_max = final_vertices.size() - 3;
            }
            if (x < minx) {
                minx = x;
                x_min = final_vertices.size() - 3;
            }
            if (y > maxy) {
                maxy = y;
                y_max = final_vertices.size() - 2;
            }
            if (y < miny) {
                miny = y;
                y_min = final_vertices.size() - 2;
            }
            if (z > maxz) {
                maxz = z;
                z_max = final_vertices.size() - 1;
            }
            if (z < minz) {
                minz = z;
                z_min = final_vertices.size() - 1;
            }
        }
        //纹理
        else if (strcmp(lineHeader, "vt") == 0) {
            fscanf(file, "%f %f\n", &x, &y);
            final_textures.push_back(x);
            final_textures.push_back(1.0-y);
        }
        //由于绘画的时候是双面绘制，所以不用担心法向量
        else if (strcmp(lineHeader, "vn") == 0) {
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            final_normals.push_back(x*factor);
            final_normals.push_back(y*factor);
            final_normals.push_back(z*factor);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d",
                &vertexIndex[0], &textureIndex[0], &normalIndex[0],
                &vertexIndex[1], &textureIndex[1], &normalIndex[1],
                &vertexIndex[2], &textureIndex[2], &normalIndex[2]);
            //因为读入的obj文件已经进行了三角化操作，不必担心读入超过9个
            if (matches == 9) {
                //triangle
                vertexIndices.push_back(vertexIndex[0] - 1);
                vertexIndices.push_back(vertexIndex[1] - 1);
                vertexIndices.push_back(vertexIndex[2] - 1);
                textureIndices.push_back(textureIndex[0] - 1);
                textureIndices.push_back(textureIndex[1] - 1);
                textureIndices.push_back(textureIndex[2] - 1);
                normalIndices.push_back(normalIndex[0] - 1);
                normalIndices.push_back(normalIndex[1] - 1);
                normalIndices.push_back(normalIndex[2] - 1);
            }
            else {
                cout << "wrong obj structure!" << endl;
                exit(0);
            }
        }
        else {
            // Probably a comment, eat up the rest of the line
            char Buffer[1000];
            fgets(Buffer, 1000, file);
        }
    }

    //计算中心点
    center.x = (minx + maxx) / 2;
    center.y = (miny + maxy) / 2;
    center.z = (minz + maxz) / 2;

    //把每一个点分成 x y z position 和  x y texture
    for (int i = 0; i < vertexIndices.size(); i++) {
        vertexAndTextures.push_back(i);
        final_verticesTextures.push_back(final_vertices[3 * vertexIndices[i]]);
        final_verticesTextures.push_back(final_vertices[3 * vertexIndices[i] + 1]);
        final_verticesTextures.push_back(final_vertices[3 * vertexIndices[i] + 2]);
        final_verticesTextures.push_back(final_textures[2 * textureIndices[i]]);
        final_verticesTextures.push_back(final_textures[2 * textureIndices[i] + 1]);
    }

    //其次进行点集绑定，纹理绑定

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将顶点数据绑定至当前默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, final_verticesTextures.size() * sizeof(GLfloat), &final_verticesTextures[0], GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexAndTextures.size() * sizeof(unsigned int), &vertexAndTextures[0], GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // 纹理地址
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)* 3));
    glEnableVertexAttribArray(1);

    // 解绑VAO和VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    loadTexture(textureFileName, texture);
}
void objModel::draw(Shader& shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

void loadTexture(const char* textureFileName, unsigned int& texture){

    glGenTextures(2, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //加载纹理
    int width, height, nrchannels;//纹理长宽，通道数
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(textureFileName, &width, &height, &nrchannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);//释放资源
}
GLfloat objModel::getEdgePoint(int k) {
    switch (k) {
    case 0://最小x
        return final_vertices[x_min];
    case 1://最大x
        return final_vertices[x_max];
    case 2://最小y
        return final_vertices[y_min];
    case 3://最大y
        return final_vertices[y_max];
    case 4://最小z
        return final_vertices[z_min];
    case 5://最大z
        return final_vertices[z_max];
    default:
        cout << "error input!" << endl;
        return 0.0;
    }
}
glm::vec3 objModel::getCenter() {
    return center;
}
glm::vec3 objModel::getXMax() {
    return glm::vec3(final_vertices[x_max], final_vertices[x_max + 1.0], final_vertices[x_max + 2.0]);
}
glm::vec3 objModel::getXMin() {
    return glm::vec3(final_vertices[x_min], final_vertices[x_min + 1.0], final_vertices[x_min + 2.0]);
}
glm::vec3 objModel::getYMax() {
    return glm::vec3(final_vertices[y_max - 1.0], final_vertices[y_max], final_vertices[y_max + 1.0]);
}
glm::vec3 objModel::getYMin() {
    return glm::vec3(final_vertices[y_min - 1.0], final_vertices[y_min], final_vertices[y_min + 1.0]);
}
glm::vec3 objModel::getZMax() {
    return glm::vec3(final_vertices[z_max - 2.0], final_vertices[z_max - 1.0], final_vertices[z_max]);
}
glm::vec3 objModel::getZMin() {
    return glm::vec3(final_vertices[z_min - 2.0], final_vertices[z_min - 1.0], final_vertices[z_min]);
}