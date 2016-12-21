//
//  myDebug.hpp
//  PhunWithPhysics
//
//  Created by onyx on 3/15/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#pragma once

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>

#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif

class myDebug : public btIDebugDraw{
public:
    void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
    void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    void reportErrorWarning(const char* warningString);


    void draw3dText(const btVector3& location,const char* textString);
    void setDebugMode(int debugMode);
    int getDebugMode() const;
};