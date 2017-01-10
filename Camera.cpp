

#include "Camera.hpp"

/* Thanks to:
 http://computergraphics.stackexchange.com/questions/151/how-to-implement-a-trackball-in-opengl
 * -- RichieSams --
 */

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

int Camera::voltageFrame = 0;
// Initial Field of View
float Camera::FoV = 45.0f;
//float Camera::FoV = 3.0f;
//float Camera::FoV = 0.008412f;
//float Camera::FoV = 0.01f;


GLuint Camera::vp_width = 800;
GLuint Camera::vp_height = 600;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

bool Camera::mouse_clicked = false;
bool Camera::mouse_was_clicked = false;
float Camera::mouse_x_prev = 0.0f;
float Camera::mouse_y_prev = 0.0f;
float Camera::mouse_x_cur = 0.0f;
float Camera::mouse_y_cur = 0.0f;
float Camera::scroll_yoffset = 0.0f;
float Camera::scroll_xoffset = 0.0f;
bool Camera::shift_pressed = false;
bool Camera::ctl_pressed = false;
bool Camera::space_pressed = false;


float Camera::MIN_RAD = .00001; // just a really small number.
float Camera::MAX_RAD = 45.f;

//float stof(std::string s){
//    return ((float)atof(s.c_str()));
//}

Camera::Camera(GLFWwindow* window): window (window){
    
    std::vector<std::string> configVec = FileAssistant::readCameraConfig();
    if (configVec.size() == 4){// config found
        std::vector<std::string> target = StrUtils::parseOnCharDelim(configVec[0],',');
        m_target = glm::vec3(stof(target[0]), stof(target[1]), stof(target[2]));
        m_theta = stof(configVec[1]);
        m_phi = stof(configVec[2]);
        m_radius = stof(configVec[3]);
        
    }
    else { // no config, use defaults
        // these numbers were 'exprimentally' chosen -- a decent view was selected, and the parameters were printed out.
        //m_phi = 1.4f;
        //m_theta = 2.8f;
        //m_radius = 3.5f;
        //m_target = glm::vec3(0,0,0);
        
        m_phi = 1.30489f;
        m_theta = 23.88578f;
        m_radius = 7.15772f;
        m_target = glm::vec3(0.06556, -0.0746, 0.99980);
        
        m_phi = 1.29208f;
        m_theta = 22.47337;
        m_radius = 7.37368;
        m_target = glm::vec3(1.03372, -0.50640, 3.86143);
    }
}


glm::mat4 Camera::getViewMatrix(){
    return ViewMatrix;
}

glm::mat4 Camera::getProjectionMatrix(){
    return ProjectionMatrix;
}

glm::vec3 Camera::toCartesian(){
    float x = m_radius * sinf(m_phi) * sinf(m_theta);
    float y = m_radius * cosf(m_phi);
    float z = m_radius * sinf(m_phi) * cosf(m_theta);
    return glm::vec3(x,y,z);
}

void Camera::rotate(float dTheta, float dPhi){
    m_theta += dTheta;
    m_phi += dPhi;
    if (m_phi <= 0){
        m_phi = .00001; // just something really small
    }
    else if (m_phi > M_PI){
        m_phi = M_PI;
    }
    
}

void Camera::zoom(float dist){
    //printf("WTF?\n");
    m_radius -= dist;
}


void Camera::pan(float dx, float dy){
    glm::vec3 lookDirection = glm::normalize(toCartesian()); // pretty sure this is look direction... not the lookAt, which is m_target.
    glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
    right = glm::cross(lookDirection, worldUp);
    cameraUp = glm::cross(lookDirection, right);
    m_target = m_target + (right * dx) + (cameraUp * dy);
    //printf("target: (%.2f, %.2f, %.2f)\n",m_target.x, m_target.y, m_target.z);
}


glm::vec3 Camera::getEye(){
    return m_target + toCartesian();
}

glm::vec3 Camera::getTarget(){
    return m_target;
}

glm::vec3 Camera::getRight(){
    return right;
}

void Camera::updateStatus(){
    
    if (Camera::space_pressed){
        std::vector<std::string> configVec;
        char buf[32];
        sprintf(buf,"%.5f,%.5f,%.5f",m_target.x, m_target.y, m_target.z);
        std::string temp(buf);
        configVec.push_back(buf);
        sprintf(buf,"%.5f",m_theta);
        temp = std::string(buf);
        configVec.push_back(buf);
        sprintf(buf,"%.5f",m_phi);
        temp = std::string(buf);
        configVec.push_back(buf);
        sprintf(buf,"%.5f",m_radius);
        temp = std::string(buf);
        configVec.push_back(buf);
        FileAssistant::writeCameraConfig(configVec);
        printf("CAMERA == target: (%.5f, %.5f, %.5f), theta: %.5f, phi: %.5f, radius: %.5f\n",m_target.x, m_target.y, m_target.z, m_theta, m_phi, m_radius);
        Camera::space_pressed = false;
        
    }
    int height, width;
    glfwGetWindowSize(window, &width, &height);
    vp_width = width;
    vp_height = height;
    
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    //deltaTime = float(currentTime - lastTime)*FoV/100.0f;
    deltaTime = float(currentTime - lastTime)*m_radius/100.0f;
    
    
    if (Camera::shift_pressed && !Camera::ctl_pressed){
        float dy = (scroll_yoffset * 10* deltaTime);
        float dx = (scroll_xoffset * 10*deltaTime);
        pan(dx, dy);
    }
    if (Camera::ctl_pressed && !Camera::shift_pressed) {
        zoom((scroll_yoffset*deltaTime));
    }
   else if (!Camera::shift_pressed && !Camera::ctl_pressed){
        float dPhi = (scroll_yoffset * 10* deltaTime);
        float dTheta = (scroll_xoffset * 10*deltaTime);
        rotate(dTheta, dPhi);
    }
    
    
    
    if (FoV < 0.00000001){
        FoV = 0.00000001;
    }
    else if (FoV >= 180){
        FoV = 179.9999f;
    }
    if (m_radius <= MIN_RAD ){
        m_radius = MIN_RAD;
    }
    else if (m_radius >= MAX_RAD){
        m_radius = MAX_RAD;
    }
    // reset scroll
    scroll_yoffset = 0.0f;
    scroll_xoffset = 0.0f;
    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), ((float)Camera::vp_width / Camera::vp_height), Camera::Z_NEAR, Camera::Z_FAR);
    // Camera matrix
    ViewMatrix       = glm::lookAt(
                                   m_target + toCartesian(),           // Camera is here
                                   m_target, // and looks here : at the same position, plus "direction"
                                   this->up                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}




void Camera::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            printf("oOoOO you clicked it!\n");
            mouse_clicked = true;
            mouse_was_clicked = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            mouse_clicked = false;
            mouse_was_clicked = false;
        }
    }

void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scroll_xoffset = xoffset;
    scroll_yoffset = yoffset;
}

/*
glm::vec3 Camera::get_arcball_vector(int x, int y){
    glm::vec3 P = glm::vec3((2.0f*x - vp_width)/vp_width, (2.0f*y - vp_height)/vp_height,0);
        float OP_squared = P.x*P.x + P.y*P.y;
        P.y = -P.y;
        if (OP_squared <= 1.0f){
            P.z = sqrt(1.0f - OP_squared);
        }
        else{
            P = glm::normalize(P);
        }
        return P;
    }
 */

void Camera::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_x_prev = mouse_x_cur;
    mouse_y_prev = mouse_y_cur;
    mouse_x_cur = xpos;
    mouse_y_cur = ypos;
    /*
    if( mouse_clicked && mouse_was_clicked == false){
        mouse_was_clicked = true;
        mouse_x_prev = xpos;
        mouse_y_prev = ypos;
        mouse_x_cur = xpos;
        mouse_y_cur = ypos;
    }
    else if (mouse_clicked && mouse_was_clicked){
        mouse_x_prev = mouse_x_cur;
        mouse_y_prev = mouse_y_cur;
        mouse_x_cur = xpos;
        mouse_y_cur = ypos;
    }
     */
}


