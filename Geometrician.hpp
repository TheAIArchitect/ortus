//
//  Geometrician.hpp
//  LearningOpenGL
//
//  Created by onyx on 1/25/16.
//  Copyright © 2016 AweM. All rights reserved.
//

#ifndef Geometrician_hpp
#define Geometrician_hpp

#include <stdio.h>
#include "MassPoint.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <cmath>

/**
 * Ported from:
 *
 * https://github.com/openworm/CElegansNeuroML/blob/master/CElegans/pythonScripts/PositionStraighten.py
 *
 *
 * Spine data from:
 *
 * https://github.com/openworm/CElegansNeuroML/blob/master/CElegans/dataSets/spine-spline.tsv
 */

class Geometrician{
    
public:
    typedef std::pair<float, int> DistPair;
    std::vector<DistPair> distPairs;
    
    // Initialize vector
    
    
public:
    
    // each bezier curve is defined by 4 points (2 end points, 2 control points)
    // the spine is (apparently) composed of 6 bezier curves
    static const int num_bezier_curves = 6;
    static const int pts_per_bezier_curve = 4;
    glm::vec3 spine[num_bezier_curves][pts_per_bezier_curve] ={
        {
            glm::vec3(   -4.95064599052e-09f,-3.54269265383f,0.593364208937),
            glm::vec3(    5.60455896274e-08f,-2.80419903994f,0.535723637789f),
            glm::vec3(    -2.11915240988e-14f,-2.71887999773f,0.493553757668f),
            glm::vec3(    7.54979084942e-08f,-1.85220086575f,-0.00531244277954f)
        },{
            glm::vec3(    7.54979084942e-08f,-1.85220086575f,-0.00531244277954f),
            glm::vec3(    1.50995873707e-07f,-0.985521316528f,-0.504178881645f),
            glm::vec3(    7.23018533162e-15f,-0.79419708252f,-0.638910531998f),
            glm::vec3(    7.54979227051e-08f,0.0825190544128f,-0.157902300358f)
        },{
            glm::vec3(    7.54979227051e-08f,0.0825190544128f,-0.157902300358f),
            glm::vec3(    1.5099591634e-07f,0.95923614502f,0.323106437922f),
            glm::vec3(    1.42537016897e-09f,1.15001249313f,0.399453416467f),
            glm::vec3(    7.54978658617e-08f,2.12088441849f,0.150156021118f)
        },{
            glm::vec3(    7.54978658617e-08f,2.12088441849f,0.150156021118f),
            glm::vec3(    1.49395361984e-07f,3.08946275711f,-0.0985524058342f),
            glm::vec3(    5.48648302026e-07f,3.05415344238f,-0.0894721746445f),
            glm::vec3(    5.47251033947e-07f,3.29034137726f,-0.0990565419197f)
        },{
            glm::vec3(    5.47251033947e-07f,3.29034137726f,-0.0990565419197f),
            glm::vec3(    5.46588921804e-07f,3.40226221085f,-0.103598237038f),
            glm::vec3(    4.71336278576e-07f,4.02469396591f,0.0354323983192f),
            glm::vec3(    5.92063088459e-07f,4.13465976715f,0.123009085655f)
        },{
            glm::vec3(    5.92063088459e-07f,4.13465976715f,0.123009085655f),
            glm::vec3(    7.30952194897e-07f,4.26116895676f,0.223760932684f),
            glm::vec3(    6.04087233694e-07f,4.47946739197f,0.494868323207f),
            glm::vec3(    5.90954471261e-07f,4.61192750931f,0.63269393146f)
        }};
    
    Geometrician();
    
    void straighten(MassPoint** points, int num_points, MassPoint*** bezPts, int* numBezPts);
    void generatePointSet();
    void bezierPointSet(glm::vec3* p, bool isLast);
    std::vector<glm::vec3> pointSet;
    std::vector<glm::vec2> bezPoly(glm::vec4 p_x, glm::vec4 p_y);
    std::vector<float> getZDistances();
    void get_nearest_point_pair(int& left, int& right, glm::vec3 coord);
    int num_t = 100;
    float* t;
    float avg_y;
    
private:
    float WRL_UPSCALE = .0001f;//.0009f;//.00009f;//100.0f;
};

#endif /* Geometrician_hpp */
