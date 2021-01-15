#define _CRT_SECURE_NO_WARNINGS
#include"../include/boat.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
boat::boat(){
	//TODO找一个合适的速度
	waterSpeedDown =  -0.004f;
	angle_side = 0;
	acclerate =  0.008f;
	tilt_rate = 55.0f;
	tilt_rate_back = 2 * tilt_rate;//数据待定
	front_change_rate = 20.0f;
	speed = glm::vec2(0.0f, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, 1.0f);
	Right = glm::vec3(-1.0f, 0.0f, 0.0f);
	Translate = glm::vec3(0.0f, 0.0f, 0.0f);
	Steering_shaft = glm::vec3(0.0f, 1.0f, 0.0f);
}
void boat::init() {
	//Elements: 船底 船身 桅杆 船帆 贴字
	//objModel bottomModel("../obj/bottom.obj", "../textures/boat/bottom.bmp");
	//Shader bottomShader("../shader/BoatShader.vs", "../shader/BoatShader.fs");
	//MyBoatElements.push_back(bottomModel);
	//MyBoatShaders.push_back(bottomShader);
	//objModel hullModel("../obj/hull.obj", "../textures/boat/hull4.bmp");
	//Shader hullShader("../shader/BoatShader.vs", "../shader/BoatShader.fs");
	//MyBoatElements.push_back(hullModel);
	//MyBoatShaders.push_back(hullShader);
	//objModel mastModel("../obj/mast.obj", "../textures/boat/mast.bmp");
	//Shader mastShader("../shader/BoatShader.vs", "../shader/BoatShader.fs");
	//MyBoatElements.push_back(mastModel);
	//MyBoatShaders.push_back(mastShader);
	//objModel sailsModel("../obj/sails.obj", "../textures/boat/sails.bmp");
	//Shader sailsShader("../shader/BoatShader.vs", "../shader/BoatShader.fs");
	//MyBoatElements.push_back(sailsModel);
	//MyBoatShaders.push_back(sailsShader);
	//objModel wordsModel("../obj/words.obj", "../textures/boat/words.bmp");
	//Shader wordsShader("../shader/BoatShader.vs", "../shader/BoatShader.fs");
	//MyBoatElements.push_back(wordsModel);
	//MyBoatShaders.push_back(wordsShader);
	//调整船位置
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	MyBoat = new objModel("../obj/boat2.obj", "../textures/boat/boat1.bmp", 1);
	MyShader = new Shader("../shader/BoatShader.vs", "../shader/BoatShader.fs");
	Translate.y = MyBoat->getYMin().y * 1.5;//定下倾斜旋转轴；
	

}
glm::vec3 boat::Draw(glm::mat4 view, glm::mat4 projection,glm::vec3 CameraPos,glm::vec3 LightPos, glm::vec3 LightInfo, GLfloat LightIntensity, GLfloat gDeltaTime,GLfloat * points){
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
	MyBoat->draw(*MyShader, model, view, projection, CameraPos, LightPos, LightInfo, LightIntensity);

	//cout << "speed: " << speed.x << " " << speed.y << endl;
	//以下为自己绘制的船
	/*for (auto shader : MyBoatShaders) {
		shader.use();
		shader.setInt("boatTexture", 0);
	}
	for (int i = 0; i < MyBoatElements.size() ; i++) {
		MyBoatElements[i].draw(MyBoatShaders[i], model, view, projection);
	}*/
	//TODO 
	//cout << angle_side << endl;
	//if (angle_side >= 30.0f || angle_side <= -45.0f) {
	//	cout << "GAME OVER" << endl;
	//}
	temp0+=getCenter() - temp1;
	return getCenter()-temp1;
	//cout << model[1][3] << endl;
	//cout << "x:" << Front.x <<"y:"<<Front.y<< "z:" << Front.z << endl;
}
void boat::backToFront() {
	float k = 0.0f;
	if (angle_side != 0)
		k = ((abs(angle_side) / (angle_side)) * sqrt(abs(angle_side)) > 3.0) ? 3.0 : (abs(angle_side) / (angle_side)) * sqrt(abs(angle_side));
	else k = 0.0;
	angle_side -= k;
	glm::mat4 temp = glm::mat4(1.0f);
	temp = glm::translate(temp, Translate);
	temp = glm::rotate(temp, -(GLfloat)(glm::radians(k)), Front);
	temp = glm::translate(temp, -Translate);
	model = temp * model;
}
void boat::ProcessKeyboard(int Type, GLfloat gDeltaTime) {

	if (Type == GOFORWARD) {
		if (abs(speed.x + acclerate * gDeltaTime) <= MAX_SPEED)
			speed.x += acclerate * gDeltaTime;
		else speed.x = MAX_SPEED;
	}
	else if (Type == GOBACKWARD) {
		if (abs(speed.x - acclerate * gDeltaTime) <= MAX_SPEED)
			speed.x -= acclerate * gDeltaTime;
		else speed.x = -MAX_SPEED;
	}
	else if (Type == TURNLEFT) {
		if (abs(speed.y - acclerate * gDeltaTime) <= MAX_SPEED)
			speed.y -= acclerate * gDeltaTime;
		else speed.y = -MAX_SPEED;
		changeFrontVector(front_change_rate * gDeltaTime);
		modelSideRotaChanges(angleTurnLeft(gDeltaTime));
	}
	else if (Type == TURNRIGHT) {
		if (abs(speed.y + acclerate * gDeltaTime) <= MAX_SPEED)
			speed.y += acclerate * gDeltaTime;
		else speed.y = MAX_SPEED;
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

	//BUG

	//temp = glm::translate(temp, temp0);
	temp = glm::rotate(temp, glm::radians(offset), Steering_shaft);
	//temp = glm::translate(temp, -temp0);
	Front = glm::normalize(glm::vec3(temp * glm::vec4(Front, 1.0f)));
	//cout << "1 x:" << Front.x << "y:" << Front.y << "z:" << Front.z << endl;
	//cout << "x:" << Front.x << "y:" << Front.y << "z:" << Front.z << endl;
}
void boat::changeSpeedforWater(GLfloat gDeltaTime) {
	GLfloat gDeltaSpeedX = gDeltaTime * waterSpeedDown * (1 + speed.x / MAX_SPEED);
	GLfloat gDeltaSpeedY = gDeltaTime * waterSpeedDown * (1 + speed.y / MAX_SPEED);
	//cout << gDeltaSpeedY<<" "<<speed.y << endl;
	if (speed.y == 0 && speed.x != 0) {//向前速度
		int i = speed.x / abs(speed.x);
		if (abs(speed.x) >= abs(gDeltaSpeedX))
			speed.x += i * gDeltaSpeedX;
		else
			speed.x = 0;
	}
	else if (speed.x == 0 && speed.y != 0) {//向右速度
		int i = speed.y / abs(speed.y);
		if (abs(speed.y) >= abs(gDeltaSpeedY))
			speed.y += i * gDeltaSpeedY;
		else
			speed.y = 0;
	}
	else if (speed.x != 0 && speed.y != 0) {//前右都有
		double angle = atan(abs(speed.y / speed.x));//速度分解
		GLfloat gDeltaSpeedForword = gDeltaTime * cos(angle) * waterSpeedDown * (1 + speed.x / MAX_SPEED);
		GLfloat gDeltaSpeedRight = gDeltaTime * sin(angle) * waterSpeedDown * (1 + speed.y / MAX_SPEED);
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
	//const int N = 1024;//点的个数
	int x, z;
	x = point.x;
	z = point.z;

	//BUG not equal x and z
	//cout << " x: " << x << " z: " << z << endl;
	//cout << "ocean x:" << points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 0]
	//	<< "ocean y:" << points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 1]
	//	<< "ocean z:" << points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 2] << endl;
	return points[(x + 128) * 4 * 1025 * 4 + (z + 128) * 4 * 4 + 1];
}
static int l = 0;
void boat::buoyancy(GLfloat* points) {
	/*if (l == 0) {
		FILE* file = fopen("../End.obj", "w");
		if (file == NULL) {
			cout << "Failed to open the obj file!" << endl;
			exit(0);
		}
		fprintf(file, " %f %f %f %f ", points[0], points[1], points[2], points[3]);
		fclose(file);
		l++;
	}*/

	float y[5];
	y[0] = getOceanY(getFrontPoint(), points);
	y[1] = getOceanY(getBackPoint(), points);
	y[2] = getOceanY(getLeftPoint(), points);
	y[3] = getOceanY(getRightPoint(), points);
	y[4] = getOceanY(getCenter(), points);
	/*float k = -1000.0;
	for (int i = 0; i < 5; i++) {
		if (y[i] >= k)k = y[i];
	}*/
	float k = (y[0] + y[1] + y[2] + y[3] + y[4]) / 5;
	glm::mat4 temp1 = glm::mat4(1.0f);
	temp1 = glm::translate(temp1, glm::vec3(0.0f, 0.5f+k-getCenter().y, 0.0f));
	model = temp1 * model;

	float temp2 = y[2] - y[3];//0.2左右
	buoyancyForSide(temp2*30);
}
void boat::buoyancyForSide(GLfloat angleTurn) {
	int i = 0;
	while (abs(angle_side + angleTurn) > MAX_ANGLE) {
		i++;
		angleTurn /= 2;
		if (i == 10) {
			angleTurn = 0;
			break;
		}
	}
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
void boat::OutObj() {
	MyBoat->objModelOut();
}