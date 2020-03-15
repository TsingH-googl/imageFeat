
#include<iostream>
#include<ctime>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include <cstdlib>  
#include <ctime>  
#include <fstream>  
#include <string> 
using namespace std;


//��ȡ��һ����Ծ�ķ���
int getNextDirection(int lastdirection, int encourageLast){ 
	srand((unsigned)time(NULL));
	int random = rand() % 100 + 1;//�����������
	if (1 <= random   &&  random <= encourageLast)//�ڸ���k��Χ֮�ڣ�������һ�ε���Ծ������Ծ
		return lastdirection;
	else
		return -lastdirection; //�ڸ���k��Χ֮�⣬������һ���෴����Ծ������Ծ
}


int main(){
	srand((unsigned)time(NULL));
	int dataori[100] = {0}; // the average number of keypoints�����ڱ����ڲ�ͬpatch size�µ�SIFT�������ֵ��
	//����dataori�е�ֵ
	int i = 1;
	for (i = 1; i <= 50; i++){
		string filepath = "C://Users//406//Desktop//�½��ļ���//�㷨ʵ��//txtdata//AllData//ground", line;
		filepath = filepath + to_string(i * 2) + ".txt"; //��������ݼ������ļ��С�C://Users//406//Desktop//�½��ļ���//�㷨ʵ��//txtdata//AllDatasky//����txt�ļ���ʽ������ˡ�
		ifstream in(filepath);
		int sum = 0;
		while (getline(in, line)) // line�в�����ÿ�еĻ��з�  
		{
			int number = std::atoi(line.c_str());
			sum = sum + number;  // ͳ�ƶ�Ӧpatch size�µ�SIFT��������
		}
		dataori[i] = sum / 33; // ����patch sizeΪi��ʱ��SITF�������ֵ
		//cout << sum / 33 << endl;
	}

	for (i = 1; i <= 50; i++)
		cout << "��patch sizeΪ��"<< i*2  << "��SIFT�������ֵ" << dataori[i] << endl;
	

	int data[51] = { 0 };
	int pro[51] = { 0 };//���ڱ�����Ծ����patch size�Ĵ���
	int step = 1, direction = -1, encourageLast = 50;//����k��ʼ��Ϊ0.5
	int x = rand() % 50 + 1, y = 0;
	int N = 100;//��������N����Ϊ100
	y = dataori[x];
	data[x] = dataori[x];
	pro[x]++;

	N--;
	while (N--){
		direction = getNextDirection(direction, encourageLast);
		int nextx = x + step*direction;//the next value of x
		if (nextx > 50 || nextx < 1)// if nextx is out of boundry,set to the original
			nextx = nextx - x*direction; 
		int tempy = 0;//to record the value of y����¼��Ծ����patch size��Ӧ��SIFT��ֵ�������ظ����㡣
		//get the value of next y���õ���һ�Ρ����㡱��Ծ��SIFT�������ֵ
		if (data[nextx] == 0){
			data[nextx] = dataori[nextx];
			tempy = data[nextx];
		}
		else
			tempy = data[nextx];
		if (y < tempy){
			y = tempy;
			x = nextx;
			pro[x]++;
			encourageLast = 70; //���ø���Ϊk=0.9������Ϊ�˷�������������������1< = randnum <= k*100 ֮��Ŀ�����
		}
		else{
			pro[x]++;
			encourageLast = 30;
		}

	}

    //ͳ��ÿ��patch size�����ʵĴ����������������Ǹ�patch size����optimal patch size
	for (int i = 1; i <= 50; i++){
		if (pro[i]>=1)
		   cout <<i<<": "<< pro[i] <<endl;
	}
	cout << endl;
	system("pause");
	return 0;


}