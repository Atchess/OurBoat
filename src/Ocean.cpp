#include "../include/Ocean.h"

GLfloat RandomUniform()
{
	return ((GLfloat)rand() / RAND_MAX);
}
 
glm::vec2 RandomNormal()
{
    float x1, x2, w;
    do {
        x1 = 2.f * RandomUniform() - 1.f;
        x2 = 2.f * RandomUniform() - 1.f;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.f );
    w = sqrt((-2.f * log(w)) / w);
    return glm::vec2(x1 * w, x2 * w);
}


Ocean::Ocean(GLfloat N, GLfloat A, GLfloat length, vec2 windDirection) : g(9.81), N(N), A(A), length(length), windDirection(windDirection){
    Init();
}

Ocean::~Ocean()
{
	delete[] vertices;
	delete[] texCoords;
	delete[] indices;
}

GLfloat Ocean::PhillipsSpectrum(GLfloat A, GLfloat length, glm::vec2 wave, glm::vec2 wind)
{
	GLfloat k = glm::length(wave);
	GLfloat waveDotWind = glm::dot(wave, wind);
 
    GLfloat w_length  = glm::length(wind);
    GLfloat l         = w_length * w_length / g;

    GLfloat damping   = 0.001;
    GLfloat l2        = l * l * damping * damping;

	if (length == 0.0f || k == 0.0f)
	{
		return 0.0f;
	}
 
	return A * expf(-1.0f / (k * l * k * l)) / (k * k * k * k) * waveDotWind * waveDotWind * expf(-1 * k * k * l2);
}

GLfloat Ocean::PhillipsSpectrum(int n, int m) {
    vec2 k(M_PI * (2 * n - N) / length, M_PI * (2 * m - N) / length);
    float k_length  = glm::length(k);
    if (k_length < 0.000001) return 0.0;
    
    float k_length2 = k_length  * k_length;
    float k_length4 = k_length2 * k_length2;
    
    float k_dot_w   = glm::dot(glm::normalize(k), glm::normalize(windDirection));
    float k_dot_w2  = k_dot_w * k_dot_w;
    
    float w_length  = glm::length(windDirection);
    float L         = w_length * w_length / g;
    float L2        = L * L;
    
    float damping   = 0.001;
    float l2        = L2 * damping * damping;
    
    return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}

void Ocean::CreateGridPlane()
{	
	VN = 2 * N;
	// 计算用的数据
	verticesNum = (N + 1) * (N + 1);
    vert = new GLfloat[verticesNum * 4];
	texCoords = new GLfloat[verticesNum * 2];
	point = new GLfloat[verticesNum * 4];
    
    verticesNum = (VN + 1) * (VN + 1);
    indicesNum  = 6 * (VN + 1) * (VN + 1);
    // 渲染用的数据
	vertices = new GLfloat[verticesNum * 5];	
	indices  = new GLuint[indicesNum];
    indicesNum = 0;

    for (unsigned int i = 0; i < VN + 1; ++i) {
        for (unsigned int j = 0; j < VN + 1; ++j) {
            int index = i * (VN + 1) + j;
            indices[indicesNum ++] = index;
            indices[indicesNum ++] = index + VN + 1;
            indices[indicesNum ++] = index + VN + 1 + 1;
            indices[indicesNum ++] = index;
            indices[indicesNum ++] = index + VN + 1 + 1;
            indices[indicesNum ++] = index + 1;
        }
    }
    for (int i = 0; i < VN + 1; ++i) {
        for (int j = 0; j < VN + 1; ++j) {
            int pos = 5 * (i * (VN + 1) + j);
            vertices[pos + 0] = (i - VN / 2.0) * length / VN;
            vertices[pos + 1] = 0.0f;
            vertices[pos + 2] = (j - VN / 2.0) * length / VN;
            vertices[pos + 3] = (GLfloat)i / (GLfloat)(VN);
            vertices[pos + 4] = (GLfloat)j / (GLfloat)(VN);
        }
    }

    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < N + 1; ++j) {
            int pos = 2 * (i * (N + 1) + j);
            texCoords[pos + 0] = (GLfloat)i / (GLfloat)(N + 1);
            texCoords[pos + 1] = (GLfloat)j / (GLfloat)(N + 1);
        }
    }

    for (int i = 0; i < N + 1; ++i) {
        for (int j = 0; j < N + 1; ++j) {
            int pos = 4 * (i * (N + 1) + j);
            vert[pos + 0] = (i - N / 2.0) * length / N;
            vert[pos + 1] = 0.0f;
            vert[pos + 2] = (j - N / 2.0) * length / N;
            vert[pos + 3] = 0.0f;
        }
    }

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesNum * 5 * sizeof(float), vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesNum * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(texCoordsLocation);
    glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	// 解绑VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


GLboolean Ocean::Init()
{
	lightIntensity = 1.0;
	lightPos = glm::vec3(-100.0f, 100.0f, -100.0f);

	//oceanShader = new Shader("./wave/shader/terrain.vs", "./wave/shader/terrain.frag");
	oceanShader = new Shader("./wave/shader/water.vs", "./wave/shader/water.fs");
	vertexLocation = glGetAttribLocation(oceanShader->ID, "aPos");
    texCoordsLocation = glGetAttribLocation(oceanShader->ID, "texCoords");
    CreateGridPlane();

	GLfloat* h0Data;
 
	GLfloat phillipsSpectrumValue;
	//通过项数计算出计算 FFT 的时候需要的迭代总次数
	GLint steps = 0;
	GLint temp = N;
	while (!(temp & 0x1))
	{
		temp = temp >> 1;
		steps++;
	}
	//声明更新海面网格的计算着色器
	updateHtShader = new Shader("./wave/shader/OceanUpdate.comp");
	//声明进行FFT的计算着色器
	fftShader = new Shader("./wave/shader/OceanFFT.comp");
	//声明更新法线向量的计算着色器
	updateNormalShader = new Shader("./wave/shader/OceanUpdateNormal.comp");

	//h0Data存储波浪模型的初始值
	h0Data = (GLfloat*)malloc((N + 1) * (N + 1) * 4 * sizeof(GLfloat));
	if (!h0Data) {
		return GL_FALSE;
	}
	//Phillips频谱计算出初始波形
	//windDirection = glm::normalize(windDirection);
	for (int i = 0; i < N + 1; i++) {
		for (int k = 0; k < N + 1; k++) {
            int index = (i * (N + 1) + k) * 4;
			phillipsSpectrumValue = PhillipsSpectrum(k, i);
			glm::vec2 temp = RandomNormal();
            temp = temp * sqrt(phillipsSpectrumValue / 2.0f);
            h0Data[index + 0] = temp[0];
			h0Data[index + 1] = temp[1];
            phillipsSpectrumValue = PhillipsSpectrum(-k, -i);
            temp = RandomNormal();
            temp = temp * sqrt(phillipsSpectrumValue / 2.0f);
            h0Data[index + 2] = temp[0];
			h0Data[index + 3] = -temp[1];
		}
	}

	//将数据存储到一张贴图当中，textureH0贴图的R、G两个通道分别存储h0Data（相当于复数）的实部和虚部
	glGenTextures(1, &textureH0);
	glBindTexture(GL_TEXTURE_2D, textureH0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N + 1, N + 1, 0, GL_RGBA, GL_FLOAT, h0Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	free(h0Data);

	//用于存储波浪函数计算出来的复数值的贴图
    for (int i = 0; i < 3; i++) {
        glGenTextures(1, &textureHt[i]);
	    glBindTexture(GL_TEXTURE_2D, textureHt[i]);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

	//用于存储波浪函数计算出来的复数值的贴图
    for (int i = 0; i < 3; i++) {
	    glGenTextures(1, &textureHtSx[i]);
	    glBindTexture(GL_TEXTURE_2D, textureHtSx[i]);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

	//用于存储波浪函数计算出来的复数值的贴图
    for (int i = 0; i < 3; i++) {
	    glGenTextures(1, &textureHtSz[i]);
	    glBindTexture(GL_TEXTURE_2D, textureHtSz[i]);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

	//用于存储波浪函数计算出来的复数值的贴图
    for (int i = 0; i < 3; i++) {
	    glGenTextures(1, &textureHtDx[i]);
	    glBindTexture(GL_TEXTURE_2D, textureHtDx[i]);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

	//用于存储波浪函数计算出来的复数值的贴图
    for (int i = 0; i < 3; i++) {
	    glGenTextures(1, &textureHtDz[i]);
	    glBindTexture(GL_TEXTURE_2D, textureHtDz[i]);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
 
	//用于存储顶点偏移值的贴图（按行计算得到的值）
	glGenTextures(1, &texturePoint[0]);
	glBindTexture(GL_TEXTURE_2D, texturePoint[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N + 1, N + 1, 0, GL_RGBA, GL_FLOAT, vert);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    free(vert);
 
	//用于存储顶点偏移值的贴图（按列计算得到的值）
	glGenTextures(1, &texturePoint[1]);
	glBindTexture(GL_TEXTURE_2D, texturePoint[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N + 1, N + 1, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
	//存储法线向量值
	glGenTextures(1, &textureNormal);
	glBindTexture(GL_TEXTURE_2D, textureNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	//将所需要的迭代次数传递到FFT的计算着色器中
	fftShader->use();
    fftShader->setInt("steps", steps);
	glUseProgram(0);
 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	return GL_TRUE;
}
 
GLboolean Ocean::Update(GLfloat time, glm::mat4 Model, glm::mat4 ModelInv, glm::mat4 View, glm::mat4 Projection, glm::vec3 CamPos, GLuint ReflectText)
{
	
	static GLfloat totalTime = 0.0f;
	//使用Update计算着色器并绑定初始值贴图和即时值贴图
	updateHtShader->use();
	glBindImageTexture(0, textureH0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, textureHt[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(2, textureHtSx[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(3, textureHtSz[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(4, textureHtDx[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(5, textureHtDz[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);

    //传递运行时间
    updateHtShader->setFloat("totalTime", totalTime);
	//创建一个N*N大小的工作组，即同时计算所有的顶点高度值
	glDispatchCompute(N, N, 1);
	//确保所有的数据都写入到贴图里了
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

//////////////////////////////////////////////////////////////////////////////////////
	//使用FFT计算着色器对结果进行逆变换
	fftShader->use();
	//绑定索引贴图、上个计算着色器所得结果的波浪函数贴图、将要存储偏移值的贴图
	glBindImageTexture(0, textureHt[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHt[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 0);
	// 先对每一行进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
 
	glBindImageTexture(0, textureHt[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHt[2], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 1);
	// 再对每一列进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

/////////////////////////////////////////////////////////////////////////////////////
	glBindImageTexture(0, textureHtSx[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtSx[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 0);
	// 先对每一行进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
 
	glBindImageTexture(0, textureHtSx[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtSx[2], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 1);
	// 再对每一列进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

//////////////////////////////////////////////////////////////////////////////////////
	glBindImageTexture(0, textureHtSz[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtSz[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 0);
	// 先对每一行进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
 
	glBindImageTexture(0, textureHtSz[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtSz[2], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 1);
	// 再对每一列进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

////////////////////////////////////////////////////////////////////////////////////////
	glBindImageTexture(0, textureHtDx[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtDx[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 0);
	// 先对每一行进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
 
	glBindImageTexture(0, textureHtDx[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtDx[2], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 1);
	// 再对每一列进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

////////////////////////////////////////////////////////////////////////////////////////
	glBindImageTexture(0, textureHtDz[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtDz[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 0);
	// 先对每一行进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
 
	glBindImageTexture(0, textureHtDz[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtDz[2], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    fftShader->setInt("processColumn", 1);
	// 再对每一列进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


////////////////////////////////////////////////////////////////////////////////////////
	//更新法线向量
	updateNormalShader->use();
	glBindImageTexture(0, textureHt[2], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, textureHtSx[2], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, textureHtSz[2], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(3, textureHtDx[2], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(4, textureHtDz[2], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(5, texturePoint[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(6, texturePoint[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(7, textureNormal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(N, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// 传出水面数据
	glBindTexture(GL_TEXTURE_2D, texturePoint[1]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, point);
	glBindTexture(GL_TEXTURE_2D, 0);

	//GLfloat *data  = new GLfloat[(N + 1) * (N + 1) * 4];
	//glBindTexture(GL_TEXTURE_2D, texturePoint[1]);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//Model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));
 	oceanShader->use();
	oceanShader->setMat4("model", Model);
    oceanShader->setMat4("view", View);
    oceanShader->setMat4("projection", Projection);
    oceanShader->setVec3("viewPos", CamPos);
	oceanShader->setMat4("modelInv", ModelInv);

    oceanShader->setVec3("lightDir", glm::vec3(1.0f, -1.0f, 1.0f));
    oceanShader->setVec3("lightPos", lightPos);
    oceanShader->setVec3("diffuse", deepWaterColorSunny);
    oceanShader->setVec3("ambient", deepWaterColorSunny);
    oceanShader->setVec3("specular", glm::vec3(1.0f, 1.0f, 1.0f));


	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verticesNum * 5 * sizeof(float), vertices);
    glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(texCoordsLocation);
    glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	//将波浪高度贴图绑定在GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturePoint[1]);
	//将法线贴图绑定在GL_TEXTURE1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ReflectText);
 
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, ReflectText);
	//根据索引开始绘制
	glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
	//重置GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//重置GL_TEXTURE1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	totalTime += time;
	return GL_TRUE;
}


GLfloat* Ocean::getVertices() 
{
	return point;
}

GLfloat Ocean::getLightIntensity()
{
	return lightIntensity;
}

void Ocean::setLightIntensity(GLfloat intensity)
{
	lightIntensity = intensity;
}

glm::vec3 Ocean::getLightPos()
{
	return lightPos;
}

void Ocean::setLightPos(glm::vec3 pos)
{
	lightPos = pos;
}