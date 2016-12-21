//
//  Playground.cpp
//  LearningOpenGL
//
//  Created by onyx on 1/20/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#include "Playground.hpp"


Playground::Playground(DataSteward* fakeStewie): fakeStewie {fakeStewie}{
    
    /* BULLET INIT */
    
    broadphase = new btDbvtBroadphase();
    
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    
   // dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    dynamicsWorld->setGravity(btVector3(0, 0 , 9.81));
    
    
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    
    
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    //groundRigidBody->setRestitution(.95);// rubber
    groundRigidBody->setRestitution(.5);// rubber
    //groundRigidBody->setRestitution(.01);// rubber
    //groundRigidBody->setFriction(.3); // agar??
    groundRigidBody->setFriction(.9);
    dynamicsWorld->addRigidBody(groundRigidBody);
    
}

void Playground::step(){
    /** STEP BULLET **/
    dynamicsWorld->stepSimulation(1 / 240.0f, 1, 1/240.f);
    
    //dropNeurons();
    //moveMuscles();
    
    //moveCenterCylinder();
}

void Playground::initNeurons(){
    /* BALLS */
    for (int i = 0; i < 302; i++){
        btCollisionShape* tSphere = new btSphereShape(1);
        btDefaultMotionState* tSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), 500000*btVector3(fakeStewie->nim[i]->massPoint->center.x, fakeStewie->nim[i]->massPoint->center.y,fakeStewie->nim[i]->massPoint->center.z)));
        btScalar mass = 1;
        btVector3 fallInertia(0, 0, 0);
        tSphere->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, tSphereMotionState, tSphere, fallInertia);
        btRigidBody* sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
        sphereRigidBody->setRestitution(.65); //glass marbles
        sphereRigidBody->setFriction(.9);// glass?
        dynamicsWorld->addRigidBody(sphereRigidBody);
        neuron_bodies.push_back(sphereRigidBody);
        if (i >= 1){
        p2p_neuron_cons.push_back(btPoint2PointConstraint(*neuron_bodies[i-1], *neuron_bodies[i], neuron_bodies[i]->getWorldTransform().getOrigin(),neuron_bodies[i-1]->getWorldTransform().getOrigin()));
        }
    }
    for (int i = 0; i < p2p_neuron_cons.size(); i++){
    dynamicsWorld->addConstraint(&p2p_neuron_cons[i]);
    }
}

void Playground::dropNeurons(){
    btTransform trans;
    for(int i = 0; i < neuron_bodies.size(); i++){
        neuron_bodies[i]->getMotionState()->getWorldTransform(trans);
        
        
        //std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
        //btMatrix3x3 basis = trans.getBasis();
        btVector3 origin = trans.getOrigin();
        //glm::vec3 center = glm::vec3(origin.getX(), origin.getY(), origin.getZ());
        //positions.push_back(center);
         fakeStewie->nim[i]->massPoint->center.x = origin.getX()/500000.0f;
         fakeStewie->nim[i]->massPoint->center.y = origin.getY()/500000.0f;
         fakeStewie->nim[i]->massPoint->center.z = origin.getZ()/500000.0f;
    }
}

void Playground::initMuscles(){
    
    for (int i = 0; i < fakeStewie->NUM_MUSCLES; i++){
        btCollisionShape* tSphere = new btSphereShape(.01);
        glm::vec3 anteriorPos = fakeStewie->bodyMassPoints[i*3]->center;
        btDefaultMotionState* anteriorSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(anteriorPos.x, anteriorPos.y, anteriorPos.z)));
        glm::vec3 centerPos = fakeStewie->bodyMassPoints[i*3+1]->center;
        btDefaultMotionState* centerSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(centerPos.x, centerPos.y, centerPos.z)));
        glm::vec3 posteriorPos = fakeStewie->bodyMassPoints[i*3+2]->center;
        btDefaultMotionState* posteriorSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(posteriorPos.x, posteriorPos.y, posteriorPos.z)));
        btScalar mass = 1;
        btVector3 fallInertia(0, 0, 0);
        tSphere->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo anteriorSphereRigidBodyCI(mass, anteriorSphereMotionState, tSphere, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo centerSphereRigidBodyCI(mass, centerSphereMotionState, tSphere, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo posteriorSphereRigidBodyCI(mass, posteriorSphereMotionState, tSphere, fallInertia);
        btRigidBody* anteriorSphereRigidBody = new btRigidBody(anteriorSphereRigidBodyCI);
        btRigidBody* centerSphereRigidBody = new btRigidBody(centerSphereRigidBodyCI);
        btRigidBody* posteriorSphereRigidBody = new btRigidBody(posteriorSphereRigidBodyCI);
        anteriorSphereRigidBody->setRestitution(.65); //glass marbles
        centerSphereRigidBody->setRestitution(.65); //glass marbles
        posteriorSphereRigidBody->setRestitution(.65); //glass marbles
        anteriorSphereRigidBody->setFriction(.9);// glass?
        centerSphereRigidBody->setFriction(.9);// glass?
        posteriorSphereRigidBody->setFriction(.9);// glass?
        dynamicsWorld->addRigidBody(anteriorSphereRigidBody);
        dynamicsWorld->addRigidBody(centerSphereRigidBody);
        dynamicsWorld->addRigidBody(posteriorSphereRigidBody);
        muscles.push_back(anteriorSphereRigidBody);
        muscles.push_back(centerSphereRigidBody);
        muscles.push_back(posteriorSphereRigidBody);
        float z_shift = muscles[3*i+1]->getWorldTransform().getOrigin().getZ() - muscles[3*i]->getWorldTransform().getOrigin().getZ();
        btTransform* tOne = new btTransform();
        btTransform* tTwo = new btTransform();
        btTransform* tOne2 = new btTransform();
        btTransform* tTwo2 = new btTransform();
        tOne->setIdentity();
        tTwo->setIdentity();
        tOne2->setIdentity();
        tTwo2->setIdentity();
        // only need two... tOne for 0 to 1, tTwo for 1 to 0, tOne for 1 to 2, and tTwo for 2 to 1... but, idk what happens to the objects.. so, we'll make new ones.
        tOne->setOrigin(btVector3(0,0,z_shift));
        tTwo->setOrigin(btVector3(0,0,-z_shift));
        tOne2->setOrigin(btVector3(0,0,z_shift));
        tTwo2->setOrigin(btVector3(0,0,-z_shift));
        btGeneric6DofSpring2Constraint* zeroToOneCon = new btGeneric6DofSpring2Constraint(*muscles[3*i], *muscles[3*i+1], *tOne, *tTwo);
        btGeneric6DofSpring2Constraint* oneToTwoCon = new btGeneric6DofSpring2Constraint(*muscles[3*i+1], *muscles[3*i+2], *tOne2, *tTwo2);
        zeroToOneCon->setLimit(0,1,-1); // unlimited - x
        zeroToOneCon->setLimit(1,1,-1); // unlimited - y
        zeroToOneCon->setLimit(2,1,-1); // unlimited - z
        //zeroToOne->setLimit(3,1, -1); // unlimited - x (pitch?)
        //zeroToOne->setLimit(4,1, -1); // unlimited - y (yaw?)
        //zeroToOne->setLimit(5,1, -1); // unlimited - z (roll?)
        zeroToOneCon->enableSpring(2, true);
        zeroToOneCon->setStiffness(2, 1);
        zeroToOneCon->setDamping(2,.5);
        oneToTwoCon->setLimit(0,1,-1); // unlimited - x
        oneToTwoCon->setLimit(1,1,-1); // unlimited - y
        oneToTwoCon->setLimit(2,1,-1); // unlimited - z
        //oneToTwoCon->setLimit(3,1, -1); // unlimited - x (pitch?)
        //oneToTwoCon->setLimit(4,1, -1); // unlimited - y (yaw?)
        //oneToTwoCon->setLimit(5,1, -1); // unlimited - z (roll?)
        oneToTwoCon->enableSpring(2,true);
        oneToTwoCon->setStiffness(2,1);
        oneToTwoCon->setDamping(2,.5);
        
        
        muscle_cons.push_back(zeroToOneCon);
        muscle_cons.push_back(oneToTwoCon);
        //if (muscles.size() >=12)
        //    break;
    }
    for (int i = 0; i < muscle_cons.size(); i++){
        dynamicsWorld->addConstraint(muscle_cons[i]);
    }
}

void Playground::initCenterCylinder(){
    
    for (int i = 0; i < fakeStewie->NUM_CC; i++){
        // halfExtent => (height, radius, radius)
        float scaled_radius = (fakeStewie->CENTER_CYLINDER_DIAM/2.0f);
        float scaled_half_height = (fakeStewie->CENTER_CYLINDER_HEIGHT/2.0f);
        btCylinderShapeZ* tCylinder = new btCylinderShapeZ(btVector3(scaled_radius, scaled_radius, scaled_half_height));
        float poop = tCylinder->getRadius();
        glm::vec3 pos = fakeStewie->centerCylinderMassPoints[i*3+1]->center; // center of cylinder (i*3) is front, (i*3+2) is back.
        btScalar mass = 0.0f;
        if (i == 0){
        mass = 0.0f;
        }
        btDefaultMotionState* cylinderMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
        btVector3 fallInertia(0, 0, 0);
        tCylinder->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo cylinderRigidBodyCI(mass, cylinderMotionState, tCylinder, fallInertia);
        btRigidBody* cylinderRigidBody = new btRigidBody(cylinderRigidBodyCI);
        cylinderRigidBody->setRestitution(.65); //glass marbles
        cylinderRigidBody->setFriction(.9);// glass?
        dynamicsWorld->addRigidBody(cylinderRigidBody);
        centerCylinder.push_back(cylinderRigidBody);
        
        if (centerCylinder.size() > 1){
            float pre_z = centerCylinder[i-1]->getWorldTransform().getOrigin().getZ();
            float post_z = centerCylinder[i]->getWorldTransform().getOrigin().getZ();
            btTransform* tOne = new btTransform();
            btTransform* tTwo = new btTransform();
            tOne->setIdentity();
            tTwo->setIdentity();
            tOne->setOrigin(btVector3(0,0,scaled_half_height));
            tTwo->setOrigin(btVector3(0,0,- scaled_half_height));
            centerCylinder_cons.push_back(new btGeneric6DofSpring2Constraint(*centerCylinder[i-1], *centerCylinder[i], *tOne, *tTwo ));
            
            centerCylinder_cons[centerCylinder_cons.size()-1]->setLimit(0,0,0);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setLimit(1,0,0);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setLimit(2,0,0);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setLimit(3,1,0);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setLimit(4,1,0);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setLimit(5,1,0);
            
            centerCylinder_cons[centerCylinder_cons.size()-1]->enableSpring(0,false);
            centerCylinder_cons[centerCylinder_cons.size()-1]->enableSpring(1,false);
            centerCylinder_cons[centerCylinder_cons.size()-1]->enableSpring(2,false);
            
            centerCylinder_cons[centerCylinder_cons.size()-1]->setDamping(0,100);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setDamping(1,100);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setDamping(2,100);
            
            centerCylinder_cons[centerCylinder_cons.size()-1]->setStiffness(0,1000);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setStiffness(1,1000);
            centerCylinder_cons[centerCylinder_cons.size()-1]->setStiffness(2,1000);
        }
        
    }
    for (int i = 0; i < centerCylinder_cons.size(); i++){
        dynamicsWorld->addConstraint(centerCylinder_cons[i]);
    }
}


int i_m = 0;
bool flip = false;
void Playground::moveMuscles(){
    btTransform anteriorTrans;
    btTransform centerTrans;
    btTransform posteriorTrans;
    int mim_index = 0;
    for(int i = 0; i < fakeStewie->NUM_MUSCLES; i++){
        muscles[3*i]->getMotionState()->getWorldTransform(anteriorTrans);
        muscles[3*i+1]->getMotionState()->getWorldTransform(centerTrans);
        muscles[3*i+2]->getMotionState()->getWorldTransform(posteriorTrans);
        
       
        
        
        //std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
        //btMatrix3x3 basis = trans.getBasis();
        btVector3 anteriorOrigin = anteriorTrans.getOrigin();
        btVector3 centerOrigin = centerTrans.getOrigin();
        btVector3 posteriorOrigin = posteriorTrans.getOrigin();
        
        /*
        btQuaternion q_a = anteriorTrans.getRotation();
        btQuaternion q_p = posteriorTrans.getRotation();
        fakeStewie->mim[mim_index]->rotAng = q_a.getAngle() + q_p.getAngle();
        fakeStewie->mim[mim_index]->rotAxis = glm::vec3(q_a.getAxis().getX() + q_p.getAxis().getX(),q_a.getAxis().getY() + q_p.getAxis().getY(),q_a.getAxis().getZ() + q_p.getAxis().getZ());
         */
        
         
        //glm::vec3 center = glm::vec3(origin.getX(), origin.getY(), origin.getZ());
        //positions.push_back(center);
        fakeStewie->mim[mim_index]->anteriorMassPoint->center.x = anteriorOrigin.getX();
        fakeStewie->mim[mim_index]->anteriorMassPoint->center.y = anteriorOrigin.getY();
        fakeStewie->mim[mim_index]->anteriorMassPoint->center.z = anteriorOrigin.getZ();
        fakeStewie->mim[mim_index]->centerMassPoint->center.x = centerOrigin.getX();
        fakeStewie->mim[mim_index]->centerMassPoint->center.y = centerOrigin.getY();
        fakeStewie->mim[mim_index]->centerMassPoint->center.z = centerOrigin.getZ();
        fakeStewie->mim[mim_index]->posteriorMassPoint->center.x = posteriorOrigin.getX();
        fakeStewie->mim[mim_index]->posteriorMassPoint->center.y = posteriorOrigin.getY();
        fakeStewie->mim[mim_index]->posteriorMassPoint->center.z = posteriorOrigin.getZ();
        
        
       
        
        /*
        btVector3 c = btVector3(fakeStewie->mim[mim_index]->centerMassPoint->center.x, fakeStewie->mim[mim_index]->centerMassPoint->center.y, fakeStewie->mim[mim_index]->centerMassPoint->center.z);
        
         btVector3 f1 = c.normalize();
        btVector3 f2 = -c.normalize();
        muscles[i]->applyCentralForce(f1);
       muscles[i+1]->applyCentralForce(f2);
  */
        
      
        
        mim_index++;
        //if (i >= 3)
            //break;
    }
    
}


void Playground::moveCenterCylinder(){
    btTransform trans;
    int cc_index = 0;
    for(int i = 0; i < centerCylinder.size(); i++){
        centerCylinder[i]->getMotionState()->getWorldTransform(trans);
        
       
        
        
        //std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
        //btMatrix3x3 basis = trans.getBasis();
        btVector3 origin = trans.getOrigin();
        
        btQuaternion quat = trans.getRotation();
        glm::vec3 glm_origin(origin.getX(), origin.getY(), origin.getZ());
        
        glm::vec3 anterior = glm_origin - glm::vec3(0.0f, 0.0f, fakeStewie->CENTER_CYLINDER_HEIGHT/2.0f);
        glm::vec3 posterior = glm_origin + glm::vec3(0.0f, 0.0f, fakeStewie->CENTER_CYLINDER_HEIGHT/2.0f);
        
        float angle = quat.getAngle();
        glm::vec3 axis(quat.getAxis().getX(), quat.getAxis().getY(),quat.getAxis().getZ());
        anterior = glm::rotate(anterior, angle, axis);
        posterior = glm::rotate(posterior, angle, axis);
        
        fakeStewie->centerCylinderMassPoints[cc_index*3]->center = anterior;
        fakeStewie->centerCylinderMassPoints[cc_index*3+1]->center = glm_origin;
        fakeStewie->centerCylinderMassPoints[cc_index*3+2]->center = posterior;
        
        /*
        btVector3 c = btVector3(fakeStewie->mim[mim_index]->centerMassPoint->center.x, fakeStewie->mim[mim_index]->centerMassPoint->center.y, fakeStewie->mim[mim_index]->centerMassPoint->center.z);
        
         btVector3 f1 = c.normalize();
        btVector3 f2 = -c.normalize();
        muscles[i]->applyCentralForce(f1);
       muscles[i+1]->applyCentralForce(f2);
         */
    
        
      
        
        cc_index++;
    }
    
}


void Playground::connectMusclesToCylinder(){
   /* for each set of 4 "muscles" (group of 3 body MPs),
    * we want to connect the first MP to the front of the first cylinder,
    * the second MP to the end of the first cylinder,
    * and the third to the end of the second cylinder.
    *
    * for the second set of 4 "muscles",
    * we want to conenct the first MP to the front of the second cylinder,
    * the second MP to the end of the second cylinder,
    * and the third to the end of the third cylinder.
    * 
    * and so on...
    */
    int cur_cc_index = 0; // current center cylinder index
    for (int i = 0; i < fakeStewie->NUM_MUSCLES; i++){ // muscles are modled as 3 spheres, so, access them in the muscles[] like 3i, 3i+1, 3i+2.
        int quadrant = i % 4;
        btVector3 cur_cylinder_origin = centerCylinder[cur_cc_index]->getWorldTransform().getOrigin();
        btVector3 cur_cylinder_start = centerCylinder[cur_cc_index]->getWorldTransform().getOrigin() - btVector3(0,0,fakeStewie->CENTER_CYLINDER_HEIGHT/2.0f);
        btVector3 cur_cylinder_end = centerCylinder[cur_cc_index]->getWorldTransform().getOrigin() + btVector3(0,0,fakeStewie->CENTER_CYLINDER_HEIGHT/2.0f);
        ///////////////// first MP to front of first cylinder (and later, after 4 "muscles", first MP of second group to front of second cylinder, etc.)
        btTransform* tOne = new btTransform();
        btTransform* tTwo = new btTransform();
        btVector3 transOneA =  cur_cylinder_start - muscles[3*i]->getWorldTransform().getOrigin();
        btVector3 transOneB =  muscles[3*i]->getWorldTransform().getOrigin() - cur_cylinder_start;
        tOne->setIdentity();
        tTwo->setIdentity();
        tOne->setOrigin(transOneA);
        tTwo->setOrigin(transOneB);
        btGeneric6DofSpring2Constraint* conOne = new btGeneric6DofSpring2Constraint(*muscles[3*i], *centerCylinder[cur_cc_index], *tOne, *tTwo);
        ///////////////// second MP to end of first cylinder, (and later, second MP of next set of muscles to end of second cylinder, etc.)
        btTransform* tOne2 = new btTransform();
        btTransform* tTwo2 = new btTransform();
        btVector3 transOneA2 =  cur_cylinder_end - muscles[3*i+1]->getWorldTransform().getOrigin();
        btVector3 transOneB2 =  muscles[3*i+1]->getWorldTransform().getOrigin() - cur_cylinder_end;
        tOne2->setIdentity();
        tTwo2->setIdentity();
        tOne2->setOrigin(transOneA2);
        tTwo2->setOrigin(transOneB2);
        btGeneric6DofSpring2Constraint* conTwo = new btGeneric6DofSpring2Constraint(*muscles[3*i+1], *centerCylinder[cur_cc_index], *tOne2, *tTwo2);
        ///////////////// third MP to end of second cylinder, (and later, third MP of next set of muscles to end of third cylinder, etc.)
        btVector3 next_cylinder_end = centerCylinder[cur_cc_index+1]->getWorldTransform().getOrigin() + btVector3(0,0,fakeStewie->CENTER_CYLINDER_HEIGHT/2.0f);
        btTransform* tOne3 = new btTransform();
        btTransform* tTwo3 = new btTransform();
        btVector3 transOneA3 =  next_cylinder_end - muscles[3*i+2]->getWorldTransform().getOrigin();
        btVector3 transOneB3 =  muscles[3*i+2]->getWorldTransform().getOrigin() - next_cylinder_end;
        tOne3->setIdentity();
        tTwo3->setIdentity();
        tOne3->setOrigin(transOneA3);
        tTwo3->setOrigin(transOneB3);
        btGeneric6DofSpring2Constraint* conThree = new btGeneric6DofSpring2Constraint(*muscles[3*i+2], *centerCylinder[cur_cc_index+1], *tOne3, *tTwo3);
        
        muscle_to_cc_cons.push_back(conOne);
        muscle_to_cc_cons.push_back(conTwo);
        muscle_to_cc_cons.push_back(conThree);
        if (quadrant >= 3){
            cur_cc_index++;
            //break;
        }
    }
    
    for (int i = 0; i < muscle_to_cc_cons.size(); i++){
       
        /*
        muscle_to_cc_cons[i]->setLimit(0,1, 0); // unlimited - x
        muscle_to_cc_cons[i]->setLimit(1,1, 0); // unlimited - y
        muscle_to_cc_cons[i]->setLimit(2,1, 0); // unlimited - z
        muscle_to_cc_cons[i]->enableSpring(0, true);
        muscle_to_cc_cons[i]->enableSpring(1, true);
        muscle_to_cc_cons[i]->enableSpring(2, true);
        muscle_to_cc_cons[i]->setStiffness(0, 1000);
        muscle_to_cc_cons[i]->setStiffness(1, 1000);
        muscle_to_cc_cons[i]->setStiffness(2, 1000);
        muscle_to_cc_cons[i]->setDamping(0,100);
        muscle_to_cc_cons[i]->setDamping(1,100);
        muscle_to_cc_cons[i]->setDamping(2,100);
        */
        
        dynamicsWorld->addConstraint(muscle_to_cc_cons[i]);
    }
    
}