#include "mousedraw.h"

MouseHelper4OpenCV::MouseHelper4OpenCV()
{
}

void MouseHelper4OpenCV::mouseHandler(int event, int x, int y, int flags, void *param){
	MouseHelper4OpenCV *self =static_cast<MouseHelper4OpenCV*>(param);
	self->opencv_mouse_callback(event,x,y,flags,param);
}

void MouseHelper4OpenCV::opencv_mouse_callback( int event, int x, int y, int , void *param ){
	handlerT * data = (handlerT*)param;
	switch( event ){
		// update the selected bounding box
	case cv::EVENT_MOUSEMOVE:
		if( data->isDrawing ){
			data->points.push_back (cv::Point(x,y));
		}
		break;
	case cv::EVENT_LBUTTONDOWN:
		data->isDrawing = true;
		data->points.clear ();
		data->points.push_back (cv::Point(x,y));
		break;
	case cv::EVENT_LBUTTONUP:
		data->isDrawing = false;
		break;
	}
}

cv::Mat MouseHelper4OpenCV::MouseDraw(cv::Mat img){
	return MouseDraw("DRAW AN DIGIT", img,cv::Scalar(0,0,0),4);
}

cv::Mat MouseHelper4OpenCV::MouseDraw(const std::string& windowName, cv::Mat img,cv::Scalar color, int border){
	int key=0;
	cv::imshow(windowName,img);
	printf("DRAW AN DIGIT and then press SPACE/BACKSPACE/ENTER button!\n");
	params.image=img.clone();
	cv::setMouseCallback( windowName, mouseHandler, (void *)&params );
	while(!(key==32 || key==27 || key==13)){
		int length = params.points.size ();
		for(int i = 0;i<length && length > 2;i++){
			if(i == length-1){
				cv::line (params.image,params.points.at (i),params.points.at (i),color,border);
			}else{
				cv::line (params.image,params.points.at (i),params.points.at (i+1),color,border);
			}
		}
		cv::imshow(windowName,params.image);
		key=cv::waitKey(1);
	}
	return params.image;
}