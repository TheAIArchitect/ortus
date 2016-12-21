//
//  ExeTimer.hpp
//  cpplibs
//
//  Created by Sean Grimes on 11/30/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#pragma once

#include <chrono>
#include <iostream>

using time_device = std::chrono::high_resolution_clock;
using time_p = std::chrono::high_resolution_clock::time_point;

class ExeTimer{
public:
    void start_timer();
    void stop_timer();
    double get_exe_time_in_sec();
    double get_exe_time_in_ms();
    double get_exe_time_in_micro();
    double get_exe_time_in_nano();
   
private:
    time_p start_;
    time_p stop_;
    double exe_time_sec_{};
    double exe_time_ms_{};
    double exe_time_micro_{};
    double exe_time_ns_{};
};