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

const int CutupSize=28;//�ָ��С
int total[10][CutupSize*CutupSize]={0};//����ͳ��
double templatecenter[10][CutupSize*CutupSize]={0};//ģ����������
int howmany[10]={0};//ͼƬ����
Mat center[10];//ģ������ͼƬ
double p_first[10]={0};//�������
double p_j[10][CutupSize*CutupSize]={0};//����λ����Ϣ����
double p_conditional[10]={0};//��������
double p_posterior[10];//�������

bool initializelibrary()  //��ʼ��������ͳ�ƣ�ͼƬ����ͳ�ƣ�ģ���������ɣ�ģ������ͼƬ���ɡ�
{
	ifstream file("testimages/number.txt");//��ȡͼƬ���ֵ�TXT��
	string buffer;
	string address="testimages/";

	int counter=0;
	int num_index=0;

	while (getline(file,buffer))
	{
		address="testimages\\";
		address+=buffer;   //��ַ�ϳ�

		Mat srcimage=imread(address,0);//��ȡ
		Mat binimage;
		Mat cutupimage(Size(CutupSize,CutupSize),CV_8UC1,Scalar(0));
		threshold(srcimage,binimage,123,1,0);

		int y=binimage.rows/CutupSize;
		int x=binimage.cols/CutupSize;

		for (int i=0;i<binimage.rows;i++)
			for (int j=0;j<binimage.cols;j++)
				if (binimage.at<uchar>(i,j))
					cutupimage.at<uchar>(i/y,j/x)=1; //�ָ�
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
					total[buffer[0]-'0'][i*CutupSize+j]++; //ͳ��
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
	calculatecenter(); //��������
	return true;
}

void calculatecenter() //ͼ���������飬����ͼƬ����
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

int recognize(std::string &address) //����ģ��ƥ���ʶ����
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

	double tmp[CutupSize*CutupSize]={0};  //���ڱ������������ĵĲ�ֵ
	double number[10];

	for (int i=0;i<10;i++) //����ƽ��ֵ���������ŷ�Ͼ���
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

	double min=number[0];  //��������Сֵ
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

void hand_writting()  //��д�庯��
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

int recognize_base_on_B(std::string &address) //���ڶ�ֵ��Ҷ˹��ʶ����
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
		p_first[i]=(double)howmany[i]/sum; //�����������
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
		p_conditional[i]=product; //������������
		//printf("%e\n",p_conditional[i]);
	}
	sum=0;
	for (int i=0;i<10;i++)
		sum+=p_first[i]*p_conditional[i];
	for (int i=0;i<10;i++)
		p_posterior[i]=p_first[i]*p_conditional[i]/sum; //����������
	int k=0;
	double max=p_posterior[0];
	for (int i=0;i<10;i++)
		if (p_posterior[i]>max)
		{
			max=p_posterior[i];
			k=i;
		}
	cout<<"the number is "<<k<<endl;
	return k; //����ʶ����
}

