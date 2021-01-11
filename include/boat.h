#pragma once
#pragma once
#include <glad/glad.h>
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
#define MAX_ANGLE 40
class boat{
private:
	/*std::vector<objModel> MyBoatElements;
	std::vector<Shader> MyBoatShaders;*/

	//�������˶��У�ֻ��Ҫ�ı�model���ı�λ�ã��ı�Front���ı��˶�����
	glm::mat4 model = glm::mat4(1.0f);	//���þ���
	objModel* MyBoat;
	Shader* MyShader;
	glm::vec2 speed;					//xΪFront��ǰ���ٶȣ�yΪFront���ҵ��ٶ�
	float waterSpeedDown;				//ˮ�м��ٶ�
	float acclerate;					//���ٶ�
	float tilt_rate;					//������ת����
	float tilt_rate_back;				//����ת������
	float front_change_rate;			//�����仯����
	float angle_side;					//������б�Ƕ� >0�� <0��
	glm::vec3 Translate;				//����ת��ƫ��
	glm::vec3 Front;					//����������� Ҳ�ǲ����ת��
	glm::vec3 Right;					//�ҷ���
	glm::vec3 Steering_shaft;			//����ת��
	glm::vec3 temp0;
	void changeSpeedforWater(GLfloat gDeltaTime);		//ˮ���Դ��ļ���
	void modelTranChanges(GLfloat gDeltaTime);			//����model����ı仯
	void modelSideRotaChanges(float angle);				//������б
	void changeFrontVector(float offset);				//�ı�Front
	float angleTurnRight(GLfloat gDeltaTime);			//����ת
	float angleTurnLeft(GLfloat gDeltaTime);			//����ת
	void buoyancy(GLfloat* points);						//ˮ���Դ������ƶ���Ӱ��
	void buoyancyForSide(GLfloat angleTurn);			//ˮ���Դ�����ҡ�ε�Ӱ��
	void backToFront();									//������б��ʱ��ص�����
public:
	boat();
	void init();
	glm::vec3 Draw(glm::mat4 view, glm::mat4 projection, GLfloat gDeltaTime, GLfloat* points);
	void ProcessKeyboard(int Type,GLfloat gDeltaTime);
	glm::vec3 getCenter();
	glm::vec3 getFront();
	glm::vec3 getRight();
	glm::vec3 getFrontPoint();
	glm::vec3 getBackPoint();
	glm::vec3 getUpPoint();
	glm::vec3 getDownPoint();
	glm::vec3 getLeftPoint();
	glm::vec3 getRightPoint();
	glm::vec2 getSpeed();
};
enum boatMovement { GOFORWARD, GOBACKWARD, TURNLEFT, TURNRIGHT };