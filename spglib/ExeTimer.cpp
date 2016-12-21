//
//  ExeTimer.cpp
//  cpplibs
//
//  Created by Sean Grimes on 11/30/15.
//  Copyright © 2015 Sean Grimes. All rights reserved.
//

#include "include/ExeTimer.hpp"

void ExeTimer::start_timer(){
    start_ = time_device::now();
}

void ExeTimer::stop_timer(){
    stop_ = time_device::now();
}

double ExeTimer::get_exe_time_in_sec(){
    auto duration = stop_ - start_;
    exe_time_sec_ = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return exe_time_sec_;
}

double ExeTimer::get_exe_time_in_ms(){
    auto duration = stop_ - start_;
    exe_time_ms_ = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return exe_time_ms_;
}

double ExeTimer::get_exe_time_in_micro(){
    auto duration = stop_ - start_;
    exe_time_micro_ = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    return exe_time_micro_;
}

double ExeTimer::get_exe_time_in_nano(){
    auto duration = stop_ - start_;
    exe_time_ns_ = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    return exe_time_ns_;
}