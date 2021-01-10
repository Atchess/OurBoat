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
	glm::vec3 Steering_shaft;			//����ת��
	void changeSpeedforWater(GLfloat gDeltaTime);
	void modelTranChanges(GLfloat gDeltaTime);
	void modelSideRotaChanges(float angle);
	void changeFrontVector(float offset);
	float angleTurnRight(GLfloat gDeltaTime);
	float angleTurnLeft(GLfloat gDeltaTime);
public:
	boat();
	void init();
	void Draw(glm::mat4 view, glm::mat4 projection, GLfloat gDeltaTime);
	void ProcessKeyboard(int Type,GLfloat gDeltaTime);
	glm::vec3 getCenter();
	glm::vec3 getFront();
};
enum boatMovement { GOFORWARD, GOBACKWARD, TURNLEFT, TURNRIGHT };