#pragma once
#ifndef _PROCESS_H
#include <string>
#include <opencv2\opencv.hpp>

bool initializelibrary();
void calculatecenter();
int recognize(std::string &address);
void hand_writting();
int recognize_base_on_B(std::string &address);

#endif