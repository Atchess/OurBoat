#include"../include/boat.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
boat::boat(){
	waterSpeedDown = -0.05;
	angle_side = 0;
	acclerate = 0.08f;
	tilt_rate = 10.0f;
	tilt_rate_back = 2 * tilt_rate;//���ݴ���
	front_change_rate = 8.0f;
	speed = glm::vec2(0.0f, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, 1.0f);
	Translate = glm::vec3(0.0f, 0.0f, 0.0f);
	Steering_shaft = glm::vec3(0.0f, 1.0f, 0.0f);
}
void boat::init() {
	MyBoat = new objModel("wave/obj/boat2.obj", "wave/textures/boat/boat1.bmp", 1);
	MyShader = new Shader("wave/shader/BoatShader.vs", "wave/shader/BoatShader.fs");
	Translate.y = MyBoat->getYMin() * 1.5;
}
void boat::Draw(glm::mat4 view, glm::mat4 projection,GLfloat gDeltaTime){
	changeSpeedforWater(gDeltaTime);
	modelTranChanges(gDeltaTime);
	MyShader->use();
	MyBoat->draw(*MyShader, model, view, projection);

	//����Ϊ�Լ����ƵĴ�
	/*for (auto shader : MyBoatShaders) {
		shader.use();
		shader.setInt("boatTexture", 0);
	}
	for (int i = 0; i < MyBoatElements.size() ; i++) {
		MyBoatElements[i].draw(MyBoatShaders[i], model, view, projection);
	}*/
	//TODO 
	if (angle_side >= 45.0f || angle_side <= -45.0f) {
		cout << "GAME OVER" << endl;
	}

	//cout << model[1][3] << endl;
	//cout << "x:" << Front.x <<"y:"<<Front.y<< "z:" << Front.z << endl;
}
void boat::ProcessKeyboard(int Type, GLfloat gDeltaTime) {
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
	glm::vec3 Right = glm::normalize(glm::vec3(temp * glm::vec4(Front, 1.0f)));
	//cout << "Front.y: "<<Front.y << endl;
	//cout << "Right.y: "<<Right.y << endl;
	model = glm::translate(model, speed.x * Front + speed.y * Right);
}

void boat::modelSideRotaChanges(float angle) {
	//cout <<"angle: "<<angle_side << endl;
	//model = glm::translate(model, getCenter());
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, Translate);
	temp = glm::rotate(temp, glm::radians(angle), Front);
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
	temp1 = glm::rotate(temp1, glm::radians(offset), Steering_shaft);
	model = temp1 * model;

	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::rotate(temp, glm::radians(offset), Steering_shaft);
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
glm::vec3 boat::getCenter() {
	return glm::vec3(model * glm::vec4(MyBoat->getCenter(), 1.0f));
}
glm::vec3 boat::getFront(){
	return Front;
}