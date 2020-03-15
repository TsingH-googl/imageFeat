
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


//获取下一次跳跃的方向
int getNextDirection(int lastdirection, int encourageLast){ 
	srand((unsigned)time(NULL));
	int random = rand() % 100 + 1;//生成随机数字
	if (1 <= random   &&  random <= encourageLast)//在概率k范围之内，则往上一次的跳跃方向跳跃
		return lastdirection;
	else
		return -lastdirection; //在概率k范围之外，则往上一次相反的跳跃方向跳跃
}


int main(){
	srand((unsigned)time(NULL));
	int dataori[100] = {0}; // the average number of keypoints，用于保存在不同patch size下的SIFT特征点均值。
	//计算dataori中的值
	int i = 1;
	for (i = 1; i <= 50; i++){
		string filepath = "C://Users//406//Desktop//新建文件夹//算法实验//txtdata//AllData//ground", line;
		filepath = filepath + to_string(i * 2) + ".txt"; //这里的数据假设在文件夹“C://Users//406//Desktop//新建文件夹//算法实验//txtdata//AllDatasky//”以txt文件形式保存好了。
		ifstream in(filepath);
		int sum = 0;
		while (getline(in, line)) // line中不包括每行的换行符  
		{
			int number = std::atoi(line.c_str());
			sum = sum + number;  // 统计对应patch size下的SIFT特征总数
		}
		dataori[i] = sum / 33; // 计算patch size为i的时候，SITF特征点均值
		//cout << sum / 33 << endl;
	}

	for (i = 1; i <= 50; i++)
		cout << "当patch size为："<< i*2  << "的SIFT特征点均值" << dataori[i] << endl;
	

	int data[51] = { 0 };
	int pro[51] = { 0 };//用于保存跳跃到该patch size的次数
	int step = 1, direction = -1, encourageLast = 50;//概率k初始化为0.5
	int x = rand() % 50 + 1, y = 0;
	int N = 100;//迭代次数N设置为100
	y = dataori[x];
	data[x] = dataori[x];
	pro[x]++;

	N--;
	while (N--){
		direction = getNextDirection(direction, encourageLast);
		int nextx = x + step*direction;//the next value of x
		if (nextx > 50 || nextx < 1)// if nextx is out of boundry,set to the original
			nextx = nextx - x*direction; 
		int tempy = 0;//to record the value of y，记录跳跃到该patch size对应的SIFT均值，避免重复计算。
		//get the value of next y，得到下一次“打算”跳跃的SIFT特征点均值
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
			encourageLast = 70; //设置概率为k=0.9，这里为了方便起见利用随机数落在1< = randnum <= k*100 之间的可能性
		}
		else{
			pro[x]++;
			encourageLast = 30;
		}

	}

    //统计每个patch size被访问的次数，被访问最多的那个patch size就是optimal patch size
	for (int i = 1; i <= 50; i++){
		if (pro[i]>=1)
		   cout <<i<<": "<< pro[i] <<endl;
	}
	cout << endl;
	system("pause");
	return 0;


}