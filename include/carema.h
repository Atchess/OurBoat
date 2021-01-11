#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW=0,PITCH=0,SPEED=5,SENSITIVITY=0.1,ZOOM=45;

class Camera{
public:
    glm::vec3 Position,Front,Right,WorldUp;
    glm::vec3 distance = glm::vec3(0.0f);
    float tolerableDis = 5.0f; 
    float Yaw,Pitch,MovementSpeed,MouseSensitivity,Zoom;
    
    Camera(glm::vec3 position=glm::vec3(0.0f,3.0f,3.0f),glm::vec3 up=glm::vec3(0.0f,1.0f,0.0f),
           float yaw=YAW,float pitch=PITCH):Front(glm::vec3(0.0f,0.0f,1.0f)),MouseSensitivity(SENSITIVITY),MovementSpeed(SPEED),Zoom(ZOOM){
        Position=position;
        WorldUp=up;
        Yaw=yaw;
        Pitch=pitch;
        updateCameraVectors();
    }
    Camera(float posX,float posY,float posZ,float upX,float upY,float upZ,float yaw,float pitch):
    Front(glm::vec3(0.0f,0.0f,1.0f)),MovementSpeed(SPEED),MouseSensitivity(SENSITIVITY),Zoom(ZOOM){
        Position=glm::vec3(posX,posY,posZ);
        WorldUp=glm::vec3(upX,upY,upZ);
        Yaw=yaw;
        Pitch=pitch;
        updateCameraVectors();
    }
    glm::mat4 GetViewMatrix(){
        return glm::lookAt(Position, Position-Front, WorldUp);
    }
    void processKeyboard(Camera_Movement direction,float deltaTime){
        float velocity=MovementSpeed*deltaTime;
        if(direction==FORWARD)
            Position+=-Front*velocity;
        if(direction==BACKWARD)
            Position-=-Front*velocity;
        if(direction==LEFT)
            Position-=Right*velocity;
        if(direction==RIGHT)
            Position+=Right*velocity;
    }
    void ProcessMouseMovement(float xOffset,float yOffset){
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;
        
        Yaw += xOffset;
        Pitch -= yOffset;
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
        updateCameraVectors();
    }
    
    //变焦处理
    void ProcessMouseScroll(float yoffset) {
        if (Zoom >= 5.0f && Zoom <= 70.0f)
            Zoom -= yoffset;
        if (Zoom <=5.0f)
            Zoom = 5.0f;
        if (Zoom >= 70.0f)
            Zoom = 70.0f;
    }

    void goWithBoat (glm::vec3 offset, glm::vec3 BoatFront, glm::vec3 BoatRight) {
        distance += offset;
        if (abs(offset.x) > 0) {
            Position += glm::normalize(BoatFront) * offset.x; //((distance.x / abs(distance.x)) * (abs(distance.x) - tolerableDis));
            distance.x -= (distance.x / abs(distance.x)) * (abs(distance.x) - tolerableDis);
        }
        if (abs(distance.z) > 0) {
            Position += glm::normalize(glm::cross(BoatFront, glm::vec3(0, 1, 0))) * offset.z;//BoatRight * ((distance.z / abs(distance.z)) * (abs(distance.z) - tolerableDis));
            distance.z -= (distance.z / abs(distance.z)) * (abs(distance.z) - tolerableDis);
        }
    }
    
private:
    //更新摄像机向量
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = -cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(-front);
        Right = glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),Front));
        WorldUp = glm::normalize(glm::cross(Front,Right));
    }
};
#endif /* Camera_hpp */
