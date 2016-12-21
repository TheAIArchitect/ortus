//
//  Playground.hpp
//  LearningOpenGL
//
//  Created by onyx on 1/20/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#ifndef Playground_hpp
#define Playground_hpp

#include <stdio.h>
#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <vector>
#include "DataSteward.hpp"
#include <glm/gtx/rotate_vector.hpp>

class Playground{
public:
    Playground(DataSteward* fakeStewie);
    void step();
    void initNeurons();
    void dropNeurons();
    
    void initMuscles();
    void moveMuscles();
    
    void initCenterCylinder();
    void moveCenterCylinder();
    
    void connectMusclesToCylinder();
    
public:
    float PHYSICS_SCALE = 100.0f;
    DataSteward* fakeStewie;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    
    
    std::vector<btRigidBody*> muscles;
    std::vector<btGeneric6DofSpring2Constraint*> muscle_cons;
    
    std::vector<btRigidBody*> centerCylinder;
    std::vector<btGeneric6DofSpring2Constraint*> centerCylinder_cons;
    
    // neurons
    std::vector<btRigidBody*> neuron_bodies;
    std::vector<btPoint2PointConstraint> p2p_neuron_cons;
    
    
    std::vector<btGeneric6DofSpring2Constraint*> muscle_to_cc_cons;
};


#endif /* Playground_hpp */
