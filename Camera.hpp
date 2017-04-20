#ifndef CONTROLS_HPP
#define CONTROLS_HPP


#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <vector>
#include <string>
#include "FileAssistant.hpp"

class Camera{
    
public:
    
    static int voltageFrame;
    Camera(GLFWwindow* window);
    void updateStatus();
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    enum CameraDir{
        //FORWARD,
        //BACKWARD,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NEW_ROTATION_START
    };
    void processKeyboard(CameraDir direction);
    
    float m_phi;
    float m_theta;
    float m_radius;
    glm::vec3 m_target;
    
    static float MIN_RAD; // just a really small number.
    static float MAX_RAD;
    
    
public:
    
    static float mouse_x_prev;
    static float mouse_y_prev;
    static float mouse_x_cur;
    static float mouse_y_cur;
    static bool mouse_clicked;
    static bool mouse_was_clicked;
    static bool shift_pressed;
    static bool ctl_pressed;
    static bool space_pressed;
    float deltaTime;
    GLFWwindow* window;
    // Camera Attributes
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(0,0,0);
    glm::vec3 cameraUp = glm::vec3(0,0,0);
    // Camera options
    static float scroll_yoffset;
    static float scroll_xoffset;
    
    static float FoV;
    static GLuint vp_width;
    static GLuint vp_height;
    static constexpr float Z_NEAR = 0.00001f;
    static constexpr float Z_FAR = 100.0f;
    
    
    glm::vec3 toCartesian();
    void rotate(float dTheta, float dPhi);
    void zoom(float dist);
    void pan(float dx, float dy);
    
    
    glm::vec3 getEye();
    glm::vec3 getTarget();
    glm::vec3 getRight();
    

};
#endif
