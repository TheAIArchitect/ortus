//
//  Chief.hpp
//  delegans
//
//  Created by onyx on 9/15/16.
//  Copyright © 2016 Andrew W.E. McDonald. All rights reserved.
//

#ifndef GraphicsGovernor_h
#define GraphicsGovernor_h

#include "std_graphics_header.hpp"

// GL includes
#include <Shader.hpp>
//#include <Camera.hpp>
#include "Camera.hpp"



// Other Libs
#include <SOIL.h>
//#include <learnopengl/filesystem.h>

#include <cmath>
#include <stack>

#include <queue>

#include "Artist.hpp"


#include "OptionForewoman.hpp"

#include "ConnectionComrade.hpp"
#include "Probe.hpp"


int initGraphics(DataSteward* stewie); // im not happy about doing this... graphics shouldn't hold core... something bigger should hold them both. and core should be called ComputeCore or something


#endif /* Chief_h */
