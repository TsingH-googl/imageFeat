#include<iostream>
#include<ctime>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
using namespace std;

int getNextDirection(int lastdirection, int encourageLast){
	srand((unsigned)time(NULL));
	int random = rand() % 100 + 1;
	if (1 <= random &&random<= encourageLast)
		return lastdirection;
	else
		return -lastdirection;
}


int rwalkmain(){
	srand((unsigned)time(NULL));
	int dataori[] = { 0,1, 2, 3, 4, 5, 6, 22, 8, 9, 10, 22, 19, 18, 17, 21, 15, 14, 13, 12, 11 }; // the average number of keypoints
	int data[21] = { 0 };
	int pro[21] = { 0 };//count probability
	int step = 1, direction = -1,encourageLast=50;//it is initialized as 50;
	int x = rand() % 20 + 1, y = 0;
	int N = 100;//iteration times
	y = dataori[x];
	data[x] = dataori[x];
	pro[x]++;

	while (N--){
		direction = getNextDirection(direction, encourageLast);
		int nextx = x + step*direction;//the next value of x
		if (nextx > 20 || nextx < 1)// if nextx is out of boundry,set to the original
			nextx = nextx - x*direction;
		int tempy = 0;//to record the value of y
		//get the value of next y
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
			encourageLast = 90;
		}
		else{
			pro[x]++;
			encourageLast = 10;
		}

	}

	for (int i = 1; i <= 20; i++){
		cout << pro[i] << " ";
	}
	cout << endl;
	system("pause");
	return 0;


}