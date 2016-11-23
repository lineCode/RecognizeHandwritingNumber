#include <opencv2\opencv.hpp>
#include "process.h"
#include <fstream>
#include "mousedraw.h"

using namespace cv;
using namespace std;

int Numright[10]={0};

int main()
{
	initializelibrary();
	
	ifstream file("train-images\\number.txt");
	string buffer;
	string address="train-images\\";

	while (getline(file,buffer))
	{
		cout<<buffer<<"                      ";
		address="train-images\\";
		address+=buffer;
		//int k=recognize(address);
		int k=recognize_base_on_B(address);
		if (buffer[0]-'0'==k)
			Numright[buffer[0]-'0']++;
	}

	for (int i=0;i<10;i++)
		cout<<"#"<<i<<" the right recognize have "<<Numright[i]<<endl;
	
	cout<<"Do you want to write a number? Y/N(EOF to quit)\n ";
	char judge;
	cin>>judge;
	if (judge=='Y')
	while (1)
	{
		hand_writting();
		string tmp="tmp.bmp";
		recognize_base_on_B(tmp);
	}

	return 0;
}