#include <iostream>
#include <ctime> 
#include "opencv2/core/core.hpp" 
#include "opencv2/nonfree/features2d.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/calib3d/calib3d.hpp" 
#include "cv.h"    
#include "stdio.h"  
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include "..\sift\sift.h"
#include "..\sift\imgfeatures.h"
#include "..\sift\utils.h"
#include "..\sift\xform.h"
#include "..\sift\kdtree.h"
#include "..\sift\minpq.h"

#include "opencv2/core/core.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/nonfree/nonfree.hpp"  
#include "opencv2/calib3d/calib3d.hpp"  


#include <iostream>  
#include <fstream>  


using namespace cv;
using namespace std;

#include <iostream>
#include <ctime> 
#include "cv.h"    
#include "stdio.h"  
#include <fstream>  
#include  "io.h"
using namespace cv;
using namespace std;


extern void getFiles(string path, vector<string>& files);
void cal_density(IplImage *src, int block, double w){

	//图像分别有三个颜色通道  
	IplImage *dst1 = NULL;
	IplImage *dst2 = NULL;
	IplImage *dst3 = NULL;
	IplImage *imgroi1;
	//dst1的ROI  
	IplImage *imgroi2;
	//dst2的ROI  
	IplImage *imgroi3;
	//dst3的ROI  
	IplImage *roidark;
	//dark channel的ROI  
	IplImage *dark_channel = NULL;

	//去雾后的图像,三通道合并成  
	IplImage *dst = NULL;
	//源图像ROI位置以及大小  
	CvRect ROI_rect;

	//分离的三个通道  
	dst1 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	dst2 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	dst3 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);

	//为各个ROI分配内存  
	imgroi1 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	imgroi2 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	imgroi3 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	roidark = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);

	//为暗原色先验指针分配大小  
	dark_channel = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	//将原彩色图像分离成三通道  
	cvSplit(src, dst1, dst2, dst3, NULL);
	//求暗原色  
	ROI_rect.width = block;
	ROI_rect.height = block;
	ROI_rect.x = 0;
	ROI_rect.y = 0;


	int i;
	int j;
	double min1 = 0;
	double max1 = 0;
	double min2 = 0;
	double max2 = 0;
	double min3 = 0;
	double max3 = 0;
	double min = 0;
	CvScalar value;
	int ii = 0;//我自定义，用于计算黑通道均值的
	int sum = 0;
	for (i = 0; i<src->width / block; i++)
	{
		for (j = 0; j<src->height / block; j++)
		{
			ii++;
			//分别计算三个通道内ROI的最小值  
			cvSetImageROI(dst1, ROI_rect);
			cvCopy(dst1, imgroi1, NULL);
			cvMinMaxLoc(imgroi1, &min1, &max1, NULL, NULL);
			cvSetImageROI(dst2, ROI_rect);
			cvCopy(dst2, imgroi2, NULL);
			cvMinMaxLoc(imgroi2, &min2, &max2, NULL, NULL);
			cvSetImageROI(dst3, ROI_rect);
			cvCopy(dst3, imgroi3, NULL);
			cvMinMaxLoc(imgroi3, &min3, &max3, NULL, NULL);
			//求三个通道内最小值的最小值  
			if (min1<min2)
				min = min1;
			else
				min = min2;
			if (min>min3)
				min = min3;//min为这个ROI中暗原色  
			value = cvScalar(min, min, min, min);//min放在value中  
			//min赋予dark_channel中相应的ROI  
			//cout << "min" <<min<< endl;
			sum = sum + min;

			cvSetImageROI(dark_channel, ROI_rect);
			cvSet(roidark, value, NULL);
			cvCopy(roidark, dark_channel, NULL);
			//释放各个ROI  
			cvResetImageROI(dst1);
			cvResetImageROI(dst2);
			cvResetImageROI(dst3);
			cvResetImageROI(dark_channel);
			//转入下一个ROI  
			ROI_rect.x = block*i;
			ROI_rect.y = block*j;
		}
	}
	cout << "循环次数" << ii<<endl;
	cout << "sum:" << sum << endl;
	cout << "暗原色均值:" << (sum / ii) << endl;
}

int etmain(){

	char * filePath = "D://8篇相关论文//图像数据库//Google Earth//NEW_DATABASE//guangzhou//";
	vector<string> files;
	////获取该路径下的所有文件  
	getFiles(filePath, files);
	int size = files.size();
	cout << size << endl;
	int i = 0;
	IplImage *timg;
	for (i = 0; i < size; i++){
		cout << files[i].c_str() << endl;
		IplImage *timg = cvLoadImage(files[i].c_str(), CV_LOAD_IMAGE_COLOR);
		cal_density(timg, 10, 0.95);
	}
	waitKey(0);
	system("pause");
	return 0;
}