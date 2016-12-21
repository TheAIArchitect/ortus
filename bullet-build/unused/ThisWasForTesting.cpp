//
//  TheRealDeal.cpp
//  PhunWithPhysics
//
//  Created by onyx on 3/15/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "TheRealDeal.hpp"


#include <iostream>

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <GLUT/glut.h>

btRigidBody* fallRigidBody;
btDiscreteDynamicsWorld* _dynamicsWorld;

void ddisplay(){
    
   
    for(int i = 0; i < 300; i++){
        _dynamicsWorld->stepSimulation(1 / 60.f,0);
        //Clear all pixels
        glClear(GL_COLOR_BUFFER_BIT);
 
        
        //draw white polygon (rectangle) with corners at
        // (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
        glColor3f(1.0,1.0,1.0);
        
        
        
        btTransform trans;
        fallRigidBody->getMotionState()->getWorldTransform(trans);
        
        std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
        
        _dynamicsWorld->debugDrawWorld();
        //int x;
        //std::cin >> x;
        glBegin(GL_LINES);
        
        glColor3f(1, 0, 0);
        glVertex3d(0, 0, 0);
        glVertex3d(10, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 10, 0);
        glColor3f(0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, 10);
        glEnd();
        glFlush();
    }
    return;
}



int notmain (int argc, char** argv)
{
    
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    _dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    
    _dynamicsWorld->setGravity(btVector3(0, -10, 0));
   
    //glinit(argc, argv);
   
    myDebug* debugDrawer = new myDebug();
    _dynamicsWorld->setDebugDrawer(debugDrawer);
    
    
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    
     btCollisionShape* fallShape= new btSphereShape(1);
    
    
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo
    groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    _dynamicsWorld->addRigidBody(groundRigidBody);
    
    
    btDefaultMotionState* fallMotionState =
    new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    fallRigidBody = new btRigidBody(fallRigidBodyCI);
    _dynamicsWorld->addRigidBody(fallRigidBody);
    

    glutMainLoop();
    
    _dynamicsWorld->removeRigidBody(fallRigidBody);
    delete fallRigidBody->getMotionState();
    delete fallRigidBody;
    
    _dynamicsWorld->removeRigidBody(groundRigidBody);
    delete groundRigidBody->getMotionState();
    delete groundRigidBody;
    
    
    delete fallShape;
    
    delete groundShape;
    
    
    delete _dynamicsWorld;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
    
    return 0;
}