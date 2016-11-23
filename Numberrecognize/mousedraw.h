#ifndef BOXEXTRACTOR_H
#define BOXEXTRACTOR_H
 
/*
 * 鼠标控制手写笔在Mat上涂鸦
 * usage:
 *
 *  MouseHelper4OpenCV helper;
 *  Mat res = helper.MouseDraw ("Plant Here",Mat(400,400,CV_8UC3,Scalar(0,0,0)),Scalar(255,255,255),18);
 *  黑色背景Mat(400,400,CV_8UC3,Scalar(0,0,0))上使用白色画笔Scalar(255,255,255),18像素直径的画笔
 *
 *
 */
 
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
using namespace std;
 
class MouseHelper4OpenCV {
public:
    MouseHelper4OpenCV();
    cv::Mat MouseDraw(cv::Mat img);
    cv::Mat MouseDraw(const std::string& windowName, cv::Mat img, cv::Scalar color, int border);
    struct handlerT{
        bool isDrawing;
        std::vector<cv::Point> points;
        cv::Mat image;
        handlerT(): isDrawing(false) {};
    }params;
private:
    static void mouseHandler(int event, int x, int y, int flags, void *param);
    void opencv_mouse_callback( int event, int x, int y, int , void *param );
};
#endif // BOXEXTRACTOR_H