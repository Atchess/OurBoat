#include"../include/boat.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
boat::boat(){
	//TODO��һ�����ʵ��ٶ�
	waterSpeedDown = -0.05;
	angle_side = 0;
	acclerate = 0.08f;
	tilt_rate = 10.0f;
	tilt_rate_back = 2 * tilt_rate;//���ݴ���
	front_change_rate = 20.0f;
	speed = glm::vec2(0.0f, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, 1.0f);
	Right = glm::vec3(-1.0f, 0.0f, 0.0f);
	Translate = glm::vec3(0.0f, 0.0f, 0.0f);
	Steering_shaft = glm::vec3(0.0f, 1.0f, 0.0f);
}
void boat::init() {
	MyBoat = new objModel("wave/obj/boat2.obj", "wave/textures/boat/boat1.bmp", 1);
	MyShader = new Shader("wave/shader/BoatShader.vs", "wave/shader/BoatShader.fs");
	Translate.y = MyBoat->getYMin().y * 1.5;
	

}
glm::vec3 boat::Draw(glm::mat4 view, glm::mat4 projection,GLfloat gDeltaTime,GLfloat * points){
	glm::vec3 temp1 = getCenter();
	//cout << "temp1 : " << temp1.y << endl;
	modelTranChanges(gDeltaTime);
	glm::vec3 a = getCenter();
	//cout << " a: " << a.y << endl;
	changeSpeedforWater(gDeltaTime);
	glm::vec3 b = getCenter();
	//cout << " b: " << b.y << endl;
	buoyancy(points);
	glm::vec3 c = getCenter();
	//cout << " c: " << c.y << endl;
	backToFront();
	MyShader->use();
	MyBoat->draw(*MyShader, model, view, projection);

	//TODO 
	if (angle_side >= 45.0f || angle_side <= -45.0f) {
		cout << "GAME OVER" << endl;
	}
	temp0+=getCenter() - temp1;
	return getCenter()-temp1;
	//cout << model[1][3] << endl;
	//cout << "x:" << Front.x <<"y:"<<Front.y<< "z:" << Front.z << endl;
}
void boat::backToFront() {
	float k = 0.0f;
	if (angle_side != 0)
		k = (abs(angle_side) / (angle_side)) * sqrt(abs(angle_side));
	else k = 0.0;
	angle_side -= k;
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, Translate);
	temp = glm::rotate(temp, -(GLfloat)(glm::radians(k)), Front);
	temp = glm::translate(temp, -Translate);
	model = temp * model;
}
void boat::ProcessKeyboard(int Type, GLfloat gDeltaTime) {
	//waterSpeedDown = -0.05 - abs(speed.x);
	if (Type == GOFORWARD) {
		speed.x += acclerate * gDeltaTime;
	}
	else if (Type == GOBACKWARD) {
		speed.x -= acclerate * gDeltaTime;
	}
	else if (Type == TURNLEFT) {
		speed.y -= acclerate * gDeltaTime;
		changeFrontVector(front_change_rate * gDeltaTime);
		modelSideRotaChanges(angleTurnLeft(gDeltaTime));
	}
	else if (Type == TURNRIGHT) {
		speed.y += acclerate * gDeltaTime;
		changeFrontVector(-front_change_rate * gDeltaTime);
		modelSideRotaChanges(angleTurnRight(gDeltaTime));
	}
	else {
		cout << "error!" << endl;
	}

}
float boat::angleTurnRight(GLfloat gDeltaTime) {
	float temp = angle_side;
	if (angle_side >= 0)
		angle_side += gDeltaTime * tilt_rate;
	else {
		if (abs(angle_side) > abs(gDeltaTime * tilt_rate_back))
			angle_side += gDeltaTime * tilt_rate_back;
		else 
			angle_side = (gDeltaTime + (angle_side / tilt_rate_back)) * tilt_rate;
	}
	return angle_side - temp;
}

float boat::angleTurnLeft(GLfloat gDeltaTime) {
	float temp = angle_side;
	if (angle_side <= 0) 
		angle_side -= gDeltaTime * tilt_rate;
	else {
		if (abs(angle_side) > abs(gDeltaTime * tilt_rate_back))
			angle_side -= gDeltaTime * tilt_rate_back;
		else
			angle_side = -(gDeltaTime - (angle_side / tilt_rate_back)) * tilt_rate;
	}
	return angle_side - temp;
}

void boat::modelTranChanges(GLfloat gDeltaTime){
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::rotate(temp, glm::radians(-90.0f), Steering_shaft);
	Right = glm::normalize(glm::vec3(temp * glm::vec4(Front, 1.0f)));
	//cout << "Front.y: "<<Front.y << endl;
	//cout << "Right.y: "<<Right.y << endl;
	temp = glm::mat4(1.0f);
	temp = glm::translate(temp, speed.x * Front + speed.y * Right);
	model = temp * model;
}

void boat::modelSideRotaChanges(float angle) {
	//cout <<"angle: "<<angle_side << endl;
	//model = glm::translate(model, getCenter());
	//cout << " x: " << temp0.x << " y: " << temp0.y << " z: " << temp0.z << endl;
	//cout << glm::radians(angle) / (1 + pow(temp0.x, 2) + pow(temp0.z, 2)) << endl;
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, Translate);
	temp = glm::rotate(temp, (GLfloat)(glm::radians(angle)), Front);
	temp = glm::translate(temp, -Translate);
	model = temp * model;

	//cout << "2 x:" << Front.x << "y:" << Front.y << "z:" << Front.z << endl;
	//model = glm::translate(model, -getCenter());
	//Translate.x = -Front.z * sin(abs(angle_side) * acos(-1) * 180);
	//Translate.y = MyBoat->getYMin() * 1.5 * cos(angle_side * acos(-1) / 180);
	//Translate.z = -Front.x * sin(abs(angle_side) * acos(-1) * 180);
	//cout << "y:" <<Translate.y << endl;

}

void boat::changeFrontVector(float offset) {
	glm::mat4 temp1 = glm::mat4(1.0f);
	temp1 = glm::translate(temp1,temp0);
	temp1 = glm::rotate(temp1, glm::radians(offset), Steering_shaft);
	temp1 = glm::translate(temp1, -temp0);
	model = temp1 * model;

	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, temp0);
	temp = glm::rotate(temp, glm::radians(offset), Steering_shaft);
	temp = glm::translate(temp, -temp0);
	Front = glm::normalize(glm::vec3(temp * glm::vec4(Front, 1.0f)));
	//cout << "1 x:" << Front.x << "y:" << Front.y << "z:" << Front.z << endl;
	//cout << "x:" << Front.x << "y:" << Front.y << "z:" << Front.z << endl;
}
void boat::changeSpeedforWater(GLfloat gDeltaTime) {
	GLfloat gDeltaSpeed = gDeltaTime * waterSpeedDown;
	if (speed.y == 0 && speed.x != 0) {//��ǰ�ٶ�
		int i = speed.x / abs(speed.x);
		if (abs(speed.x) >= abs(gDeltaSpeed))
			speed.x += i * gDeltaSpeed;
		else
			speed.x = 0;
	}
	else if (speed.x == 0 && speed.y != 0) {//�����ٶ�
		int i = speed.y / abs(speed.y);
		if (abs(speed.y) >= abs(gDeltaSpeed))
			speed.y += i * gDeltaSpeed;
		else
			speed.y = 0;
	}
	else if (speed.x != 0 && speed.y != 0) {//ǰ�Ҷ���
		double angle = atan(abs(speed.y / speed.x));//�ٶȷֽ�
		GLfloat gDeltaSpeedForword = gDeltaTime * cos(angle) * waterSpeedDown;
		GLfloat gDeltaSpeedRight = gDeltaTime * sin(angle) * waterSpeedDown;
		if (abs(speed.x) >= abs(gDeltaSpeedForword))
			speed.x += (speed.x / abs(speed.x)) * gDeltaSpeedForword;
		else
			speed.x = 0;
		if (abs(speed.y) >= abs(gDeltaSpeedRight))
			speed.y += (speed.y / abs(speed.y)) * gDeltaSpeedRight;
		else
			speed.y = 0;
	}
	else {
		return;
	}
}
float getOceanY(glm::vec3 point, GLfloat* points) {
	//const int N = 1024;//��ĸ���
	int x, z;
	x = point.x;
	z = point.z;
	//cout << " x: " << x << " z: " << z << endl;
	//cout << "ocean x:" << points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 0]
	//	<< "ocean y:" << points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 1]
	//	<< "ocean z:" << points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 2] << endl;
	return points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 1];
}

void boat::buoyancy(GLfloat* points) {
	float y[5];
	y[0] = getOceanY(getFrontPoint(), points);
	y[1] = getOceanY(getBackPoint(), points);
	y[2] = getOceanY(getLeftPoint(), points);
	y[3] = getOceanY(getRightPoint(), points);
	y[4] = getOceanY(getCenter(), points);
	float k = -1000.0;
	for (int i = 0; i < 5; i++) {
		if (y[i] >= k)k = y[i];
	}
	glm::mat4 temp1 = glm::mat4(1.0f);
	temp1 = glm::translate(temp1, glm::vec3(0.0f, 0.3f+k-getCenter().y, 0.0f));
	model = temp1 * model;

	float temp2 = y[2] - y[3];//0.2����
	buoyancyForSide(temp2*30);
}
void boat::buoyancyForSide(GLfloat angleTurn) {
	while (abs(angle_side + angleTurn) > MAX_ANGLE)angleTurn /= 2;
	angle_side += angleTurn;
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, Translate);
	temp = glm::rotate(temp, (GLfloat)(glm::radians(angleTurn)), Front);
	temp = glm::translate(temp, -Translate);
	model = temp * model;
}

glm::vec3 boat::getCenter() {
	return glm::vec3(model * glm::vec4(MyBoat->getCenter(), 1.0f));
}
glm::vec3 boat::getFront(){
	return Front;
}
glm::vec3 boat::getRight() {
	return Right;
}
glm::vec3 boat::getFrontPoint() {
	return glm::vec3(model * glm::vec4(MyBoat->getZMax(), 1.0f));
}
glm::vec3 boat::getBackPoint() {
	return glm::vec3(model * glm::vec4(MyBoat->getZMin(), 1.0f));
}
glm::vec3 boat::getUpPoint() {
	return glm::vec3(model * glm::vec4(MyBoat->getYMax(), 1.0f));
}
glm::vec3 boat::getDownPoint() {
	return glm::vec3(model * glm::vec4(MyBoat->getYMin(), 1.0f));
}
glm::vec3 boat::getLeftPoint() {
	return glm::vec3(model * glm::vec4(MyBoat->getXMin(), 1.0f));
}
glm::vec3 boat::getRightPoint() {
	return glm::vec3(model * glm::vec4(MyBoat->getXMax(), 1.0f));
}

glm::vec2 boat::getSpeed() 
{
	return speed;
}