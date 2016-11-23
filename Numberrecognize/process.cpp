#include "process.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <opencv2/opencv.hpp>
#include <cstdio>
#include "mousedraw.h"

using namespace cv;
using namespace std;

const int CutupSize=28;//分割大小
int total[10][CutupSize*CutupSize]={0};//像素统计
double templatecenter[10][CutupSize*CutupSize]={0};//模板中心数组
int howmany[10]={0};//图片数量
Mat center[10];//模板中心图片
double p_first[10]={0};//先验概率
double p_j[10][CutupSize*CutupSize]={0};//像素位置信息概率
double p_conditional[10]={0};//条件概率
double p_posterior[10];//后验概率

bool initializelibrary()  //初始化，像素统计，图片数量统计，模板中心生成，模板中心图片生成。
{
	ifstream file("testimages/number.txt");//读取图片名字的TXT。
	string buffer;
	string address="testimages/";

	int counter=0;
	int num_index=0;

	while (getline(file,buffer))
	{
		address="testimages\\";
		address+=buffer;   //地址合成

		Mat srcimage=imread(address,0);//读取
		Mat binimage;
		Mat cutupimage(Size(CutupSize,CutupSize),CV_8UC1,Scalar(0));
		threshold(srcimage,binimage,123,1,0);

		int y=binimage.rows/CutupSize;
		int x=binimage.cols/CutupSize;

		for (int i=0;i<binimage.rows;i++)
			for (int j=0;j<binimage.cols;j++)
				if (binimage.at<uchar>(i,j))
					cutupimage.at<uchar>(i/y,j/x)=1; //分割
		/*
		imwrite("test.bmp",cutupimage);
		for (int i=0;i<cutupimage.rows;i++)
		{
			for (int j=0;j<cutupimage.cols;j++)
				printf("%4d",cutupimage.at<uchar>(i,j));
			cout<<endl;
		}
		*/
		for (int i=0;i<cutupimage.rows;i++)
			for (int j=0;j<cutupimage.cols;j++)
				if (cutupimage.at<uchar>(i,j))
					total[buffer[0]-'0'][i*CutupSize+j]++; //统计
		howmany[buffer[0]-'0']++;
	}
/*	
	int k=0;
	for (int i=0;i<10;i++)
	{
		cout<<"total counter #"<<i<<endl;
		for (int j=0;j<CutupSize*CutupSize;j++)
		{
			printf("%4d",total[i][j]);
			k++;
			if (k%CutupSize==0)
				cout<<endl;
		}
	}
*/	
	cout<<"initialize template library success.\n";
	calculatecenter(); //计算中心
	return true;
}

void calculatecenter() //图像中心数组，中心图片生成
{
	Mat_<float>::iterator it;
	Mat_<float>::iterator itend;
	string filename="0_center.bmp";
	for (int i=0;i<10;i++)
	{
		//printf("#%d ",i);
		//printf("have %d\n",howmany[i]);
		for (int j=0;j<CutupSize*CutupSize;j++)
			templatecenter[i][j]=total[i][j]/howmany[i];
		/*
		int k=0;
		for (int j=0;j<CutupSize*CutupSize;j++)
		{
			printf("%6.2f",templatecenter[i][j]);
			k++;
			if (k%CutupSize==0)
				cout<<endl;
		}
		*/
		center[i]=Mat(Size(CutupSize,CutupSize),CV_32FC1,Scalar(0.0));
		//printf("it's %d\n",i);
		it=center[i].begin<float>();
		itend=center[i].end<float>();
		int index=0;
		while (it!=itend)
		{
			(*it)=(float)total[i][index]/howmany[i];
			index++;
			//printf("%5.2f",*it);
			//if (index%CutupSize==0)
			//	cout<<endl;
			it++;
		}
		//cout<<endl;
		filename[0]='0'+i;
		imwrite(filename,center[i]);
	}
	cout<<"calculate the center...\n";
}

int recognize(std::string &address) //基于模板匹配的识别函数
{
	Mat srcimage=imread(address,0);
	Mat binimage;
	Mat cutupimage(Size(CutupSize,CutupSize),CV_8UC1,Scalar(0));
	threshold(srcimage,binimage,123,1,0);

	int y=binimage.rows/CutupSize;
	int x=binimage.cols/CutupSize;

	for (int i=0;i<binimage.rows;i++)
		for (int j=0;j<binimage.cols;j++)
			if (binimage.at<uchar>(i,j))
				cutupimage.at<uchar>(i/y,j/x)=1;

	int sample[CutupSize*CutupSize]={0};
	for (int i=0;i<cutupimage.rows;i++)
		for (int j=0;j<cutupimage.cols;j++)
			if (cutupimage.at<uchar>(i,j))
				sample[i*CutupSize+j]=1;

	double tmp[CutupSize*CutupSize]={0};  //用于保存样本与中心的差值
	double number[10];

	for (int i=0;i<10;i++) //基于平均值类中心求得欧氏距离
	{
		double sum=0;
		for (int j=0;j<CutupSize*CutupSize;j++)
		{
			tmp[j]=sample[j]-templatecenter[i][j];
			sum+=tmp[j]*tmp[j];			
		}
		number[i]=sqrt(sum);
		//cout<<"number #"<<i<<":  "<<number[i]<<endl;
	}

	double min=number[0];  //求距离的最小值
	int k=0;
	for (int i=0;i<10;i++)
		if (number[i]<min)
		{
			min=number[i];
			k=i;
		}
	cout<<"the number is "<<k<<endl;
	return k;
}

void hand_writting()  //手写板函数
{
	using namespace cv;
	MouseHelper4OpenCV helper;

	Mat res = helper.MouseDraw("writing board",Mat(280,280,CV_8UC3,Scalar(0,0,0)),Scalar(255,255,255),25);
	imwrite("tmp.bmp",res);

	IplImage *input=cvLoadImage("tmp.bmp",1);
	CvSize tmpsize=cvGetSize(input);
	double rate=10;
	tmpsize.height/=rate;
	tmpsize.width/=rate;
	IplImage *StdImage=cvCreateImage(tmpsize,input->depth,input->nChannels);

	cvResize(input,StdImage,CV_INTER_AREA);
	cvSaveImage("tmp.bmp",StdImage);
}

int recognize_base_on_B(std::string &address) //基于二值贝叶斯的识别函数
{
	Mat srcimage=imread(address,0);
	Mat binimage;
	Mat cutupimage(Size(CutupSize,CutupSize),CV_8UC1,Scalar(0));
	threshold(srcimage,binimage,123,1,0);

	int y=binimage.rows/CutupSize;
	int x=binimage.cols/CutupSize;

	for (int i=0;i<binimage.rows;i++)
		for (int j=0;j<binimage.cols;j++)
			if (binimage.at<uchar>(i,j))
				cutupimage.at<uchar>(i/y,j/x)=1;

	int sample[CutupSize*CutupSize]={0};
	for (int i=0;i<cutupimage.rows;i++)
		for (int j=0;j<cutupimage.cols;j++)
			if (cutupimage.at<uchar>(i,j))
				sample[i*CutupSize+j]=1;

	double sum=0;
	for (int i=0;i<10;i++)
		sum+=howmany[i];
	for (int i=0;i<10;i++)
	{
		p_first[i]=(double)howmany[i]/sum; //计算先验概率
		//printf("%5.2f\n",p_first[i]);
	}
	//cout<<"calculate first probability\n";
	for (int i=0;i<10;i++)
	{	
		//int k=0;
		for (int j=0;j<CutupSize*CutupSize;j++)
		{
			//k++;
			p_j[i][j]=(double)(total[i][j]+1)/(howmany[i]+2);
			//printf("%5.2f",p_j[i][j]);
			//if (k%CutupSize==0)
			//	cout<<endl;
		}
		//cout<<endl;
	}
	double product;
	for (int i=0;i<10;i++)
	{
		product=1;
		for (int j=0;j<CutupSize*CutupSize;j++)
		{	
			if (sample[j])
				product*=p_j[i][j];
			else
				product*=(1-p_j[i][j]);
		}
		p_conditional[i]=product; //计算条件概率
		//printf("%e\n",p_conditional[i]);
	}
	sum=0;
	for (int i=0;i<10;i++)
		sum+=p_first[i]*p_conditional[i];
	for (int i=0;i<10;i++)
		p_posterior[i]=p_first[i]*p_conditional[i]/sum; //计算后验概率
	int k=0;
	double max=p_posterior[0];
	for (int i=0;i<10;i++)
		if (p_posterior[i]>max)
		{
			max=p_posterior[i];
			k=i;
		}
	cout<<"the number is "<<k<<endl;
	return k; //返回识别结果
}

