/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2015 Google Inc. http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/*
 * some initial opengl code from http://stackoverflow.com/questions/2350167/basic-skeleton-for-a-c-opengl-program-on-osx
 * (first answer -- Ned's post)
 */

#include "TheRealDeal.hpp"
#include "Logger.hpp"
#include <thread>
#include <chrono>
#include <cstdlib>


#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

float*** muscle_activations;
int num_muscle_activations;

Logger log_;
size_t ESN_GLOB;

BodyParameterSet* paramSet = new BodyParameterSet();
btAlignedObjectArray<btCollisionShape*> collisionShapes;
////
// MUSCLE INDEXING EXPLAINED
////
// basically, we take the section number, which divides the muscles into groups of 4,
// and multiply that by 4, to get the 'base' index for that group of muscles (muscle_group_index).
// then, we add either a 0, 1, 2, or 3 to get to the actual muscle index.
// Next, we multiply that by 3, because each muscle is made up of 3 objects (all stored in 'muscles')
// That will give us the base index for the actual muscle, and then if we want to access the:
// -- anterior part of the muscle, we add nothing, becaust that is the first 'part' of the muscle
// -- center part of the muscle, we add 1
// -- posterior part of the muscle, we add 2.
//
// For example:
// =====> if we want to access the center of the bottom right muscle in the 4th section from the head:
// --> our section number is: 3 (indexing starts at 0)
// ----> multiply that by 4 to get the muscle index, 4*3 = 12
// --> we want the bottom right muscle, and since the top left is 0, top right is 1, and bottom right is 2,
// we want the 3rd muscle in that 'section' (indexing starts at 0), and we add that to our 12, so,
// 12 + 2 = 14 ... and so, it is the 15th muscle (muscle indexing also starts at 0).
// --> since each muscle has 3 parts, we multiply 14 by 3, 14 * 3 = 42
// --> this means that in our 'muscles' array, 42 is the index of the anterior part of the 15th muscle
// --> finally, we add 1 to 42, 42 + 1 = 43
// --> to access the center sphere of the 15th muscle, we call:
// ==> muscles[43] or, alternatively
// ==> muscles[(((3 * 4) + 2) *3) + 1]
std::vector<btRigidBody*> muscles;
std::vector<btRigidBody*> centerCylinders;
std::vector<btGeneric6DofSpring2Constraint*> muscle_cons;
std::vector<btGeneric6DofSpring2Constraint*> cc_cons;
//std::vector<btPoint2PointConstraint*> center_muscle2cc_spring_cons;
std::vector<btGeneric6DofSpring2Constraint*> muscle2cc_spring_cons;
std::vector<btGeneric6DofSpring2Constraint*> muscle2muscle_spring_cons; // creating boxes of quadrants
std::vector<btGeneric6DofSpring2Constraint*> muscleEnds2muscleEnds_spring_cons; // tieing entire quadrants together

btVector3 prevAvgLoc;
btDiscreteDynamicsWorld* dynamicsWorld;
myDebug* debugDrawer;
SignalBuilder signalBuilder;
int simStepCount = 0;
int num_quads = 0;
bool instantDeath = false;
int instantDeathCount = 0;
int evolution_round = 1;
int windowCounter = 0;
int windowsBetweenPositionChecks = 20;
int posCheckNum = 0;

std::vector<std::vector<btVector3>> positionHistory;


float rangle_lim = M_PI/2.0f;

int sine_counter = 0;
int break_counter = 0;

int muscleActivationsIndex = 0;

//using time_device = std::chrono::system_clock;
//using time_point = time_device::time_point;
//time_point start = time_device::now();
//time_point stop;
float _exe_time_micro;
int rep_no = 1;

int mSteps = -1;

int getMaxSimSteps(){
    return 4*SignalBuilder::numWindows;
}

void start_timer(){
    //start = time_device::now();
}

void stop_timer(){
    //stop = time_device::now();
}

double get_exe_time_in_micro(){
    //auto duration = stop - start;
    //_exe_time_micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    return 0;//_exe_time_micro;
}

std::string vtos(btVector3* v){
    std::string s("(");
    s += std::to_string((*v)[0])+", ";
    s += std::to_string((*v)[1])+", ";
    s += std::to_string((*v)[2])+")";
    return s;
}

/**
 * Left if you are the worm
 */
void activate_left(int quadrant_index, float force_val){
    int mi = quadrant_index*4;
    // bottom
    btVector3 force1= muscles[3*mi+1]->getWorldTransform().getOrigin() - muscles[3*mi]->getWorldTransform().getOrigin();
    btVector3 force2 = muscles[3*mi+1]->getWorldTransform().getOrigin() - muscles[3*mi+2]->getWorldTransform().getOrigin();
    if (mi == 0) {
        //btVector3 tf1(force1.normalize()*force_val);
        //btVector3 tf2(force2.normalize()*force_val);
        //        printf("left (mi == %d): f1 - %s, f2 - %s\n",mi, vtos(&tf1).c_str(), vtos(&tf2).c_str());
        //printf("left (mi == %d) ... f1 -> %f ... %f <- f2\n",mi, tf1.length(), tf2.length());
    }
    muscles[3*mi]->applyImpulse(force1.normalize()*force_val, btVector3(0,0,0));
    muscles[3*mi+2]->applyImpulse(force2.normalize()*force_val, btVector3(0,0,0));
    // top
    if (mi != 92){ // there is no bottom left 24th muscle (ventral left) -- and we start counting at 0 (so, quadrant_index == 23 -> 24th quadrant)
        btVector3 force3= muscles[3*(mi+3)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+3)]->getWorldTransform().getOrigin();
        btVector3 force4 = muscles[3*(mi+3)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+3)+2]->getWorldTransform().getOrigin();
        muscles[3*(mi+3)]->applyImpulse(force3.normalize()*force_val, btVector3(0,0,0));
        muscles[3*(mi+3)+2]->applyImpulse(force4.normalize()*force_val, btVector3(0,0,0));
    }
}

void activate_right(int quadrant_index, float force_val){
    int mi = quadrant_index*4;
    // top
    btVector3 force3= muscles[3*(mi+1)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+1)]->getWorldTransform().getOrigin();
    btVector3 force4 = muscles[3*(mi+1)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+1)+2]->getWorldTransform().getOrigin();
    muscles[3*(mi+1)]->applyImpulse(force3.normalize()*force_val, btVector3(0,0,0));
    muscles[3*(mi+1)+2]->applyImpulse(force4.normalize()*force_val, btVector3(0,0,0));
    // bottom
    btVector3 force1= muscles[3*(mi+2)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+2)]->getWorldTransform().getOrigin();
    btVector3 force2 = muscles[3*(mi+2)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+2)+2]->getWorldTransform().getOrigin();
    muscles[3*(mi+2)]->applyImpulse(force1.normalize()*force_val, btVector3(0,0,0));
    muscles[3*(mi+2)+2]->applyImpulse(force2.normalize()*force_val, btVector3(0,0,0));
    
}

// NEW NEW NEW
void activate_muscle(std::string muscleName, float force_val){
    std::regex digitsAtEnd("([dv])(BWM)([LR])([0-9]+)");
    std::string inString;
    std::smatch match;
    int muscleNum = 0;
    MuscleQuadrant mq;
    if (std::regex_search(muscleName,match,digitsAtEnd)){
        // match[0] is full match
        // match[1] is 'd' or 'v'
        // match[2] is BWM
        // match[3] is 'L' or 'R"
        std::string dv(match[1].str());
        std::string LR(match[3].str());
        if (dv.compare("d") == 0){
            if (LR.compare("L") == 0) // it's DL
                mq = MuscleQuadrant::DL;
            else // it's DR
                mq = MuscleQuadrant::DR;
        }
        else {
            if (LR.compare("L") == 0) // it's VL
                mq = MuscleQuadrant::VL;
            else // it's VR
                mq = MuscleQuadrant::VR;
        }
        std::string sdigit(match[4].str());
        if (!sdigit.empty()){
            muscleNum = stoi(sdigit,0);
        }
        //std::cout << "digit => " << digit << std::endl;
    }
    else {
        return;
        //std::cout << "no match..." << std::endl;
    }
    int mi = muscleNum * 4;
    // VL
    switch (mq){
        case MuscleQuadrant::VL: {
            btVector3 VL_ab = muscles[3*mi+1]->getWorldTransform().getOrigin() - muscles[3*mi]->getWorldTransform().getOrigin();
            btVector3 VL_cb = muscles[3*mi+1]->getWorldTransform().getOrigin() - muscles[3*mi+2]->getWorldTransform().getOrigin();
            muscles[3*mi]->applyImpulse(VL_ab.normalize()*force_val, btVector3(0,0,0));
            muscles[3*mi+2]->applyImpulse(VL_cb.normalize()*force_val, btVector3(0,0,0));
            break;
        }
        case MuscleQuadrant::DL:{
            // DL
            if (mi != 92){ // there is no bottom left 24th muscle (ventral left) -- and we start counting at 0 (so, quadrant_index == 23 -> 24th quadrant)
                btVector3 force3= muscles[3*(mi+3)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+3)]->getWorldTransform().getOrigin();
                btVector3 force4 = muscles[3*(mi+3)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+3)+2]->getWorldTransform().getOrigin();
                muscles[3*(mi+3)]->applyImpulse(force3.normalize()*force_val, btVector3(0,0,0));
                muscles[3*(mi+3)+2]->applyImpulse(force4.normalize()*force_val, btVector3(0,0,0));
            }
            break;
        }
        case MuscleQuadrant::DR:{
            // DR
            btVector3 force3= muscles[3*(mi+1)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+1)]->getWorldTransform().getOrigin();
            btVector3 force4 = muscles[3*(mi+1)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+1)+2]->getWorldTransform().getOrigin();
            muscles[3*(mi+1)]->applyImpulse(force3.normalize()*force_val, btVector3(0,0,0));
            muscles[3*(mi+1)+2]->applyImpulse(force4.normalize()*force_val, btVector3(0,0,0));
            break;
        }
        case MuscleQuadrant::VR:{
            // VR
            btVector3 force1= muscles[3*(mi+2)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+2)]->getWorldTransform().getOrigin();
            btVector3 force2 = muscles[3*(mi+2)+1]->getWorldTransform().getOrigin() - muscles[3*(mi+2)+2]->getWorldTransform().getOrigin();
            muscles[3*(mi+2)]->applyImpulse(force1.normalize()*force_val, btVector3(0,0,0));
            muscles[3*(mi+2)+2]->applyImpulse(force2.normalize()*force_val, btVector3(0,0,0));
            break;
        }
        default:
            printf("This isn't good... didn't hit a MuscleQuadrant in muscle activation switch statement.\n");
            break;
    }
    
}

void newPTCB(btDynamicsWorld* world, btScalar timeStep){
    std::unordered_map<std::string,float> theseActivationsGJ = DataSteward::muscleActivationsGJ[muscleActivationsIndex];
    std::unordered_map<std::string,float> theseActivationsCS = DataSteward::muscleActivationsCS[muscleActivationsIndex];
    std::unordered_map<std::string,float>::iterator iter = theseActivationsGJ.begin();
    std::unordered_map<std::string,float>::iterator iterTwo = theseActivationsCS.begin();
    std::unordered_map<std::string,float> totalActivations;
    while (iter != theseActivationsGJ.end()){
        //printf("%s => %.2f\n",iter->first.c_str(), iter->second*force_scale);
        if (totalActivations.find(iter->first) == totalActivations.end()){
            totalActivations[iter->first] = 0;
        }
        totalActivations[iter->first] += iter->second;
        iter++;
    }
    while (iterTwo != theseActivationsCS.end()){
        if (totalActivations.find(iterTwo->first) == totalActivations.end()){
            totalActivations[iterTwo->first] = 0;
        }
        totalActivations[iterTwo->first] += iterTwo->second;
        iterTwo++;
    }
    auto iterThree = totalActivations.begin();
    float force_scale = -50;
    while (iterThree != totalActivations.end()){
        printf("%s => %.2f\n",iterThree->first.c_str(), iterThree->second*force_scale);
        activate_muscle(iterThree->first, iterThree->second*force_scale);
        iterThree++;
    }
    muscleActivationsIndex++;
    if (muscleActivationsIndex >= Core::numMainLoops){
       printf("REPLAY REPLAY REPLAY\n");
       muscleActivationsIndex = 0;
   }
    
}

void bEPreTickCallback (btDynamicsWorld *world, btScalar timeStep)
{
    //stop_timer();
    //printf("time: %f\n", get_exe_time_in_micro());
    
    
    std::vector<std::vector<float>>* wave = signalBuilder.getNextWindow();
    // impulses -- number of repeated impluses, combined with force, dictates speed and shit
    //int num_reps = 1;
    //int start = sine_counter*4;
    sine_counter = 0;
    
    //for (int i = 0; i < num_quads; ++i){
    //    if ((*wave)[i].size() != 2){
    //        printf("oh HELL no.\n");
    //        exit(0); // haha, sorry sean.
    //    }
        //activate_left(i,(*wave)[i][0]);
        //activate_right(i,(*wave)[i][1]);
        
    //}
    
    windowCounter++;
    // motor/servo -- servo target impacts movement (negative brings toward sphere)
    //muscle_cons[0]->setServoTarget(2,-10);
    //muscle_cons[1]->setServoTarget(2,-10);
    
    
    //start_timer();
}

// spring from ends of muscle to center cylinder
void configureMuscleCon(btGeneric6DofSpring2Constraint* con, bool lastQuadrant, BodyParameterSet* paramSet){
    for (int j = 0; j < 6; j++){
        con->setParam(BT_CONSTRAINT_CFM,0,j);
        con->setParam(BT_CONSTRAINT_STOP_CFM,0,j);
        con->setParam(BT_CONSTRAINT_ERP,.8,j);
        con->setParam(BT_CONSTRAINT_STOP_ERP,.8,j);
        con->setStiffness(j,paramSet->muscleConSprings[j].stiffness);
        con->setDamping(j,paramSet->muscleConSprings[j].damping);
        con->setBounce(j, paramSet->muscleConSprings[j].bounce);
        con->setEquilibriumPoint(j);
        con->enableSpring(j, paramSet->muscleConSprings[j].enabled);
        con->setLimit(j,paramSet->muscleConSprings[j].lowerLim,paramSet->muscleConSprings[j].upperLim);
    }
    /*
    if (lastQuadrant){ // they get pulled more
        con->setLimit(0,0,0);
        con->enableSpring(0, false);
        con->setLimit(2,0,0);
        con->enableSpring(2, false);
        con->setLimit(4,0,0);
        con->enableSpring(4, false);
    }
     */
}

// big sphere to center cylinder chain
void configureCenterToMuscleCon(btGeneric6DofSpring2Constraint* con, BodyParameterSet* paramSet){
    for (int j = 0; j < 6; j++){
        con->setParam(BT_CONSTRAINT_CFM,0,j);
        con->setParam(BT_CONSTRAINT_STOP_CFM,0,j);
        con->setParam(BT_CONSTRAINT_ERP,.8,j);
        con->setParam(BT_CONSTRAINT_STOP_ERP,.8,j);
        con->setStiffness(j,paramSet->centerToMuscleConSprings[j].stiffness);
        con->setDamping(j,paramSet->centerToMuscleConSprings[j].damping);
        con->setBounce(j, paramSet->centerToMuscleConSprings[j].bounce);
        con->setEquilibriumPoint(j);
        con->enableSpring(j, paramSet->centerToMuscleConSprings[j].enabled);
        con->setLimit(j,paramSet->centerToMuscleConSprings[j].lowerLim,paramSet->centerToMuscleConSprings[j].upperLim);
    }
}

// center cylinder chain joints
void configureCenterCylinderChainCon(btGeneric6DofSpring2Constraint* con, BodyParameterSet* paramSet){
    for (int j = 0; j < 6; j++){
        con->setParam(BT_CONSTRAINT_CFM,0,j);
        con->setParam(BT_CONSTRAINT_STOP_CFM,0,j);
        con->setParam(BT_CONSTRAINT_ERP,.8,j);
        con->setParam(BT_CONSTRAINT_STOP_ERP,.8,j);
        con->setStiffness(j,paramSet->centerCylinderChainConSprings[j].stiffness);
        con->setDamping(j,paramSet->centerCylinderChainConSprings[j].damping);
        con->setBounce(j, paramSet->centerCylinderChainConSprings[j].bounce);
        con->setEquilibriumPoint(j);
        con->enableSpring(j, paramSet->centerCylinderChainConSprings[j].enabled);
        con->setLimit(j,paramSet->centerCylinderChainConSprings[j].lowerLim,paramSet->centerCylinderChainConSprings[j].upperLim);
    }
}

/**
 * ends of muscles to center cylinder
 */
void configureMuscleEndToCenterCylinderCon(btGeneric6DofSpring2Constraint* con, BodyParameterSet* paramSet){
     for (int j = 0; j < 6; j++){
        con->setParam(BT_CONSTRAINT_CFM,0,j);
        con->setParam(BT_CONSTRAINT_STOP_CFM,0,j);
        con->setParam(BT_CONSTRAINT_ERP,.8,j);
        con->setParam(BT_CONSTRAINT_STOP_ERP,.8,j);
        con->setStiffness(j,paramSet->muscleEndToCenterCylinderSprings[j].stiffness);
        con->setDamping(j,paramSet->muscleEndToCenterCylinderSprings[j].damping);
        con->setBounce(j, paramSet->muscleEndToCenterCylinderSprings[j].bounce);
        con->enableSpring(j,paramSet->muscleEndToCenterCylinderSprings[j].enabled);
        con->setLimit(j,paramSet->muscleEndToCenterCylinderSprings[j].lowerLim,paramSet->muscleEndToCenterCylinderSprings[j].upperLim);
        con->setEquilibriumPoint(j);
    }
}

/**
 * box type shit
 */
void configureCenterMuscleSupportCon(btGeneric6DofSpring2Constraint* con, int quadrant, BodyParameterSet* paramSet){
    
    for (int j = 0; j < 6; j++){
        con->setParam(BT_CONSTRAINT_CFM,0,j);
        con->setParam(BT_CONSTRAINT_STOP_CFM,0,j);
        con->setParam(BT_CONSTRAINT_ERP,.8,j);
        con->setParam(BT_CONSTRAINT_STOP_ERP,.8,j);
        con->setStiffness(j,paramSet->centerMuscleSupportSprings[j].stiffness);
        con->setDamping(j,paramSet->centerMuscleSupportSprings[j].damping);
        con->setBounce(j, paramSet->centerMuscleSupportSprings[j].bounce);
        con->enableSpring(j, paramSet->centerMuscleSupportSprings[j].enabled);
        con->setLimit(j,paramSet->centerMuscleSupportSprings[j].lowerLim,paramSet->centerMuscleSupportSprings[j].upperLim);
        con->setEquilibriumPoint(j);
    }
    /*
    if (quadrant == 0 || quadrant == 2){ // then it's going from 0 -> 1 or 2 -> 3, and those are both horizontal
        con->enableSpring(0, true);
    }
    else if (quadrant == 1 || quadrant == 3){ // then it's going from 1 -> 2 or 3 -> 0, and those are both vertical
        con->enableSpring(1, true);
    }
     */
}

/**
 * links to centers of other muscles
 */
void configureEndsOfMusclesSupportCon(btGeneric6DofSpring2Constraint* con, BodyParameterSet* paramSet){
    // lateral
     for (int j = 0; j < 6; j++){
        con->setParam(BT_CONSTRAINT_CFM,0,j);
        con->setParam(BT_CONSTRAINT_STOP_CFM,0,j);
        con->setParam(BT_CONSTRAINT_ERP,.8,j);
        con->setParam(BT_CONSTRAINT_STOP_ERP,.8,j);
        con->setStiffness(j,paramSet->endsOfMusclesSupportSprings[j].stiffness);
        con->setDamping(j, paramSet->endsOfMusclesSupportSprings[j].damping);
        con->setBounce(j, paramSet->endsOfMusclesSupportSprings[j].bounce);
        con->enableSpring(j,paramSet->endsOfMusclesSupportSprings[j].enabled);
        con->setEquilibriumPoint(j);
        con->setLimit(j,paramSet->endsOfMusclesSupportSprings[j].lowerLim,paramSet->endsOfMusclesSupportSprings[j].upperLim);
    }
}


void buildMuscle(btDynamicsWorld* m_dynamicsWorld, BodyParameterSet* paramSet, int muscleNum, int dumbOffset){
    int i = muscleNum;
    int quadrant = i % 4;
    int muscle_row = i / 4;
    int muscle_front_back = i % 8;
    int side = 0;
    bool outside_muscle = true;
    float small_sphere_size = .2;
    float large_sphere_size = .5;
    if (muscle_front_back < 4){ // front, so, outside muscle
        outside_muscle = false;
    }
    int height = 1;
    int ant_side = 0; // for the very front of the worm
    int ant_height = 0; // for the very front of the worm
    int post_side = 0; // for the very back of the worm
    int post_height = 0; // for the bery back of the worm
    float v_end_adjustment = 0.f;
    float h_end_adjustment = 0.f;
    bool atFront = false;
    bool atBack = false;
    if (muscleNum < 4 ) { // we're at the front or back, and want to tie the muscles closer together
        atFront = true;
        v_end_adjustment = 2.f;
        h_end_adjustment = 1.f;
    }
    else if (muscleNum > 91){
        atBack = true;
        v_end_adjustment = 2.5f;
        h_end_adjustment = 2.f;
    }
    
    int muscle_row_offset = muscle_row*5;//starts with 0 -- forgot to note this when i wrote it, but i think 5 is the spacing between corresponding muscle parts from one row to the next
    // quatrants: if -z coming at you: top right is first, top left is second, bottom left is third, bottom right is fourth
    if (quadrant == 0 || quadrant == 1){
        height = 6 + height;
        if (atFront){
            ant_height = height - v_end_adjustment;
            post_height = height;
        }
        else if (atBack){
            post_height = height - v_end_adjustment;
            ant_height = height;
        }
        else{
            ant_height = post_height = height;
        }
    }
    else {
        height = height;
        if (atFront){
            ant_height = height + v_end_adjustment;
            post_height = height;
        }
        else if (atBack){
            post_height = height + v_end_adjustment;
            ant_height = height;
        }
        else{
            ant_height = post_height = height;
        }
    }
    
    if (outside_muscle){
        if(quadrant == 0 || quadrant == 3){
            side = side - ((small_sphere_size)+(2*large_sphere_size));
            if (atFront){
                ant_side = side + h_end_adjustment;
                post_side = side;
            }
            else if (atBack){
                post_side = side + h_end_adjustment;
                ant_side = side;
            }
            else{
                ant_side = post_side = side;
            }
        }
        else{
            side = 4 + ((small_sphere_size)+(2*large_sphere_size));
            if (atFront){
                ant_side = side - h_end_adjustment;
                post_side = side;
            }
            else if (atBack){
                post_side = side - h_end_adjustment;
                ant_side = side;
            }
            else{
                ant_side = post_side = side;
            }
        }
    } else {
        if(quadrant == 1 || quadrant == 2){
            side = 4;
            if (atFront){
                ant_side = side - h_end_adjustment;
                post_side = side;
            }
            else if (atBack){
                post_side = side - h_end_adjustment;
                ant_side = side;
            }
            else{
                ant_side = post_side = side;
            }
        }
        else {
            if (atFront){
                ant_side = side + h_end_adjustment;
                post_side = side;
            }
            else if (atBack){
                post_side = side + h_end_adjustment;
                ant_side = side;
            }
            else{
                ant_side = post_side = side;
            }
        }
    }
    
    btCollisionShape* tSphere = new btSphereShape(large_sphere_size);
    btCollisionShape* stSphere = new btSphereShape(small_sphere_size);
    btDefaultMotionState* anteriorSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(ant_side,ant_height,muscle_row_offset+small_sphere_size)));
    // now we
    btDefaultMotionState* centerSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(side,height,muscle_row_offset+5)));
    btDefaultMotionState* posteriorSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(post_side,post_height,muscle_row_offset+10-small_sphere_size)));
    btVector3 fallInertia(0, 0, 0);
    tSphere->calculateLocalInertia(paramSet->muscleCenterSphereMass, fallInertia);
    stSphere->calculateLocalInertia(paramSet->muscleOuterSphereMass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo anteriorSphereRigidBodyCI(paramSet->muscleOuterSphereMass, anteriorSphereMotionState, stSphere, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo centerSphereRigidBodyCI(paramSet->muscleCenterSphereMass, centerSphereMotionState, tSphere, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo posteriorSphereRigidBodyCI(paramSet->muscleOuterSphereMass, posteriorSphereMotionState, stSphere, fallInertia);
    btRigidBody* anteriorSphereRigidBody = new btRigidBody(anteriorSphereRigidBodyCI);
    btRigidBody* centerSphereRigidBody = new btRigidBody(centerSphereRigidBodyCI);
    btRigidBody* posteriorSphereRigidBody = new btRigidBody(posteriorSphereRigidBodyCI);
    anteriorSphereRigidBody->setAnisotropicFriction(paramSet->centerSphereAnisotropicFriction,btCollisionObject::CF_ANISOTROPIC_FRICTION);
    centerSphereRigidBody->setAnisotropicFriction(paramSet->centerSphereAnisotropicFriction,btCollisionObject::CF_ANISOTROPIC_FRICTION);
    posteriorSphereRigidBody->setAnisotropicFriction(paramSet->centerSphereAnisotropicFriction,btCollisionObject::CF_ANISOTROPIC_FRICTION);
    anteriorSphereRigidBody->setRestitution(paramSet->muscleRestitution); //glass marbles
    centerSphereRigidBody->setRestitution(paramSet->muscleRestitution); //glass marbles
    posteriorSphereRigidBody->setRestitution(paramSet->muscleRestitution); //glass marbles
    //anteriorSphereRigidBody->setFriction(paramSet->muscleFriction);// glass?
    //using anisotropic friction, not this friciton //centerSphereRigidBody->setFriction(paramSet->muscleFriction);// glass?
    //posteriorSphereRigidBody->setFriction(paramSet->muscleFriction);// glass?
    m_dynamicsWorld->addRigidBody(anteriorSphereRigidBody);
    m_dynamicsWorld->addRigidBody(centerSphereRigidBody);
    m_dynamicsWorld->addRigidBody(posteriorSphereRigidBody);
    muscles.push_back(anteriorSphereRigidBody);
    muscles.push_back(centerSphereRigidBody);
    muscles.push_back(posteriorSphereRigidBody);
    float z_shift = muscles[3*(i-dumbOffset)+1]->getWorldTransform().getOrigin().getZ() - muscles[3*(i-dumbOffset)]->getWorldTransform().getOrigin().getZ();
    z_shift /= 2.0f;
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
    
    btGeneric6DofSpring2Constraint* conOne = new btGeneric6DofSpring2Constraint(*muscles[3*(i-dumbOffset)], *muscles[3*(i-dumbOffset)+1], *tOne, *tTwo);
    btGeneric6DofSpring2Constraint* conTwo = new btGeneric6DofSpring2Constraint(*muscles[3*(i-dumbOffset)+1], *muscles[3*(i-dumbOffset)+2], *tOne2, *tTwo2);
    
    
   
    bool lastQuadrant = false;
    configureMuscleCon(conOne, lastQuadrant, paramSet);
    // we only want to lock the very end -- just the last 3 little balls
    if (muscleNum > 91){
        lastQuadrant = true;
    }
    configureMuscleCon(conTwo, lastQuadrant, paramSet);
    
    
    
    muscle_cons.push_back(conOne);
    muscle_cons.push_back(conTwo);
    
    m_dynamicsWorld->addConstraint(conOne);
    m_dynamicsWorld->addConstraint(conTwo);
}

void tieMusclesToCenterCylinder(btDynamicsWorld* m_dynamicsWorld, BodyParameterSet* paramSet){
    //muscle2cc_p2p_cons;
    //muscle2cc_spring_cons;
    int num_loops = muscles.size()/3.0f;
    int quadrant = 0;
    float cc_len = 1.25;
    int cc_offset = 2.5 / cc_len; // original cc's were 2.5
    int cc_index = -cc_offset;
    for(int i = 0; i < num_loops; i++){
        quadrant = i % 4;
        if (quadrant == 0){
            cc_index += cc_offset;
        }
        btTransform* cc_trans = new btTransform();
        btTransform* m_trans = new btTransform();
        cc_trans->setIdentity();
        m_trans->setIdentity();
        
        btVector3 cc_point = centerCylinders[cc_index]->getWorldTransform().getOrigin() + btVector3(0.0, 0.0, cc_len);
        btVector3 m_point = muscles[3*i+1]->getWorldTransform().getOrigin();
        btVector3 m_pivot = m_point - cc_point;
        btVector3 cc_pivot = cc_point - m_point;
        
        
        m_pivot = btVector3(0,0,0)+ cc_pivot;//+ m_pivot;
        cc_pivot = btVector3(0,0,cc_len);
        
        m_trans->setOrigin(m_pivot);
        cc_trans->setOrigin(cc_pivot);
        
        btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[3*i+1], *centerCylinders[cc_index], *m_trans, *cc_trans);
//        btPoint2PointConstraint* tempcon = new btPoint2PointConstraint(*muscles[3*i+1], *centerCylinders[cc_index], m_pivot, cc_pivot);
        
        configureCenterToMuscleCon(tempcon, paramSet);
        muscle2cc_spring_cons.push_back(tempcon);
//        muscle2cc_p2p_cons.push_back(tempcon);
        m_dynamicsWorld->addConstraint(tempcon);
    }
    num_loops = muscles.size()/3.0f;
    cc_index = -cc_offset;
    quadrant = 0;
    for(int i = 0; i < num_loops; i++){
        quadrant = i % 4;
        if( quadrant== 0){
            cc_index += cc_offset;
        }
        
        btTransform* cp1_trans = new btTransform();
        btTransform* cp2_trans = new btTransform();
        btTransform* mpfront_trans = new btTransform();
        btTransform* mpback_trans = new btTransform();
        cp1_trans->setIdentity();
        cp2_trans->setIdentity();
        mpfront_trans->setIdentity();
        mpback_trans->setIdentity();
        
        btVector3 cp1_front = centerCylinders[cc_index]->getWorldTransform().getOrigin() + btVector3(0.0, 0.0, -cc_len);
        btVector3 cp2_back = centerCylinders[cc_index+1]->getWorldTransform().getOrigin() + btVector3(0.0, 0.0, cc_len);
        btVector3 mp_front = muscles[3*i]->getWorldTransform().getOrigin();
        btVector3 mp_back = muscles[3*i+2]->getWorldTransform().getOrigin();
        // cp1_front -> mp_front, cp2_back -> mp_back
        
        btVector3 mp_front_pivot = cp1_front - mp_front;
        btVector3 mp_back_pivot = cp2_back - mp_back;
        cp1_trans->setOrigin(btVector3(0,0,-cc_len));
        cp2_trans->setOrigin(btVector3(0,0,cc_len));
        mpfront_trans->setOrigin(mp_front_pivot);
        mpback_trans->setOrigin(mp_back_pivot);
        
        
        btGeneric6DofSpring2Constraint* tempcon_front = new btGeneric6DofSpring2Constraint(*muscles[3*i], *centerCylinders[cc_index], *mpfront_trans, *cp1_trans);
        btGeneric6DofSpring2Constraint* tempcon_back = new btGeneric6DofSpring2Constraint(*muscles[3*i+2], *centerCylinders[cc_index+1], *mpback_trans, *cp2_trans);
        
        configureMuscleEndToCenterCylinderCon(tempcon_front, paramSet);
        configureMuscleEndToCenterCylinderCon(tempcon_back, paramSet);
        
        muscle2cc_spring_cons.push_back(tempcon_front);
        muscle2cc_spring_cons.push_back(tempcon_back);
        m_dynamicsWorld->addConstraint(tempcon_front);
        m_dynamicsWorld->addConstraint(tempcon_back);
    }
    
}
        
void createSupportsBetweenMuscles(btDynamicsWorld* m_dynamicsWorld, BodyParameterSet* paramSet){
    int num_loops = muscles.size()/3.0f-1;
    //int cc_index = 0;
    //bool vertical = true;
    int quadrant = 0;
    int first_index_offset = 0;
    int second_index_offset = 0;
    for(int i = 0; i < num_loops; i++){
        first_index_offset = 3*i;
        second_index_offset = 3*(i+1);
        quadrant = i % 4;
        if (quadrant == 3){
            second_index_offset = 3*(i-3); // need to connect muscle in fourth quadrant to muscle in first quadrant
        }
        btTransform* m1_trans = new btTransform();
        btTransform* m2_trans = new btTransform();
        m1_trans->setIdentity();
        m2_trans->setIdentity();
        
        btVector3 mp1 = muscles[first_index_offset+1]->getWorldTransform().getOrigin();
        btVector3 mp2 = muscles[second_index_offset+1]->getWorldTransform().getOrigin();
        btVector3 m1_pivot = (mp2 - mp1)/2.0f;
        btVector3 m2_pivot = (mp1 - mp2)/2.0f;
        
        m1_trans->setOrigin(m1_pivot);
        m2_trans->setOrigin(m2_pivot);
        
        
        btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[first_index_offset+1], *muscles[second_index_offset+1], *m1_trans, *m2_trans);
        
        configureCenterMuscleSupportCon(tempcon, quadrant, paramSet);
        muscle2muscle_spring_cons.push_back(tempcon);
        m_dynamicsWorld->addConstraint(tempcon);
    }
    if (num_loops >= 4){ // tie the first 4 'balls' together, but to almost form a point. Basically, don't add one the the index offsets
        for(int i = 0; i < 4; i++){
            first_index_offset = 3*i;
            second_index_offset = 3*(i+1);
            quadrant = i % 4;
            if (quadrant == 3){
                second_index_offset = 3*(i-3); // need to connect muscle in fourth quadrant to muscle in first quadrant
            }
            btTransform* m1_trans = new btTransform();
            btTransform* m2_trans = new btTransform();
            m1_trans->setIdentity();
            m2_trans->setIdentity();
            
            btVector3 mp1 = muscles[first_index_offset]->getWorldTransform().getOrigin();
            btVector3 mp2 = muscles[second_index_offset]->getWorldTransform().getOrigin();
            btVector3 m1_pivot = (mp2 - mp1)/2.0f;
            btVector3 m2_pivot = (mp1 - mp2)/2.0f;
            
            m1_trans->setOrigin(m1_pivot);
            m2_trans->setOrigin(m2_pivot);
            
            
            btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[first_index_offset], *muscles[second_index_offset], *m1_trans, *m2_trans);
            
            configureCenterMuscleSupportCon(tempcon, quadrant, paramSet);
            muscle2muscle_spring_cons.push_back(tempcon);
            m_dynamicsWorld->addConstraint(tempcon);
        }
        
    }
    if (num_loops >= 94){
        // tie the last 3 'balls' together, but to almost form a point. Basically, don't add one the the index offsets
        for(int i = 92; i < 95; i++){
            first_index_offset = 3*i+2;
            second_index_offset = 3*(i+1)+2;
            quadrant = i % 4;
            if (quadrant == 2){
                second_index_offset = 3*(i-3)+2; // need to connect muscle in third quadrant to muscle in first quadrant
            }
            btTransform* m1_trans = new btTransform();
            btTransform* m2_trans = new btTransform();
            m1_trans->setIdentity();
            m2_trans->setIdentity();
            
            btVector3 mp1 = muscles[first_index_offset]->getWorldTransform().getOrigin();
            btVector3 mp2 = muscles[second_index_offset]->getWorldTransform().getOrigin();
            btVector3 m1_pivot = (mp2 - mp1)/2.0f;
            btVector3 m2_pivot = (mp1 - mp2)/2.0f;
            
            m1_trans->setOrigin(m1_pivot);
            m2_trans->setOrigin(m2_pivot);
            
            
            btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[first_index_offset], *muscles[second_index_offset], *m1_trans, *m2_trans);
            
            configureCenterMuscleSupportCon(tempcon, quadrant, paramSet);
            muscle2muscle_spring_cons.push_back(tempcon);
            m_dynamicsWorld->addConstraint(tempcon);
        }
       
        // we also want to tie the last 4 balls of the previous muscle group together
        for(int i = 88; i < 94; i++){
            first_index_offset = 3*i+2;
            second_index_offset = 3*(i+1)+2;
            quadrant = i % 4;
            if (quadrant == 2){
                second_index_offset = 3*(i-3)+2; // need to connect muscle in third quadrant to muscle in first quadrant
            }
            btTransform* m1_trans = new btTransform();
            btTransform* m2_trans = new btTransform();
            m1_trans->setIdentity();
            m2_trans->setIdentity();
            
            btVector3 mp1 = muscles[first_index_offset]->getWorldTransform().getOrigin();
            btVector3 mp2 = muscles[second_index_offset]->getWorldTransform().getOrigin();
            btVector3 m1_pivot = (mp2 - mp1)/2.0f;
            btVector3 m2_pivot = (mp1 - mp2)/2.0f;
            
            m1_trans->setOrigin(m1_pivot);
            m2_trans->setOrigin(m2_pivot);
            
            
     
            btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[first_index_offset], *muscles[second_index_offset], *m1_trans, *m2_trans);
            
            configureCenterMuscleSupportCon(tempcon, quadrant, paramSet);
            muscle2muscle_spring_cons.push_back(tempcon);
            m_dynamicsWorld->addConstraint(tempcon);
        }
        
    }
    
   // now tie ends of muscles to adjacent ceneter spheres
    
    /* 3*i <=> 3*(i-4) + 1 --> start of 4th muscle to center of 0th muscle
     * 3*i+2 <=> 3*(i+4)+1 --> end of 4th muscle to center of 8th muscle
     *
     *
     *
     * 3*i + 2 <=> 3*(i+8) --> end of 0th muscle, to start of 8th muscle, and so on.
     */
    num_loops = muscles.size()/3.0f-(1+4); // 1 because we don't have a full last quadrant of muscles, 4 because that's our offset
    int first_index, second_index;
    for(int i = 0; i < num_loops; i++){
        if (i >= 4){
            first_index = 3*i;
            second_index = 3*(i-4) + 1;
            btTransform* m_front = new btTransform();
            btTransform* m_back = new btTransform();
            m_front->setIdentity();
            m_back->setIdentity();
            
            
            btVector3 mp1 = muscles[first_index]->getWorldTransform().getOrigin();
            btVector3 mp2 = muscles[second_index]->getWorldTransform().getOrigin();
            btVector3 mf_pivot = (mp2 - mp1)/2.0f;
            btVector3 mb_pivot = (mp1 - mp2)/2.0f;
                
            m_front->setOrigin(mf_pivot);
            m_back->setOrigin(mb_pivot);
            
            btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[first_index], *muscles[second_index], *m_front, *m_back);
            
            configureEndsOfMusclesSupportCon(tempcon, paramSet);
            muscleEnds2muscleEnds_spring_cons.push_back(tempcon);
            m_dynamicsWorld->addConstraint(tempcon);
        }
        
        first_index = 3*i+2;
        second_index = 3*(i+4) + 1;
        btTransform* m_front = new btTransform();
        btTransform* m_back = new btTransform();
        m_front->setIdentity();
        m_back->setIdentity();
        
        
        btVector3 mp1 = muscles[first_index]->getWorldTransform().getOrigin();
        btVector3 mp2 = muscles[second_index]->getWorldTransform().getOrigin();
        btVector3 mf_pivot = (mp2 - mp1)/2.0f;
        btVector3 mb_pivot = (mp1 - mp2)/2.0f;
            
        m_front->setOrigin(mf_pivot);
        m_back->setOrigin(mb_pivot);
        
        btGeneric6DofSpring2Constraint* tempcon = new btGeneric6DofSpring2Constraint(*muscles[first_index], *muscles[second_index], *m_front, *m_back);
        
        configureEndsOfMusclesSupportCon(tempcon, paramSet);
        muscleEnds2muscleEnds_spring_cons.push_back(tempcon);
        m_dynamicsWorld->addConstraint(tempcon);
    }
 
    
}

btRigidBody* createCenterCylinder(btCollisionShape* colShape, btDynamicsWorld* m_dynamicsWorld, int cylNum, float cylLen){
    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();
    
    startTransform.setOrigin(btVector3(2.0, 4,cylLen+(cylNum*cylLen*2)));
    
    
    btVector3 localInertia(0,0,0);
    
    colShape->calculateLocalInertia(paramSet->centerCylinderMass,localInertia);
    
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo cInfo(paramSet->centerCylinderMass, myMotionState, colShape, localInertia);
    
    btRigidBody* body = new btRigidBody(cInfo);
    m_dynamicsWorld->addRigidBody(body);
    return body;
}


void buildCenterCylinderChain(btDynamicsWorld* m_dynamicsWorld, btAlignedObjectArray<btCollisionShape*>* m_collisionShapes, BodyParameterSet* paramSet){
    
    int num_cc = paramSet->num_cc;
    float cylinderLen = paramSet->cylinderLen;
    btCylinderShapeZ* colShape = new btCylinderShapeZ(btVector3(.25,.25,cylinderLen));
    
    
    //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    //(*m_collisionShapes).push_back(colShape);
    
   
    
    
    /*
    startTransform.setOrigin(btVector3(2.0, 4,7.5));
    btRigidBody* rb2 = createRigidBody(1,startTransform, colShape);
    
    startTransform.setOrigin(btVector3(2.0, 4,12.5));
    btRigidBody* rb3 = createRigidBody(1,startTransform, colShape);
     */
    for (int i = 0; i < num_cc; i++){
        centerCylinders.push_back(createCenterCylinder(colShape, m_dynamicsWorld,i, cylinderLen));
    }
    
    float cylinderLenTimesTwo = cylinderLen*2;
    for (int i = 0; i < num_cc-1; i++){
        //btVector3 pivot = btVector3(0,0,2.5+(i*cylinderLenTimesTwo));
        //btVector3 pivot2 = btVector3(0,0,2.5+((i+1)*cylinderLenTimesTwo));
        
        btVector3 pivotP2P = btVector3(0,0,cylinderLen);
        btVector3 pivotP2P2 = btVector3(0,0,-cylinderLen);
        
        btTransform* tOne = new btTransform();
        btTransform* tTwo = new btTransform();
        
        tOne->setIdentity();
        tTwo->setIdentity();
        
        tOne->setOrigin(pivotP2P);
        tTwo->setOrigin(pivotP2P2);
        
        btRigidBody* rb1 = centerCylinders[i];
        btRigidBody* rb2 = centerCylinders[i+1];
    
        //btPoint2PointConstraint* cons = new btPoint2PointConstraint(*rb1, *rb2, pivotP2P, pivotP2P2);
        //btPoint2PointConstraint* cons2= new btPoint2PointConstraint(*rb2, *rb3, pivotP2P3, pivotP2P4);
        
        //cons2->setParam(BT_CONSTRAINT_CFM, 0);
        //cons2->setParam(BT_CONSTRAINT_ERP, .8);
    
        btGeneric6DofSpring2Constraint* cons = new btGeneric6DofSpring2Constraint(*rb1, *rb2, *tOne, *tTwo);
        
        configureCenterCylinderChainCon(cons, paramSet);
    
        //        m_dynamicsWorld->addConstraint(cons, true);
        cc_cons.push_back(cons);
        //cc_cons.push_back(cons2);
        m_dynamicsWorld->addConstraint(cons, true);
        //m_dynamicsWorld->addConstraint(cons2, true);
    }

}

btRigidBody*	createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape,  const btVector4& color = btVector4(1, 0, 0, 1)){
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));
    
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);
    
    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);


    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

    btRigidBody* body = new btRigidBody(cInfo);

    body->setUserIndex(-1);
    return body;
}

void init() // Called before main loop to set up the program
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}



// Called every time a window is resized to resize the projection matrix
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    /* below code thanks to:
     *
     * https://sites.google.com/site/gmitresearch/mt4j/mt4j-jogl/modelview-and-projection-matrices
     */
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();
    gluPerspective(45,1000.0f/800.0f, 1, 500);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(100.0, 100.0, 150.0, 0.0, 0.0, 60.0, 0.0f, 1.0f, 0.0f);
}


void grabCurrentLocation(){
    int num_sections = paramSet->num_sections - 1; // last section isn't full, we can probably safely ignore it.
    int start_point = 0; // just so we don't have a zero plopped in below.
    int mid_point = num_sections/2; // we don't really care exactly where it is, just somewhere around the middle.
    int end_point = num_sections-1; // 0 indexing.
    // ------------ start
    int muscle_index = start_point * 4;
    // right bottom muscle (2 -- if counting from 0)
    positionHistory[posCheckNum][0] = (muscles[((muscle_index+2)*3)+1]->getWorldTransform().getOrigin());
    // left bottom muscle (3 -- if counting from 0)
    positionHistory[posCheckNum][1] = (muscles[((muscle_index+3)*3)+1]->getWorldTransform().getOrigin());
    // ------------- middle
    muscle_index = mid_point * 4;
    positionHistory[posCheckNum][2] = (muscles[((muscle_index+2)*3)+1]->getWorldTransform().getOrigin()); // right
    positionHistory[posCheckNum][3] = (muscles[((muscle_index+3)*3)+1]->getWorldTransform().getOrigin()); // left
    // ------------- end
    muscle_index = end_point * 4;
    positionHistory[posCheckNum][4] = (muscles[((muscle_index+2)*3)+1]->getWorldTransform().getOrigin()); // right
    positionHistory[posCheckNum][5] = (muscles[((muscle_index+3)*3)+1]->getWorldTransform().getOrigin()); // left
    for (int i = 0; i < 6; i++){
        if (fabs(positionHistory[posCheckNum][i][0]) >= 120){
            //printf("sidewinder... ");
            instantDeath = true;
            break;
        }
        else if(fabs(positionHistory[posCheckNum][i][1]) >= 10){
            //printf("highflier... ");
            instantDeath = true;
            break;
        }
    }
    posCheckNum++;
    return;
}

/* averages two points */
btVector3 avgPts(btVector3& a, btVector3& b){
    return ((a+b)/2.f);
}

/* squares a */
float square(float a){
    return a*a;
}

/* x-z euclidean distance between points a and b 
 
 y is ignored.... for now.
*/
float eucDist(btVector3& a, btVector3& b){
    return sqrtf(square(b[0]-a[0])+square(b[2]-a[2]));
}


/*
 minimial path length from each pair's (front, middle, and back) starting point, to (0,0,100) -- or -100
 */
float computeUtilityIndex(){
    btVector3 startF, endF, startM, endM, startB, endB;
    startF = avgPts(positionHistory[0][0], positionHistory[0][1]);
    //startM = avgPts(positionHistory[0][2], positionHistory[0][3]);
    //startB = avgPts(positionHistory[0][4], positionHistory[0][5]);
    endF = avgPts(positionHistory[posCheckNum-1][0], positionHistory[posCheckNum-1][1]); // posCheckNum is incremented each time we add something
    //endM = avgPts(positionHistory[ph_size-1][2], positionHistory[ph_size-1][2]);
    //endB = avgPts(positionHistory[ph_size-1][4], positionHistory[ph_size-1][5]);
    btVector3 point(0,0,-1000);
    float diffF = eucDist(point,endF);
    if (endF[2] >= startF[2]){
        //printf("no forward movement... ");
        instantDeath = true;
        return -1.f;
    }
    //float diffM = endM[2] - startM[2];
    //float diffB = endB[2] - startB[2];
    //float utility = -1*(diffF + diffM + diffB);
    float utility = diffF;
    return utility;
}

//void resetBullet(btDynamicsWorld* dynamicsWorld){
void resetBullet(){
    

    auto num_muscle_cons = muscle_cons.size();
    for (int i = 0; i < num_muscle_cons; i++){
        dynamicsWorld->removeConstraint(muscle_cons[i]);
        delete muscle_cons[i];
    }
    auto num_cc_cons = cc_cons.size();
    for (int i = 0; i < num_cc_cons; i++){
        dynamicsWorld->removeConstraint(cc_cons[i]);
        delete cc_cons[i];
    }
    auto num_muscle2cc_cons = muscle2cc_spring_cons.size();
    for (int i = 0; i < num_muscle2cc_cons; i++){
        dynamicsWorld->removeConstraint(muscle2cc_spring_cons[i]);
        delete muscle2cc_spring_cons[i];
    }
    auto num_muscle2muscle_cons = muscle2muscle_spring_cons.size();
    for (int i = 0; i < num_muscle2muscle_cons; i++){
        dynamicsWorld->removeConstraint(muscle2muscle_spring_cons[i]);
        delete muscle2muscle_spring_cons[i];
    }
    auto num_muscleEnds2muscleEnds_cons = muscleEnds2muscleEnds_spring_cons.size();
    for (int i = 0; i < num_muscleEnds2muscleEnds_cons; i++){
        dynamicsWorld->removeConstraint(muscleEnds2muscleEnds_spring_cons[i]);
        delete muscleEnds2muscleEnds_spring_cons[i];
    }
    auto num_muscles = muscles.size();
    for (int i = 0; i < num_muscles; i++){
        dynamicsWorld->removeRigidBody(muscles[i]);
        delete muscles[i];
    }
    auto num_cc = centerCylinders.size();
    for (int i = 0; i < num_cc; i++){
        dynamicsWorld->removeRigidBody(centerCylinders[i]);
        delete centerCylinders[i];
    }
    

    muscles.clear();
    centerCylinders.clear();
    muscle_cons.clear();
    cc_cons.clear();
    muscle2cc_spring_cons.clear();
    muscle2muscle_spring_cons.clear();
    muscleEnds2muscleEnds_spring_cons.clear();
    simStepCount = 0;
    windowCounter = 0;
    posCheckNum = 0;
}

//void makeWorm(btDynamicsWorld* dynamicsWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, BodyParameterSet* paramSet){
void makeWorm(){

    buildCenterCylinderChain(dynamicsWorld, &collisionShapes, paramSet);
   
    
   // muscle
    for (int i = 0; i < paramSet->num_muscles; i++){
      buildMuscle(dynamicsWorld, paramSet, i, 0);
    }
    
    
    // tie muscles to center cylinder
   tieMusclesToCenterCylinder(dynamicsWorld, paramSet);
    
    // add supports between muscles
    createSupportsBetweenMuscles(dynamicsWorld, paramSet);
    
    grabCurrentLocation();
}

bool doInstantDeath(){
    if(instantDeath){
        instantDeath = false;
        instantDeathCount++;
        log_.warn("(executable "+std::to_string(ESN_GLOB)+") evolution round "+std::to_string(evolution_round)+" - instant death ("+std::to_string(instantDeathCount)+")");
        if(ESN_GLOB == 0 || ESN_GLOB == 1)
            printf("(executable %d) evolution round %d - instant death (%d)\n",paramSet->executableSessionNumber,evolution_round,instantDeathCount);
        return true;
    }
    return false;
}

bool doSimStep(){
    
    // Hit on instant death being true only (set it grabCurrentLocation())
    if (doInstantDeath()){
        return false;
    }
    // Hit when evolving and we've hit the max steps (paramSet->num_windows)
    // Will grab the currentLocation, compute utility (stored in paramSet)
    // Then it will save the configuration.
    // Probably want to add something here to save it for my use w/ genetic algorithm
    //printf("simstepcount: %d (max,%d) -- ", simStepCount, mSteps);
    if (paramSet->evolve && (simStepCount >= getMaxSimSteps())){
        //printf("true\n");
        grabCurrentLocation();
        paramSet->currentUtilityIndex = computeUtilityIndex();
        if (!doInstantDeath()){// computeUtilityIndex() sets instantDeath to true if worm's head ended up moving backwards.
            if(!paramSet->reproduce)
                paramSet->saveConfiguration();
            log_.info("(executable "+std::to_string(ESN_GLOB)+") evolution round "+std::to_string(evolution_round)+", utility: "+std::to_string(paramSet->currentUtilityIndex));
            if(ESN_GLOB == 0 || ESN_GLOB == 1)
                printf("(executable %d) evolution round %d, utility: %.3f\n",paramSet->executableSessionNumber,evolution_round, paramSet->currentUtilityIndex);
        }
        return false;
    }
    //printf("false\n");
    if(simStepCount >= getMaxSimSteps()){
        if (paramSet->saveDefaultConfigAndUtility){// this bit of code is just for a 'first run' sort of situation (or the saved parameters are deleted -- so we know how everything compares to our default)
            grabCurrentLocation();
            paramSet->currentUtilityIndex = computeUtilityIndex();
            paramSet->saveConfiguration();
            paramSet->saveDefaultConfigAndUtility = false;
        }
        //btVector3 curAvgLoc = getAverageLocationOfBottomCenterSpheres();
        //paramSet->currentUtilityIndex += computeUtilityIndex(prevAvgLoc,curAvgLoc);
        //resetBullet(dynamicsWorld);
        //makeWorm(dynamicsWorld, collisionShapes, paramSet);
        resetBullet();
        paramSet->loadParams();
        signalBuilder.wreckSignal();
        signalBuilder = SignalBuilder(paramSet);
        signalBuilder.buildSignal();
        makeWorm();
        //printf("replaying...");
        return false;
    }
    else if(windowCounter >= windowsBetweenPositionChecks){
        grabCurrentLocation();
        windowCounter = 0;
    }
    dynamicsWorld->stepSimulation(1 / 120.f,0);
    simStepCount++;
    return true;
    
}


// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{
    
    //for (int i = 0; i < 5000; i++){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(100, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 100, 0);
    glColor3f(0, 0, 1);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 100);
    glEnd();
    
    doSimStep();
    
    //std::cout<< "pee" << std::endl;
    dynamicsWorld->debugDrawWorld();
    
    
    
    glPopMatrix();
    
    
    glutSwapBuffers();
    
    
}


void initPositionHistoryVector(){
   mSteps = getMaxSimSteps();// /windowsBetweenPositionChecks + 3;// +2 because int division rounds down, and we'll need one more for the last call. the last slot may be empty sometimes.
    int intervals = ceilf((float)mSteps/windowsBetweenPositionChecks)+2;
    if(ESN_GLOB == 1 || ESN_GLOB == 0)
        printf("windows, steps, windows between pos checks, intervals: %d, %d, %d, %d\n",mSteps/4, mSteps, windowsBetweenPositionChecks, intervals);
    positionHistory.clear();
    for (int i = 0; i < intervals; ++i){
        positionHistory.push_back(std::vector<btVector3>());
        for(int j = 0; j < 6; j++){
            positionHistory[i].push_back(btVector3(0,0,0));
        }
    }
}

void out_of_memory_handler(){
    log_.fatal("process: "+std::to_string(ESN_GLOB)+": Out of memory handler called.");
    // make sure buffer is flushed, only an issue if the log message gets dumped on the thread pool
    // and fatal messages should get dumped onto the single-threader writer by default
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::set_new_handler(nullptr); // Will throw std::bad_alloc now
}

//int fmain(int argc, char** argv){
void gitErDone(int* argc, char** argv){
    
    /*
    if (argc != 2){
        printf("please enter an exectuable number (integer between 0 and 100).\n");
        exit(0);
    }
    int exec_num = atoi(argv[1]);
    if(exec_num < 0 || exec_num > 100){
        printf("Does '%s' look like an integer between 0 and 100 to you?\n",argv[1]);
        exit(0);
    }
     */
    
    std::set_new_handler(out_of_memory_handler);
    log_.info("out_of_memory_handler registered", FUNC);

    /* Set the new handler function */

    
    // NOTE: if graphicsOn == true, useDefault OR useBest can be true... but evolve MUST be false.
    // (we can't 'safely' exit the glut main loop, so, if we have graphics, we don't evolve.)
    
    // NOTE 2: "NOTE" not valid anymore. but it might be in the future. but the code won't run if you
    // ignore it... so, it has a certain vaility. The point is, the code could run. but it doesn't right now...
    
    /*
     NOTE 3:
        - by default (with evolve and UseBest both false), default parameter set will run, and it will loop.
        - with evolve set to true, it will evolve 10 times. quit when you like, only current run will* be lost.
        - with useBest true (and evolve false, if evolve is true, it takes precedence), the best saved configuration 
        will be loaded, and will loop.
     
        => both evolution and 'playback' run for 2000 windows.
        => graphicsOn should be false if evolve is true
     
        *should
    */
    
    // NOTE 4: to run a different best configuration, change BodyParameterSet::useBest_n from 0 (the best) to whatever you want to run.
    num_quads = paramSet->num_sections;
    bool EVOLVE = false;
    bool graphicsOn = !EVOLVE;
    bool REPRODUCE = EVOLVE;
    bool MAKE_WORMS = REPRODUCE;
    paramSet->executableSessionNumber = 0;//exec_num
    std::string eSN = std::to_string(paramSet->executableSessionNumber);
    ESN_GLOB = paramSet->executableSessionNumber;

    paramSet->useBest_n = 1;
    if(EVOLVE){
        paramSet->paramFile ="worm_body_params_" + eSN +".txt";// would be .csv, but want mac to preview as text file
        paramSet->paramMetaData = "worm_body_params_meta_" + eSN + ".txt"; // would be .dat
    }
    else{
        paramSet->paramFile ="merged_wbp.txt";// would be .csv, but want mac to preview as text file
        paramSet->paramMetaData = "merged_wbp_meta.txt"; // would be .dat
    }
    
    // ------ Genetic changes ------
    
    // On initial run, load the generation and worm number from file, from here on out all these
    // variables will be handled by param class and Darwin
    if(REPRODUCE){
        paramSet->generationNumber = GeneticUtils::findGenerationNumber(eSN);
        paramSet->currentWormNumber = GeneticUtils::findWormNumber(eSN);
        paramSet->evolveFile = GeneticUtils::getParamsFilePath(eSN);
        paramSet->evolveMetaFile = GeneticUtils::getMetaFilePath(eSN);
        paramSet->reproduce = REPRODUCE;
        paramSet->numWormsInGeneration = 100; // ....uh what...sean? (sean wrote this)
        
        /*
        std::cout << "gen#: " << paramSet->generationNumber << '\n';
        std::cout << "cur#: " << paramSet->currentWormNumber << '\n';
        std::cout << "evolveFile: " << paramSet->evolveFile << '\n';
        std::cout << "evolveMeta: " << paramSet->evolveMetaFile << '\n';
        std::cout << "reproduce: " << paramSet->reproduce << '\n';
        std::cout << "numWorms: " << paramSet->numWormsInGeneration << std::endl;
        exit(-1);
        */
    }
    
    // ------ END ------
    

    paramSet->evolve = EVOLVE; // highest precedence
    paramSet->useBest = !EVOLVE; // middle precedence
  
    int numEvolutions = 100000;
    ////////////////////////////////////////////BodyParameterSet* paramSet = new BodyParameterSet();
    paramSet->loadParams();
    //printf("Parameters set, generating wave.\n");
    signalBuilder = SignalBuilder(paramSet);
    signalBuilder.buildSignal();
    initPositionHistoryVector();
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    
    dynamicsWorld->setGravity(btVector3(0, paramSet->gravity, 0));
    

    /*
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe+btIDebugDraw::DBG_DrawContactPoints);
    */
    

    
    
   //dynamicsWorld->setInternalTickCallback(bEPreTickCallback);
    dynamicsWorld->setInternalTickCallback(newPTCB);
    
	///create a few basic rigid bodies
	btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(500.),btScalar(50.),btScalar(500.)));
    
    ////////////////////////////btAlignedObjectArray<btCollisionShape*> collisionShapes;
    //m_dynamicsWorld->getSolverInfo().m_numIterations = 30;
	//groundShape->initializePolyhedralFeatures();
//	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);
	
	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

	btScalar mass(0.);
    btRigidBody* groundRigidBody = createRigidBody(mass,groundTransform,groundShape, btVector4(0,0,1,1));
    groundRigidBody->setRestitution(paramSet->groundRestitution);
    groundRigidBody->setFriction(paramSet->groundFriction);
    dynamicsWorld->addRigidBody(groundRigidBody);

//    makeWorm(dynamicsWorld,collisionShapes, paramSet);
    makeWorm();
    
    //btVector3 startAvgLocation = getAverageLocationOfBottomCenterSpheres();
    
    if (graphicsOn && !paramSet->evolve){
        debugDrawer = new myDebug();
        dynamicsWorld->setDebugDrawer(debugDrawer);
        
        glutInit(argc, argv); // Initializes glut
        
        // Sets up a double buffer with RGBA components and a depth component
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
        
        // Sets the window size to 512*512 square pixels
        glutInitWindowSize(512, 512);
        
        // Sets the window position to the upper left
        glutInitWindowPosition(0, 0);
        
        // Creates a window using internal glut functionality
        glutCreateWindow("Hello!");
        
        // passes reshape and display functions to the OpenGL machine for callback
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);
        glutIdleFunc(display);
        
        init();
        
        // Starts the program.
        glutMainLoop();
    }
    else if (graphicsOn && paramSet->evolve){
        printf("Evolution with graphics is not supported at this time. Please implement this feature yourself if you are so damn interested in it! (If you aren't, and this was a mistake, we apologize profusely for our presumtion.)\n");
    }
    else if (!graphicsOn && paramSet->evolve){
        int i = 0;
        if(!REPRODUCE) printf("Starting evolution...\n");
        while (i < numEvolutions){
            
            // We need to create a new generation before we run the utility functions
            if(paramSet->reproduce && MAKE_WORMS){
                std::cout << "Running Darwin...\n";
                DarwinEffectuator de;
                // Update the generation tracking file so darwin knows what worms to work on
                GeneticUtils::updateGenerationNumber(paramSet->executableSessionNumber,
                                                     paramSet->generationNumber);
                
                // Entry to genetic algorithm, code found in DarwinEffectuator
                de.survivalOfTheFittest(paramSet->numRandomWormsGeneratedInCurrentGeneration,
                                        paramSet->numWormsInGeneration,
                                        paramSet->executableSessionNumber);
                
                // Reset the control values
                paramSet->currentWormNumber = 0;
                GeneticUtils::updateWormNumber(paramSet->executableSessionNumber,
                                               paramSet->currentWormNumber);
                paramSet->generationNumber++;
                GeneticUtils::updateGenerationNumber(paramSet->executableSessionNumber,
                                                     paramSet->generationNumber);
                
                // A new generation has been created, time to load the worms and get some values
                MAKE_WORMS = false;
                
                paramSet->loadParams();
                //printf("Parameters set, generating wave.\n");
                signalBuilder = SignalBuilder(paramSet);
                signalBuilder.buildSignal();
                initPositionHistoryVector();
                resetBullet();
                makeWorm();
            }
            
            while(doSimStep());
            evolution_round++;
            
            //resetBullet(dynamicsWorld);
            //makeWorm(dynamicsWorld, collisionShapes, paramSet);
            
            // Please note, if REPRODUCE is true the loadParams call just delegates to loadEvolvedWorm
            paramSet->currentWormNumber++;
            GeneticUtils::updateWormNumber(paramSet->executableSessionNumber, paramSet->currentWormNumber);
            if(paramSet->currentWormNumber >= paramSet->numWormsInGeneration)
                MAKE_WORMS = true;
            
            paramSet->loadParams();
            signalBuilder.wreckSignal();
            signalBuilder = SignalBuilder(paramSet);
            signalBuilder.buildSignal();
            initPositionHistoryVector();
            resetBullet();
            makeWorm();
            i++;
        }
    }

//	m_guiHelper->autogenerateGraphicsObjects(dynamicsWorld);
    //return 0;
}

/*
void BasicExample::renderScene()
{
	CommonRigidBodyBase::renderScene();
	
}
 */







/*
CommonExampleInterface*    BasicExampleCreateFunc(CommonExampleOptions& options)
{
	return new BasicExample(options.m_guiHelper);
}

 */



/* good ones *(before 4/22/16):
     * file 4: 3, 7*, 9, 10, 13,
     * file 3: 3 (stays straight, doesn't move much), 5 (pfw), 7,
     * file 2: 1**, 2, 3, 8** (straighter for longer), 10 (interesting)
     * file 1: 0*** (perhaps most wormlike, but doesn't stay straight), 1 (alright), 3 (does C. elegans head swinging), 5 (fairly wormy),
     *
     * thoughts/ideas/observations:
     * - most worms spin... maybe friction is an issue, but maybe its the direction and magnitude of the wave.
            = maybe the wave should be scaled from out to in to out (small - large - small)
            = could evolve the wave among a few different bodies
     * - for the worms that moved, as the utility went up, the *type* of motion generally stayed the same, but the speed decreased.
     * - should run some sort of clustering to determine good ranges, then can do evolution
     *      = (or, don't evolve bodies now, but evolve wave, and assume body is alright)
     *          - probably a bad idea.
     * - maybe should increase required phase offset for evolution
     *      = maybe should set wave frequency, and evolve worm, but not wave???
     *          - this would encourage it to look like c. elegans when moving, which might force other parameters to be more suitable.
     * - friction much better than it was
     */
