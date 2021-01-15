#pragma once
#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include "objModel.h"
#include "Shader.h"
/// <summary>
/// For a Boat there are some important things
/// 1.all parts of the boat  done!
/// 2.speed and move 
/// 3.force analysis
/// 
/// </summary>
#define MAX_ANGLE 30
#define MAX_SPEED 0.4
class boat{
public:
	glm::vec2 speed;					//x为Front向前的速度，y为Front向右的速度
	float angle_side;					//船身倾斜角度 >0右 <0左
private:
	/*std::vector<objModel> MyBoatElements;
	std::vector<Shader> MyBoatShaders;*/

	//在整个运动中，只需要改变model来改变位置，改变Front来改变运动方向
	glm::mat4 model = glm::mat4(1.0f);	//公用矩阵
	objModel* MyBoat;
	Shader* MyShader;

	float waterSpeedDown;				//水中加速度
	float acclerate;					//加速度
	float tilt_rate;					//侧向旋转速率
	float tilt_rate_back;				//侧向转回速率
	float front_change_rate;			//向量变化速率

	glm::vec3 Translate;				//侧向转轴偏移
	glm::vec3 Front;					//最初的正方向 也是侧向的转轴
	glm::vec3 Right;					//右方向
	glm::vec3 Steering_shaft;			//左右转轴
	glm::vec3 temp0;
	void changeSpeedforWater(GLfloat gDeltaTime);		//水流对船的减速
	void modelTranChanges(GLfloat gDeltaTime);			//船的model矩阵的变化
	void modelSideRotaChanges(float angle);				//船身倾斜
	void changeFrontVector(float offset);				//改变Front
	float angleTurnRight(GLfloat gDeltaTime);			//向右转
	float angleTurnLeft(GLfloat gDeltaTime);			//向左转
	void buoyancy(GLfloat* points);						//水流对船上下移动的影响
	void buoyancyForSide(GLfloat angleTurn);			//水流对船左右摇晃的影响
	void backToFront();									//船身倾斜的时候回调函数
public:
	boat();
	void init();
	glm::vec3 Draw(glm::mat4 view, glm::mat4 projection,glm::vec3 CameraPos, glm::vec3 LightPos,glm::vec3 LightInfo,GLfloat LightIntensity, GLfloat gDeltaTime, GLfloat* points);
	void ProcessKeyboard(int Type,GLfloat gDeltaTime);
	void OutObj();
	glm::vec3 getCenter();
	glm::vec3 getFront();
	glm::vec3 getRight();
	glm::vec3 getFrontPoint();
	glm::vec3 getBackPoint();
	glm::vec3 getUpPoint();
	glm::vec3 getDownPoint();
	glm::vec3 getLeftPoint();
	glm::vec3 getRightPoint();
};
enum boatMovement { GOFORWARD, GOBACKWARD, TURNLEFT, TURNRIGHT };