//
//  myDebug.cpp
//  PhunWithPhysics
//
//  Created by onyx on 3/15/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "myDebug.hpp"

void myDebug::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
    glBegin(GL_LINES);
    glColor3f(color.getX(), color.getY(), color.getZ());
    glVertex3d(from.getX(), from.getY(), from.getZ());
    glVertex3d(to.getX(), to.getY(), to.getZ());
    glFlush();
    glEnd();
}

void myDebug::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
    
}

void myDebug::reportErrorWarning(const char* warningString)
{
    
}

void myDebug::draw3dText(const btVector3& location,const char* textString)
{
    
}

void myDebug::setDebugMode(int debugMode)
{
    
}

int myDebug::getDebugMode() const{
    int r = 1;
    return r;
}
