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
Mat Stitched(Mat img1, Mat img2) {
	Mat g1(img1, Rect(0, 0, img1.cols, img1.rows));
	Mat g2(img2, Rect(0, 0, img2.cols, img2.rows));
	cvtColor(g1, g1, CV_BGR2GRAY);
	cvtColor(g2, g2, CV_BGR2GRAY);
	SiftFeatureDetector siftdet;
	vector<KeyPoint>kp1, kp2;
	//SIFT sift;
	SiftDescriptorExtractor extractor;
	Mat descriptor1, descriptor2;
	FlannBasedMatcher matcher;
	vector<DMatch> matches, goodmatches;
	/*进行特征点提取*/
	siftdet.detect(g1, kp1);
	siftdet.detect(g2, kp2);
	/* 进行特征向量提取 */
	extractor.compute(g1, kp1, descriptor1);
	extractor.compute(g2, kp2, descriptor2);
	/* 进行特征向量临近匹配 */
	matcher.match(descriptor1, descriptor2, matches);
	Mat  firstmatches;
	/*画出第一次匹配的结果*/
	drawMatches(img1, kp1, img2, kp2,
		matches, firstmatches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("first_matches", firstmatches);
	/* 下面计算向量距离的最大值与最小值 */
	double max_dist = 0; double min_dist = 1000;
	for (int i = 0; i < descriptor1.rows; i++) {
		if (matches[i].distance > max_dist) {
			max_dist = matches[i].distance;
		}
		if (matches[i].distance < min_dist) {
			min_dist = matches[i].distance;
		}
	}
	cout << "The max distance is: " << max_dist << endl;
	cout << "The min distance is: " << min_dist << endl;
	for (int i = 0; i < descriptor1.rows; i++) {
		if (matches[i].distance < 2 * min_dist) {
			goodmatches.push_back(matches[i]);
		}
	}
	Mat img_matches;
	/*第二次筛选后的结果*/
	drawMatches(img1, kp1, img2, kp2,
		goodmatches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("good_matches", img_matches);
	vector<Point2f> keypoints1, keypoints2;
	for (int i = 0; i < goodmatches.size(); i++) {
		keypoints1.push_back(kp1[goodmatches[i].queryIdx].pt);
		keypoints2.push_back(kp2[goodmatches[i].trainIdx].pt);
	}
	/*计算单应矩阵*/
	Mat H = findHomography(keypoints1, keypoints2, CV_RANSAC);
	Mat stitchedImage;
	int mRows = img2.rows;
	if (img1.rows> img2.rows)
	{
		mRows = img1.rows;
	}
	/*判断图像在左边还是在右边*/
	int propimg1 = 0, propimg2 = 0;
	for (int i = 0; i < goodmatches.size(); i++) {
		if (kp1[goodmatches[i].queryIdx].pt.x > img1.cols / 2) {
			propimg1++;
		}
		if (kp2[goodmatches[i].trainIdx].pt.x > img2.cols / 2) {
			propimg2++;
		}
	}
	bool flag = false;
	Mat imgright;
	Mat imgleft;
	if ((propimg1 / (goodmatches.size() + 0.0)) > (propimg2 / (goodmatches.size() + 0.0))) {
		imgleft = img1.clone();
		flag = true;
	}
	else {
		imgleft = img2.clone();
		flag = false;
	}
	if (flag) {
		imgright = img2.clone();
		flag = false;
	}
	else {
		imgright = img1.clone();
	}
	/*把上边求得的右边的图像经过矩阵H转换到stitchedImage中对应的位置*/
	warpPerspective(imgright, stitchedImage, H, Size(img2.cols + img1.cols, mRows));
	/*把左边的图像放进来*/
	Mat half(stitchedImage, Rect(0, 0, imgleft.cols, imgleft.rows));
	imgleft.copyTo(half);
	return stitchedImage;
}

bool ImageOverlap(Mat &img1, Mat &img2, vector<KeyPoint> &vPtsImg1, vector<KeyPoint> &vPtsImg2)
{
	cv::Mat g1(img1, Rect(0, 0, img1.cols, img1.rows));
	cv::Mat g2(img2, Rect(0, 0, img2.cols, img2.rows));

	cv::cvtColor(g1, g1, CV_BGR2GRAY);
	cv::cvtColor(g2, g2, CV_BGR2GRAY);

	std::vector<cv::KeyPoint> keypoints_roi, keypoints_img;  /* keypoints found using SIFT */
	cv::Mat descriptor_roi, descriptor_img;             /* Descriptors for SIFT */
	cv::FlannBasedMatcher matcher;                      /* FLANN based matcher to match keypoints */
	std::vector<cv::DMatch> matches, good_matches;
	cv::SIFT sift;
	int i, dist = 80;

	sift(g1, Mat(), keypoints_roi, descriptor_roi);      /* get keypoints of ROI image */
	sift(g2, Mat(), keypoints_img, descriptor_img);         /* get keypoints of the image */
	matcher.match(descriptor_roi, descriptor_img, matches);

	double max_dist = 0; double min_dist = 1000;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptor_roi.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	for (i = 0; i < descriptor_roi.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//printf("%ld no. of matched keypoints in right image\n", good_matches.size());
	/* Draw matched keypoints */

	//Mat img_matches;
	//drawMatches(img1, keypoints_roi, img2, keypoints_img, 
	//    good_matches, img_matches, Scalar::all(-1), 
	//    Scalar::all(-1), vector<char>(), 
	//    DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//imshow("matches",img_matches);

	vector<Point2f> keypoints1, keypoints2;
	for (i = 0; i<good_matches.size(); i++)
	{
		keypoints1.push_back(keypoints_img[good_matches[i].trainIdx].pt);
		keypoints2.push_back(keypoints_roi[good_matches[i].queryIdx].pt);
	}
	//计算单应矩阵
	Mat H = findHomography(keypoints1, keypoints2, CV_RANSAC);

	//show stitchImage
	// cv::Mat stitchedImage;
	// int mRows = img2.rows;
	// if (img1.rows> img2.rows)
	// {
	// mRows = img1.rows;
	// }
	// stitchedImage = Mat::zeros(img2.cols+img1.cols, mRows, CV_8UC3);
	// warpPerspective(img2,stitchedImage,H,Size(img2.cols+img1.cols,mRows));
	// Mat half(stitchedImage,Rect(0,0,img1.cols,img1.rows));
	// img1.copyTo(half);
	// imshow("stitchedImage",stitchedImage);

	std::vector<cv::Point> vSrcPtsImg1;
	std::vector<cv::Point> vSrcPtsImg2;

	vSrcPtsImg1.push_back(cv::Point(0, 0));
	vSrcPtsImg1.push_back(cv::Point(0, img1.rows));
	vSrcPtsImg1.push_back(cv::Point(img1.cols, img1.rows));
	vSrcPtsImg1.push_back(cv::Point(img1.cols, 0));

	vSrcPtsImg2.push_back(cv::Point(0, 0));
	vSrcPtsImg2.push_back(cv::Point(0, img2.rows));
	vSrcPtsImg2.push_back(cv::Point(img2.cols, img2.rows));
	vSrcPtsImg2.push_back(cv::Point(img2.cols, 0));

	//计算图像2在图像1中对应坐标信息
	std::vector<cv::Point> vWarpPtsImg2;
	for (int i = 0; i < vSrcPtsImg2.size(); i++)
	{
		cv::Mat srcMat = Mat::zeros(3, 1, CV_64FC1);
		srcMat.at<double>(0, 0) = vSrcPtsImg2[i].x;
		srcMat.at<double>(1, 0) = vSrcPtsImg2[i].y;
		srcMat.at<double>(2, 0) = 1.0;

		cv::Mat warpMat = H * srcMat;
		cv::Point warpPt;
		warpPt.x = cvRound(warpMat.at<double>(0, 0) / warpMat.at<double>(2, 0));
		warpPt.y = cvRound(warpMat.at<double>(1, 0) / warpMat.at<double>(2, 0));

		vWarpPtsImg2.push_back(warpPt);
	}
	//计算图像1和转换后的图像2的交点


	for (int i = 0; i < vPtsImg1.size(); i++)
	{
		cv::Mat srcMat = Mat::zeros(3, 1, CV_64FC1);
		//srcMat.at<double>(0, 0) = vPtsImg1[i].x;
		//srcMat.at<double>(1, 0) = vPtsImg1[i].y;
		srcMat.at<double>(2, 0) = 1.0;

		cv::Mat warpMat = H.inv() * srcMat;
		cv::Point warpPt;
		warpPt.x = cvRound(warpMat.at<double>(0, 0) / warpMat.at<double>(2, 0));
		warpPt.y = cvRound(warpMat.at<double>(1, 0) / warpMat.at<double>(2, 0));
//vPtsImg2.push_back(warpPt);
	}
	return true;
}


int red_area_count_main() {
	float rate;
	cv::VideoCapture capture(NULL);    // 初始化摄像头 

    if (!capture.isOpened()) 

  return 0;        // 摄像头打开失败，直接退出 

 cv::Mat image, HSV, channels[3]; 

 //float threshold = 0.3;        // 定义阀值，如果整幅图片30%以上都是红色返回1 

 while (capture.read(image))        // 从摄像头取图 

 { 
//imshow("ResultImage", image);

  cv::cvtColor(image, HSV, CV_BGR2HSV);        // 将RGB空间转换为HSV空间 

  cv::split(HSV, channels);        // 把HSV三通道拆开 

// 下面三句表示把H < 15 || H > 245 （红色）的地方设置为1，其他地方设置为0   

  cv::threshold(channels[0], channels[1], 15, 1,CV_THRESH_BINARY_INV); 

  cv::threshold(channels[0], channels[0], 245, 1,CV_THRESH_BINARY); 

  channels[0] += channels[1]; 

  erode(channels[0], channels[0], cv::Mat::ones(3,3,CV_8UC1), cv::Point(1,1), 3);        // 去掉孤立的离散的点 

  Scalar v = cv::sum(channels[0]);        // 计算有多少个1，也就是红色的面积 

  int result = v[0];        // v[0]为红色的面积，channels[0].total()为总面积。 
  rate = (float)result / (float)(channels[0].total());
  //cout << (float)result << endl;
  //cout << (float)(channels[0].total()) << endl;
  cout << "红色面积"<<(float)rate*100 <<"%"<< endl;

 }

	/*
	string dir = "D://images_integration//";
	Mat img1 = imread("D://images_integration//1.jpg");
	Mat  stitchedImage;
	int n;
	cout << "Dataset2" << endl;
	cout << "请输入想拼接的图片数量（大于1小于18）" << endl;
	cin >> n;
	cout << "输入成功，开始计时" << endl;
	clock_t start, finish;
	double totaltime;
	start = clock();
	resize(img1, img1, Size(img1.cols/4, img1.rows /4));
	for (int k = 2; k <= n; k++) {
		stringstream stream;
		string str;
		stream << k;
		stream >> str;
		string filename = dir+str + ".jpg";
		cout << "正在拼接......." << filename << endl;
		Mat img = imread(filename);
		resize(img, img, Size(img.cols / 4, img.rows / 4));
		stitchedImage = Stitched(img1, img);
		img1 = stitchedImage;
	}
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "拼接成功" << endl;
	cout << "拼接花费总时间为：" << totaltime << "秒！" << endl;
	imshow("ResultImage", stitchedImage);
	imwrite("ResultImage.jpg", stitchedImage);
	*/
	waitKey(0);
	return 0;
}