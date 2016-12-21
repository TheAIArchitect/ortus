//
//  Geometrician.cpp
//  LearningOpenGL
//
//  Created by onyx on 1/25/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#include "Geometrician.hpp"

/**
 * The geometry doctor.
 */
Geometrician::Geometrician(){
    float increment = 1.0f/(num_t-1);
    t = new float[num_t];
    t[0] = 0.0f;
    for (int i = 1; i < num_t; i++){
        t[i] = t[i-1] + increment;
    }
    for (int i = 0; i < num_bezier_curves; i++){
        for (int j = 0; j < pts_per_bezier_curve; j++){
            spine[i][j] = WRL_UPSCALE*spine[i][j];
        }
    }
}

void Geometrician::straighten(MassPoint** massPoints, int num_points, MassPoint*** bezPts, int* numBezPts){
    
    /*
    std::vector<glm::vec3> toFix;
    for (int i = 0; i < num_points; i++){
        toFix.push_back(massPoints[i]->center);
    }
     */
    generatePointSet();
    *bezPts = new MassPoint*[pointSet.size()];
    std::vector<float> zDistances = getZDistances();
    std::vector<float> oldYs;
    *numBezPts = pointSet.size();
    for (int i = 0; i < pointSet.size(); i++){
        //pointSet[i].x = pointSet[i].x;
        //oldYs.push_back(pointSet[i].y);
        //pointSet[i].y = avg_y;
        //pointSet[i].z = pointSet[i].z;
        //pointSet[i].z = zDistances[i];
        //pointSet[i].z += 0.000354269 ;
        
        //printf("(%f, %f, %.9f)\n",pointSet[i].x, pointSet[i].y,pointSet[i].z);
        (*bezPts)[i] = new MassPoint(glm::vec3(pointSet[i]));
        
    }
    
    
    // adjust z coords so that they start at 0
    //for (int j = 0; j < num_points; j++) {
    //    massPoints[j]->center.z = massPoints[j]->center.z - most_neg_z;
        //printf("%.9f\n",massPoints[j]->center.z-most_neg_z);
    //}
    int left = 0;
    int right = 0;
    std::vector<glm::vec3> oldies;
    bool straighten = true;
    for (int j = 0; j < num_points; j++) {
        glm::vec3 oldp = massPoints[j]->center;
        oldies.push_back(oldp);
        get_nearest_point_pair(left,right, oldp);
        glm::vec3 match = pointSet[left];
        float y_new = (oldp.y - match.y) + avg_y;// - oldp.y;
        y_new = oldp.y - match.y;
        if (straighten){
            massPoints[j]->center.y = y_new;
            massPoints[j]->center.z = zDistances[left];
        }
        //printf("(%d) match, MP, new: %f, %f, %f -- %f, %f, %f -- %f, %f, %f\n",left, match.x, match.y, match.z, oldp.x, oldp.y, oldp.z, massPoints[j]->center.x, massPoints[j]->center.y, massPoints[j]->center.z);
        printf("(%d) match: (%f, %f, %f) -- coord: (%f, %f, %f)\n",left, match.x, match.y, match.z, massPoints[j]->center.x, massPoints[j]->center.y, massPoints[j]->center.z);
        
       
        
    }
    // adjust worm so it starts at 0 on z axis.
    float smallest_z = 100000.0f;
    for (int j = 0; j < num_points; j++) {
        if (massPoints[j]->center.z < smallest_z){
            smallest_z = massPoints[j]->center.z;
        }
    }
    for (int j = 0; j < num_points; j++) {
        massPoints[j]->center.z -= smallest_z;
    }
    
    //for (int i = 0; i < num_points; i++){
    //  printf("(%d) before: (%f, %f, %f) --- after: (%f, %f, %f)\n",i,oldies[i].x,oldies[i].y,oldies[i].z,massPoints[i]->center.x,massPoints[i]->center.y,massPoints[i]->center.z);
    //}
    
    
}


void Geometrician::get_nearest_point_pair(int& left, int& right, glm::vec3 coord){
   
    if (!distPairs.empty()){
        distPairs.clear();
    }
    for (int i = 0; i < pointSet.size(); i++){
        DistPair dp;
        // first is distance
        dp.first = std::sqrt(std::pow(pointSet[i][0]-coord[0],2)+std::pow(pointSet[i][1]-coord[1],2)+std::pow(pointSet[i][2]-coord[2],2));
        //dp.first = std::sqrt(std::pow(pointSet[i][1]-coord[1],2)+std::pow(pointSet[i][2]-coord[2],2));
        //dp.first = std::sqrt(std::pow(pointSet[i][2]-coord[2],2));
        // second is index
        dp.second = i;
        distPairs.push_back(dp);
    }
    std::sort(std::begin(distPairs), std::end(distPairs), [](DistPair& first, DistPair& second)->bool { return first.first < second.first; });
    // they said to return two nearest points in pointset, leftmost first....
    if (distPairs[0].second > distPairs[1].second){
        left = distPairs[1].second;
        right = distPairs[0].second;
    }
    else{
        left = distPairs[0].second;
        right = distPairs[1].second;
    }
}
    
void Geometrician::generatePointSet(){
    glm::vec3* cPt; // control point
    
    for (int i = 0; i < num_bezier_curves; i++){
        cPt = spine[i];
        if (i == (num_bezier_curves-1)){
            bezierPointSet(cPt, true);
        }
        else{
            bezierPointSet(cPt, false);
        }
    }
    float avg_y = 0;
    for (int i = 0; i < pointSet.size(); i++){
        //printf("%f, %f, %f\n",pointSet[i][0], pointSet[i][1], pointSet[i][2]);
        avg_y += pointSet[i][1];
    }
    this->avg_y = avg_y/((float)pointSet.size());
}
    
void Geometrician::bezierPointSet(glm::vec3* p, bool isLast){
    glm::vec4 p_x = glm::vec4(p[0][1], p[1][1], p[2][1], p[3][1]);
    glm::vec4 p_y = glm::vec4(p[0][2], p[1][2], p[2][2], p[3][2]);
    std::vector<glm::vec2> new_xy = bezPoly(p_x, p_y);
    for (int i = 0; i < new_xy.size(); i++){
        if (!isLast && i == (new_xy.size() - 1)){
            break;
        }
        pointSet.push_back(glm::vec3(0.0f, new_xy[i][1], new_xy[i][0]));
    }
}

std::vector<glm::vec2> Geometrician::bezPoly(glm::vec4 p_x, glm::vec4 p_y){
        std::vector<glm::vec2> new_xy;
        for (int i = 0; i < num_t; i++){
            float temp_x = (std::pow(1-t[i],3)*p_x[0]) + (3*(std::pow(1-t[i],2)*t[i]*p_x[1])) + (3*(1-t[i])*std::pow(t[i],2)*p_x[2]) + (std::pow(t[i],3)*p_x[3]);
            float temp_y = (std::pow(1-t[i],3)*p_y[0]) + (3*(std::pow(1-t[i],2)*t[i]*p_y[1])) + (3*(1-t[i])*std::pow(t[i],2)*p_y[2]) + (std::pow(t[i],3)*p_y[3]);
            new_xy.push_back(glm::vec2(temp_x, temp_y));
        }   
        return new_xy;
    }

/**
 * NOTE: must be run AFTER filling pointSet vector.
 * 
 * uses the y and z coordinates to compute the Z coordinates of the points in the pointset once 'flattened'
 */
std::vector<float> Geometrician::getZDistances(){
    std::vector<float> distances;
    for (int i = 0; i < (pointSet.size()-1); i++){
        distances.push_back(std::sqrt(std::pow((pointSet[i+1][1]-pointSet[i][1]),2)+std::pow((pointSet[i+1][2]-pointSet[i][2]),2)));
    }
    std::vector<float> totals;
    float total = 0;
    totals.push_back(0.0f);
    for (int i = 0; i < distances.size(); i++){
        total += distances[i];
        totals.push_back(total);
    }
    // totals has the *actual* Z location of points 0, 1, 2, .., n
    return totals;
    
}





