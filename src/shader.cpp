#include "../include/shader.h"

Shader::Shader(const GLchar* vertexPath,const GLchar* fragmentPath,const GLchar* geometryPath){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    try{
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream,fShaderStream;
        vShaderStream<<vShaderFile.rdbuf();
        fShaderStream<<fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode=vShaderStream.str();
        fragmentCode=fShaderStream.str();
    }
    catch(std::ifstream::failure e){
        std::cout<<"ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ"<<std::endl;
    }
    const char* vShaderCode=vertexCode.c_str();
    const char* fShaderCode=fragmentCode.c_str();

    int success;
    char infoLog[512];

    //编译定点着色器
    GLuint vertexShaderobj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderobj, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShaderobj);
    glGetShaderiv(vertexShaderobj, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShaderobj, 512, nullptr, infoLog);
        std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }

    //编译片元着色器
    GLuint fragmentShaderobj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderobj, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShaderobj);
    glGetShaderiv(fragmentShaderobj, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShaderobj, 512, nullptr, infoLog);
        std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }

    
    GLuint geometryShaderobj;
    
    
    //链接着色器程序
    this->ID = glCreateProgram();
    glAttachShader(ID, vertexShaderobj);
    glAttachShader(ID, fragmentShaderobj);
    
    if(geometryPath){
        
        std::string geomertryCode;
        std::ifstream gShaderFile;
        std::stringstream gShaderStream;
        
        gShaderFile.open(geometryPath);
        gShaderStream<<gShaderFile.rdbuf();
        gShaderFile.close();
        geomertryCode=gShaderStream.str();
        const char* gShaderCode=geomertryCode.c_str();
        
        //编译几何着色器
        geometryShaderobj=glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShaderobj, 1, &gShaderCode, nullptr);
        glCompileShader(geometryShaderobj);
        glGetShaderiv(geometryShaderobj, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(geometryShaderobj, 512, nullptr, infoLog);
            std::cout<<"ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"<<infoLog<<std::endl;
        }
        glAttachShader(ID, geometryShaderobj);
    }
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout<<"ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }
    
    glDeleteShader(vertexShaderobj);
    glDeleteShader(fragmentShaderobj);
    if (geometryPath) {
        glDeleteShader(geometryShaderobj);
    }
}

Shader::Shader(const GLchar* compPath){
    std::string computeCode;
    std::ifstream cShaderFile;
    cShaderFile.exceptions(std::ifstream::badbit);
    try{
        cShaderFile.open(compPath);
        std::stringstream cShaderStream;
        cShaderStream<<cShaderFile.rdbuf();
        cShaderFile.close();
        computeCode=cShaderStream.str();
    }
    catch(std::ifstream::failure e){
        std::cout<<"ERROR::COMPUTE SHADER::FILE_NOT_SUCCESFULLY_READ"<<std::endl;
    }
    const char* cShaderCode=computeCode.c_str();

    int success;
    char infoLog[512];

    //编译计算着色器
    GLuint computerShaderobj=glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computerShaderobj,1,&cShaderCode,NULL);
    glCompileShader(computerShaderobj);
    glGetShaderiv(computerShaderobj,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(computerShaderobj,512,NULL,infoLog);
        std::cout<<"ERROR::SHADER::COMPUTER::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }
    this->ID = glCreateProgram();
    glAttachShader(ID, computerShaderobj);
    glLinkProgram(ID);
    glDeleteShader(computerShaderobj);
}


void Shader::use(){
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value)const{
    glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}

void Shader::setInt(const std::string &name, int value)const{
    glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setFloat(const std::string &name, float value)const{
    glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setVec2(const std::string &name,float x,float y)const{
    glUniform2f(glGetUniformLocation(ID,name.c_str()),x,y);
}
void Shader::setVec2(const std::string &name,glm::vec2 value)const{
    glUniform2fv(glGetUniformLocation(ID,name.c_str()),1,glm::value_ptr(value));
}
void Shader::setVec3(const std::string &name, float x, float y, float z)const{
    glUniform3f(glGetUniformLocation(ID,name.c_str()),x,y,z);
}
void Shader::setVec3(const std::string &name,glm::vec3 value)const{
    glUniform3fv(glGetUniformLocation(ID,name.c_str()),1,glm::value_ptr(value));
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)const{
    glUniform4f(glGetUniformLocation(ID,name.c_str()),x,y,z,w);
}
void Shader::setVec4(const std::string &name,glm::vec4 value)const{
    glUniform3fv(glGetUniformLocation(ID,name.c_str()),1,glm::value_ptr(value));
}

void Shader::setMat3(const std::string &name,const glm::mat3& matrix)const{
    glUniformMatrix3fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(matrix));
}

void Shader::setMat4(const std::string &name,const glm::mat4& matrix)const{
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(matrix));
}

void Shader::close(){
    glUseProgram(0);
}

void Shader::deleteShader(){
    glDeleteProgram(ID);
}

GLuint generalTex(const std::string& name,int& width,int& height,int& nrChannels){
    GLuint textures;
    glGenTextures(1, &textures);
    
    glBindTexture(GL_TEXTURE_2D, textures);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unsigned char* image=stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
    if(!image){
        std::cout<<"texture error to load"<<std::endl;
        return -1;
    }
//    if (nrChannels==1) {
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//    }else
    if(nrChannels==3){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }else if (nrChannels==4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return textures;
}

GLint Shader::getAttibLoaction(const std::string& name)const{
    return glGetAttribLocation(ID,name.c_str());
}











