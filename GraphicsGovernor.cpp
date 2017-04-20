#include "GraphicsGovernor.hpp"

// Properties

//glm::mat4 view;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void float_to_color(float color_index, float* color);
void Do_Movement(Camera camera);

// Camera
//Camera camera(glm::vec3(0.18f, -0.13f, -0.24f));
//Camera camera;//(glm::vec3(0.0f, 0.0f, 1.0f));
glm::vec3 lightPos(10.0f, 10.0f, 10.0f);// lights to eyepoint
bool keys[1024];
//GLfloat lastX = 400, lastY = 300;
//bool firstMouse = true;



GLfloat deltaTime = 0.0f;
//GLfloat lastFrame = 0.0f;

//bool first_go = true;

/******************************* TEMP *****************************/





// The MAIN function, from here we start our application and run our Game loop
int initGraphics(DataSteward* stewie)
{
    
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(Camera::vp_width, Camera::vp_height, "ortus", NULL, NULL); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    // all callbacks, other than key, for now, defined in camera.h
    glfwSetKeyCallback(window, key_callback); // physical key press
    glfwSetCursorPosCallback(window, Camera::mouse_callback);
    glfwSetScrollCallback(window, Camera::scroll_callback);
    glfwSetMouseButtonCallback(window, Camera::mouse_button_callback);

    // Options
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    //glewExperimental = GL_TRUE;
    //glewInit();

    Camera camera(window);
    
    // Define the viewport dimensions
    glViewport(0, 0, Camera::vp_width, Camera::vp_height);

    /* Setup some OpenGL options */
    // glEnable is a multi-purpose tool; many binary on/off flags turned on by this command
    // here, we enable 'face culling' -- draw the sides of an object that you can see,
    // and don't draw the ones you can't see.
    glEnable(GL_CULL_FACE);
    // tell OpenGL which face (front or back) to cull,
    // and whether the front face has a CW or CCW 'winding' (ordering of vertices)
    glCullFace(GL_BACK);
    //glFrontFace(GL_CW); // CCW for the way i did the spheres....
    
    
    // enable depth testing, and writing to the depth buffer
    glEnable(GL_DEPTH_TEST);
    // causes the depth value of a fragment to be written to the depth buffer
    glDepthMask(GL_TRUE);
    // sets the depth test function to '<=' -- so, overwrite value in depth buffer
    // if new value's depth is less than or the same as the existing one's
    glDepthFunc(GL_LEQUAL);
    // defines linear mapping from NDC (normalized device coordinate) space to window space
    glDepthRange(0.0f, 1.0f);
    
    /* // BEGIN ORTUS COMMENT

    // Setup and compile our shaders
    Shader ourShader("coordinate_systems.vs", "coordinate_systems.frag");
    Shader myShader("my_vertex_shader.vert", "my_frag_shader.frag");
    Shader axesShader("axes_vert_shader.vert", "axes_frag_shader.frag");
    Shader textShader("text_vert_shader.vert", "text_frag_shader.frag");
    
    
    //glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, -5.0f)};
    glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, -2.0f)};

    GLuint VBO, VAO;
    
    
    
    
    
 
    Artist artist(&core->stewie);
    artist.voltages = &core->stewie.kernelVoltages; // core should almost certainly hold the kernelVoltages
    
    artist.prep_axes();
    
 
    artist.prep_axes();
    artist.prep_neurons();
    artist.prep_sine();
    
    artist.prep_muscles();
    //artist.prep_shroom();
    artist.prep_petriDish();
    artist.prep_bez();
    //artist.prep_center_cylinder();
    bool specific = false;
    
    //const int startingSetSize = 1;
    //std::string startingSet[startingSetSize] = {"ASEL"};
    
//    const int startingSetSize = 16;
//    std::string startingSet[startingSetSize] = {"AVM", "ALML", "ALMR", "AVDL", "AVDR", "AVAL", "AVAL", "DVA", "PLML", "PLMR", "AVBL", "AVBR", "PVDL", "PVDR", "PVCL", "PVCR"};
    
    //const int startingSetSize = 1;
    //std::string startingSet[startingSetSize] = {"ALML"};
    
    //const int startingSetSize = 12;
    //std::string startingSet[startingSetSize] = {"ALML", "SIADR","AVDR","AVDL","AVBR", "AVBL", "RICR", "RICL", "AVER", "AVEL","dBWML5", "dBWML4"};
    
    
    const int startingSetSize = 2;
    std::string startingSet[startingSetSize] = {"ASEL","ASER"};
    
    ConnectionComrade comrade = ConnectionComrade(core->stewie.elements, startingSet, startingSetSize);
    if (OptionForewoman::WormOpts[SHOW_MUSCLES]){
        comrade.generateConns(ConnectionComrade::MUSCLES | ConnectionComrade::STARTING_SET);
    }
    else{
        comrade.generateConns(ConnectionComrade::STARTING_SET);
    }
    artist.prep_conns(comrade.num_all_indices, comrade.num_chem_indices, comrade.num_gap_indices, comrade.all_conns);
    
 
   // INITIALIZE BULLET PHYSICS
    Playground playground(&core->stewie);
    playground.initNeurons();
    */// END ORTUS COMMENT
    //playground.initMuscles();
    //playground.initCenterCylinder();
    //playground.connectMusclesToCylinder();
   // END BULLET INIT
 
    /*
    int  count_conn = 0;
    for (int i = 0; i < total_chems; i+=6){
        printf("CHEM CONN (%d): %f, %f, %f -> %f, %f, %f\n",count_conn,chem_conns[i],chem_conns[i+1],chem_conns[i+2],chem_conns[i+3],chem_conns[i+4],chem_conns[i+5]);
        count_conn++;
    }
     */
    //exit(0);

    
    ///////////////////////// TEXT
    
    //glm::mat4 ortho_projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    
 
    ///////////////// END TEXT
    
    /*
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    // Load and create a texture 
    GLuint texture1;
    GLuint texture2;
    // --== TEXTURE 1 == --
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("resources/textures/container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    // --== TEXTURE 2 == --
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); 
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    image = SOIL_load_image("resources/textures/awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    */
    
    
    
    
    
    
    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        camera.updateStatus();
        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
        // Draw our first triangle
        ourShader.Use();

        // Bind Textures using texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);
        
        // Create camera transformation
        //if(first_go){
        glm::mat4 view = camera.GetViewMatrix();
        //    first_go = false;
        //}
        glm::mat4 projection;
        projection = glm::perspective(45.0f, (float)Camera::vp_width/(float)Camera::vp_height, Camera::Z_NEAR, Camera::Z_FAR); // Z_NEAR and Z_FAR defined in camera.h
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        for(GLuint i = 0; i < 1; i++)
        {
            // Calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            //GLfloat angle = 20.0f * i;
            //model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
     */
        if (Probe::newElementToProbeRequested){
            std::string res = stewie->probe->setNewElementToProbe();
            printf("%s\n", res.c_str());
        }
        
        // this should probably be held within Probe...
        if (Probe::update){// update isn't set to true unless Probe::probeEnabled == true... if that changes, we need to ensure it's enabled here.
            if (Probe::probeAll){
                stewie->probe->printAll(Camera::voltageFrame);
            }
            else {
                stewie->probe->printCurrentProbe(Camera::voltageFrame);
            }
            Probe::update = false;
        }
        /* // BEGIN ORTUS COMMENT
        if (OptionForewoman::REFRESH_CONNS){
            comrade.resetConns();
            if (OptionForewoman::WormOpts[SHOW_MUSCLES]){
                comrade.generateConns(ConnectionComrade::MUSCLES | ConnectionComrade::STARTING_SET);
            }
            else{
                comrade.generateConns(ConnectionComrade::STARTING_SET);
            }
            artist.prep_conns(comrade.num_all_indices, comrade.num_chem_indices, comrade.num_gap_indices, comrade.all_conns);
            printf("TADA!\n");
            OptionForewoman::REFRESH_CONNS = false;
        }
        
        
        glm::mat4 view = camera.getViewMatrix();
        
        glm::mat4 projection;
        projection = camera.getProjectionMatrix();
        glm::mat4 model(1.0f);
        
        axesShader.Use();
        artist.draw_axes(axesShader, &projection, &view);
        
        myShader.Use();
        artist.draw_neurons(myShader, &projection, &view, &model, camera);
        if (OptionForewoman::WormOpts[SHOW_MUSCLES]){
            artist.draw_muscles(myShader, &projection, &view, &model, camera);
        }
        //myShader.Use();
        //artist.draw_bez(myShader, &projection, &view, camera, bezPts, numBezPts); NOTE: bezPts and numBezPts in DataSteward!!!
        
        //myShader.Use();
        //artist.draw_center_cylinder(myShader, &projection, &view, camera);
        //axesShader.Use();
         //artist.draw_sine(axesShader, &projection, &view);
        view = camera.getViewMatrix();
        projection = camera.getProjectionMatrix();
        //projection = glm::perspective(45.0f, (float)Camera::vp_width/(float)Camera::vp_height, Camera::Z_NEAR, Camera::Z_FAR); // Z_NEAR and Z_FAR defined in camera.h
        // NOTE: uncomment pretty much everything here....

        textShader.Use();
        artist.draw_neuron_names(textShader, camera, &projection, &view);
       
        
        axesShader.Use();
        
        view = camera.getViewMatrix();
        projection = camera.getProjectionMatrix();
        
        artist.draw_conns(axesShader, camera, &projection, &view);
        
        if (OptionForewoman::WormOpts[SHOW_WEIGHTS]){
            textShader.Use();
            artist.draw_conn_weights(textShader, &comrade, camera, &projection, &view);
        }
        
       
        
//        curr_prog = -1;
//        glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
//        printf("PROGRAM: %d\n",((int)curr_prog));

        
        
       // artist.draw_shroom(myShader, &projection, &view, camera);
       //artist.draw_petriDish(myShader, &projection, &view, camera);
        
        
        
        
        
        
        
        // Swap the buffers
        glfwSwapBuffers(window);
        // BULLET STEP
        playground.step();
        */// END ORTUS COMMENT
        // Swap the buffers
        glfwSwapBuffers(window);
        
    }
    // Properly de-allocate all resources once they've outlived their purpose
    // ORTUS COMMENT glDeleteVertexArrays(1, &VAO);
    // ORTUS COMMENT glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
/*
void Do_Movement(Camera camera)
{
    //deltaTime = .0005;
    deltaTime = .005;
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.processKeyboard(Camera::CameraDir::UP);
    if(keys[GLFW_KEY_S])
        camera.processKeyboard(Camera::CameraDir::DOWN);
    if(keys[GLFW_KEY_A])
        camera.processKeyboard(Camera::CameraDir::LEFT);
    if(keys[GLFW_KEY_D])
        camera.processKeyboard(Camera::CameraDir::RIGHT);
    
}
*/
int lastKey = -1;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (mods == GLFW_MOD_SHIFT){
        Camera::shift_pressed = true;
    }
    else{
        Camera::shift_pressed = false;
    }
    if (mods == GLFW_MOD_CONTROL){
         Camera::ctl_pressed = true;
    }
    else{
        Camera::ctl_pressed = false;
    }
    if (action == GLFW_PRESS){
        keys[key] = true;
        if(key == GLFW_KEY_SPACE && !Camera::space_pressed){
            Camera::space_pressed = true;
        }
        
        // Worm options
        else if (OptionForewoman::WAITING_ON_OPTION_REQUEST){
            if (key == GLFW_KEY_DELETE){
                OptionForewoman::WAITING_ON_OPTION_REQUEST = false;
            }
            else if (key == GLFW_KEY_ENTER){
                std::string res = "";
                if (OptionForewoman::OPTION_REQUESTED == GLFW_KEY_P){ // we do something different... this should be fixed...
                    Probe::newElementToProbe = OptionForewoman::CHOICE;
                    Probe::newElementToProbeRequested = true;
                }
                else {
                    res = OptionForewoman::keyboardOptionGauntlet(OptionForewoman::OPTION_REQUESTED,OptionForewoman::CHOICE);
                }
                // reset variables
                OptionForewoman::WAITING_ON_OPTION_REQUEST = false;
                OptionForewoman::OPTION_REQUESTED = -1;
                OptionForewoman::CHOICE = "";
                printf("%s\n",res.c_str());
            }
            else{
                OptionForewoman::CHOICE += static_cast<char>(key);
            }
        }
        else if ((key == GLFW_KEY_1 || key == GLFW_KEY_2 )){ // GLFW_KEY_1 maps to 49, 2 maps to 50, and 0 maps to 48... it's ASCII.
            OptionForewoman::WAITING_ON_OPTION_REQUEST = true;
            if (OptionForewoman::WAITING_ON_OPTION_REQUEST){
                int actualOption = key - GLFW_KEY_1; // we start our enum at 0, so, 1 should map to 0.
                printf("Now waiting for input... remembering key '%d'\n",actualOption);
                OptionForewoman::OPTION_REQUESTED = actualOption;
            }
        }
        else if (key == GLFW_KEY_P){
            OptionForewoman::WAITING_ON_OPTION_REQUEST = true;
            if (OptionForewoman::WAITING_ON_OPTION_REQUEST){
                OptionForewoman::OPTION_REQUESTED = GLFW_KEY_P;
                printf("Now waiting for input... remembering key '%c'\n",'p');
            }
        }
        else if (key == GLFW_KEY_3){
            std::string res = OptionForewoman::keyboardOptionGauntlet(SHOW_ALL_NEURONS,std::to_string(!OptionForewoman::WormOpts[SHOW_ALL_NEURONS]));
            printf("%s\n",res.c_str());
        }
        else if (key == GLFW_KEY_4){
            std::string res = OptionForewoman::keyboardOptionGauntlet(SHOW_MUSCLES, std::to_string(!OptionForewoman::WormOpts[SHOW_MUSCLES]));
            printf("%s\n",res.c_str());
        }
        else if (key == GLFW_KEY_5){
            std::string res = OptionForewoman::keyboardOptionGauntlet(SHOW_ACTIVATION,std::to_string(!OptionForewoman::WormOpts[SHOW_ACTIVATION]));
            printf("%s\n",res.c_str());
        }
        /*
        else if (key == GLFW_KEY_G){
            ConnectionComrade::doGaps = !ConnectionComrade::doGaps;
            OptionForewoman::REFRESH_CONNS = true;
        }
        else if (key == GLFW_KEY_C){
            ConnectionComrade::doChems = !ConnectionComrade::doChems;
            OptionForewoman::REFRESH_CONNS = true;
        }
        else if (key == GLFW_KEY_W){
            ConnectionComrade::doChems = !ConnectionComrade::doChems;
            std::string res = OptionForewoman::keyboardOptionGauntlet(SHOW_WEIGHTS,std::to_string(!OptionForewoman::WormOpts[SHOW_WEIGHTS]));
            printf("%s\n",res.c_str());
        }
         */
    }
    else if(action == GLFW_RELEASE){
            keys[key] = false;
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || mods == GLFW_MOD_SUPER)){
        Camera::voltageFrame--;
        if (Camera::voltageFrame < 0){
            Camera::voltageFrame = 0;
        }
        if(Probe::probeEnabled) {
            Probe::update = true;
        }
    }
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || mods == GLFW_MOD_SUPER)){
        Camera::voltageFrame++;
        if (Camera::voltageFrame >= 1000){
            Camera::voltageFrame = 999;
        }
        if(Probe::probeEnabled) {
            Probe::update = true;
        }
    }
    
}


float test1[] = {0.0f, 0.0f, 0.0f, 1.0f};



