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

#pragma once

#include "myDebug.hpp"
#include "SignalBuilder.hpp"
#include "DataSetup.hpp"
#include "Core.hpp"
#include "BodyParameterSet.hpp"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "Logger.hpp"
#include "DarwinEffectuator.hpp"

#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>
#include <regex>

#ifdef __APPLE__
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif

enum class MuscleQuadrant {VL, DL, DR, VR};

void gitErDone(int* argc, char** argv);