#include <opencv2/opencv.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<vector>
#include<iostream>
#include<sstream>
#include<string>
#include<time.h> 
using namespace std;
using namespace cv;
void OnOK() //读入视频并检测车辆
{

	// TODO: Add extra validation here


	//此处添加图像处理的代码

	//CDialog::OnOK();
	IplImage *frame = NULL; //定义帧,每帧就是一张图 
	IplImage *B_part = NULL, *G_part = NULL, *R_part = NULL;
	IplImage *pFrImg = NULL; //前景图片
	IplImage *pBkImg = NULL; //背景图片
	IplImage *dst = NULL;


	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	//轮廓矩形
	CvRect contourRect = cvRect(0, 0, 0, 0);


	//可以获取视频的各种属性
	CvCapture * pCapture = cvCreateFileCapture("C://Users//徐荣//Documents//Tencent Files//1251931015//FileRecv//MobileFile//test.avi");


	cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE); //设置窗口 
	cvNamedWindow("temp", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("background", 1);
	cvNamedWindow("foreground", 1);
	cvNamedWindow("contours", 1);

	cvMoveWindow("background", 360, 0);
	cvMoveWindow("foreground", 690, 0);


	//初始化高斯混合模型参数
	CvGaussBGModel *bg_model = NULL;



	int count = 1;
	while (frame = cvQueryFrame(pCapture))
	{
		if (count == 1)
		{
			B_part = cvCreateImage(cvGetSize(frame), frame->depth, 1);
			G_part = cvCreateImage(cvGetSize(frame), frame->depth, 1);
			R_part = cvCreateImage(cvGetSize(frame), frame->depth, 1);
			pBkImg = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 3);
			pFrImg = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);
			dst = cvCreateImage(cvGetSize(frame), 8, 3);//找到轮廓并绘制轮廓
			bg_model = (CvGaussBGModel*)cvCreateGaussianBGModel(frame, 0);
		}
		else
		{
			cvUpdateBGStatModel(frame, (CvBGStatModel *)bg_model);//更新高斯背景模型

			//pFrImg为前景图像，只能为单通道
			//pBkImg为背景图像，可以为单通道或与pFrame通道数相同
			cvCopy(bg_model->foreground, pFrImg, 0);
			cvCopy(bg_model->background, pBkImg, 0);
			//把图像正过来
			pBkImg->origin = 1;
			pFrImg->origin = 1;
			cvSplit(frame, B_part, G_part, R_part, 0);
			cvFlip(B_part, B_part, 0);//沿X轴翻转一次


			//下面进行阈值分割
			//如何确定阈值？？是否存在编好的函数呢？
			//固定阈值分割方式，会导致一张图片中如果亮暗不均，一张图的不同区域分割效果差距很大
			//如果能够提前提取出图像的背景，然后想减的方式会使分割效果变好
			cvThreshold(B_part, B_part, 195, 255, CV_THRESH_BINARY);


			//阈值分割之后，进行膨胀操作和腐蚀操作
			//默认的模板由于腐蚀和膨胀的幅度比较大，效果不是很好啊
			//最好是自定义模板
			//观察白线大多为竖直的，腐蚀时主要是横向腐蚀
			//车体车头和车身由于在前玻璃的分割作用，数要是在竖向，膨胀时主要在竖向，但是需避免造成两车的太近而融为一体
			IplConvKernel * kenel = cvCreateStructuringElementEx(3, 1, 1, 0, CV_SHAPE_RECT, NULL);
			IplConvKernel * kenel2 = cvCreateStructuringElementEx(1, 3, 0, 1, CV_SHAPE_RECT, NULL);

			cvErode(B_part, B_part, kenel, 1);
			cvDilate(B_part, B_part, kenel, 1);
			cvErode(B_part, B_part, kenel2, 1);
			cvErode(B_part, B_part, kenel, 1);

			cvDilate(B_part, B_part, kenel2, 2);
			cvDilate(B_part, B_part, kenel2, 2);
			cvDilate(B_part, B_part, NULL, 1);


			//获取并绘制轮廓 
			cvFindContours(B_part, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
			cvZero(dst);
			for (; contour != 0; contour = contour->h_next)
			{


				contourRect = cvBoundingRect(contour, 0);


				if ((contourRect.width > 10) && (contourRect.height >12)){
					CvScalar color = CV_RGB(rand() & 255, rand() & 255, rand() & 255);//每次都随机颜色显示
					/* 用1替代 CV_FILLED 所指示的轮廓外形 */
					cvDrawContours(dst, contour, color, color, -1, CV_FILLED, 8);
					//cvRectangle(pFrame,pt3,pt4,CV_RGB(255,0,0),1, 8, 0 );
					cvRectangle(dst, cvPoint(contourRect.x, contourRect.y),
						cvPoint(contourRect.x + contourRect.width, contourRect.y + contourRect.height),
						CV_RGB(255, 0, 0),
						1,
						8,
						0);
				}

			}


			cvShowImage("Camera", frame);
			cvShowImage("temp", B_part);
			cvShowImage("background", pBkImg);
			cvShowImage("foreground", pFrImg);
			cvShowImage("contours", dst);


			int key1;
			key1 = cvWaitKey(300);
			if (key1 == 'q' || key1 == 'Q')
				break;


		}
		count++;
	}



	cvDestroyWindow("temp");
	cvReleaseCapture(&pCapture); //销毁摄像头 
	cvDestroyWindow("Camera"); //销毁窗口
	cvReleaseBGStatModel((CvBGStatModel**)&bg_model);
	cvDestroyWindow("background");
	cvDestroyWindow("foreground");
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);
	cvReleaseImage(&dst);
	cvDestroyWindow("contours");
	//CTrafficsampleDlg::OnOK();
 }

 int main3(){

     OnOK();
	 waitKey(0);
	 return 0;
 }