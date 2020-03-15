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
void OnOK() //������Ƶ����⳵��
{

	// TODO: Add extra validation here


	//�˴����ͼ����Ĵ���

	//CDialog::OnOK();
	IplImage *frame = NULL; //����֡,ÿ֡����һ��ͼ 
	IplImage *B_part = NULL, *G_part = NULL, *R_part = NULL;
	IplImage *pFrImg = NULL; //ǰ��ͼƬ
	IplImage *pBkImg = NULL; //����ͼƬ
	IplImage *dst = NULL;


	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	//��������
	CvRect contourRect = cvRect(0, 0, 0, 0);


	//���Ի�ȡ��Ƶ�ĸ�������
	CvCapture * pCapture = cvCreateFileCapture("C://Users//����//Documents//Tencent Files//1251931015//FileRecv//MobileFile//test.avi");


	cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE); //���ô��� 
	cvNamedWindow("temp", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("background", 1);
	cvNamedWindow("foreground", 1);
	cvNamedWindow("contours", 1);

	cvMoveWindow("background", 360, 0);
	cvMoveWindow("foreground", 690, 0);


	//��ʼ����˹���ģ�Ͳ���
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
			dst = cvCreateImage(cvGetSize(frame), 8, 3);//�ҵ���������������
			bg_model = (CvGaussBGModel*)cvCreateGaussianBGModel(frame, 0);
		}
		else
		{
			cvUpdateBGStatModel(frame, (CvBGStatModel *)bg_model);//���¸�˹����ģ��

			//pFrImgΪǰ��ͼ��ֻ��Ϊ��ͨ��
			//pBkImgΪ����ͼ�񣬿���Ϊ��ͨ������pFrameͨ������ͬ
			cvCopy(bg_model->foreground, pFrImg, 0);
			cvCopy(bg_model->background, pBkImg, 0);
			//��ͼ��������
			pBkImg->origin = 1;
			pFrImg->origin = 1;
			cvSplit(frame, B_part, G_part, R_part, 0);
			cvFlip(B_part, B_part, 0);//��X�ᷭתһ��


			//���������ֵ�ָ�
			//���ȷ����ֵ�����Ƿ���ڱ�õĺ����أ�
			//�̶���ֵ�ָʽ���ᵼ��һ��ͼƬ���������������һ��ͼ�Ĳ�ͬ����ָ�Ч�����ܴ�
			//����ܹ���ǰ��ȡ��ͼ��ı�����Ȼ������ķ�ʽ��ʹ�ָ�Ч�����
			cvThreshold(B_part, B_part, 195, 255, CV_THRESH_BINARY);


			//��ֵ�ָ�֮�󣬽������Ͳ����͸�ʴ����
			//Ĭ�ϵ�ģ�����ڸ�ʴ�����͵ķ��ȱȽϴ�Ч�����Ǻܺð�
			//������Զ���ģ��
			//�۲���ߴ��Ϊ��ֱ�ģ���ʴʱ��Ҫ�Ǻ���ʴ
			//���峵ͷ�ͳ���������ǰ�����ķָ����ã���Ҫ������������ʱ��Ҫ�����򣬵�����������������̫������Ϊһ��
			IplConvKernel * kenel = cvCreateStructuringElementEx(3, 1, 1, 0, CV_SHAPE_RECT, NULL);
			IplConvKernel * kenel2 = cvCreateStructuringElementEx(1, 3, 0, 1, CV_SHAPE_RECT, NULL);

			cvErode(B_part, B_part, kenel, 1);
			cvDilate(B_part, B_part, kenel, 1);
			cvErode(B_part, B_part, kenel2, 1);
			cvErode(B_part, B_part, kenel, 1);

			cvDilate(B_part, B_part, kenel2, 2);
			cvDilate(B_part, B_part, kenel2, 2);
			cvDilate(B_part, B_part, NULL, 1);


			//��ȡ���������� 
			cvFindContours(B_part, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
			cvZero(dst);
			for (; contour != 0; contour = contour->h_next)
			{


				contourRect = cvBoundingRect(contour, 0);


				if ((contourRect.width > 10) && (contourRect.height >12)){
					CvScalar color = CV_RGB(rand() & 255, rand() & 255, rand() & 255);//ÿ�ζ������ɫ��ʾ
					/* ��1��� CV_FILLED ��ָʾ���������� */
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
	cvReleaseCapture(&pCapture); //��������ͷ 
	cvDestroyWindow("Camera"); //���ٴ���
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