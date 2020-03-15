#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<vector>
using namespace std;
using namespace cv;

int main()
{
	const char* imagename = "D:\\NEW_NO_FOG_DATABASE\\below\\DJI_0050.JPG";

	//从文件中读入图像
	Mat img = imread(imagename);
	resize(img, img, Size(img.cols / 10, img.rows / 10));
	Mat img2 = imread("D:\\NEW_NO_FOG_DATABASE\\below\\DJI_0051.JPG");
	resize(img2, img2, Size(img2.cols / 100, img2.rows / 100));
	//如果读入图像失败
	if (img.empty())
	{
		fprintf(stderr, "Can not load image %s\n", imagename);
		return -1;
	}
	if (img2.empty())
	{
		fprintf(stderr, "Can not load image %s\n", imagename);
		return -1;
	}
	//显示图像
	imshow("image before", img);
	imshow("image2 before", img2);


	//sift特征检测
	SiftFeatureDetector  siftdtc;
	vector<KeyPoint>kp1, kp2;

	siftdtc.detect(img, kp1);
	Mat outimg1;
	drawKeypoints(img, kp1, outimg1);
	imshow("image1 keypoints", outimg1);
	KeyPoint kp;

	vector<KeyPoint>::iterator itvc;
	for (itvc = kp1.begin(); itvc != kp1.end(); itvc++)
	{
		cout << "angle:" << itvc->angle << "\t" << itvc->class_id << "\t" << itvc->octave << "\t" << itvc->pt << "\t" << itvc->response << endl;
	}

	siftdtc.detect(img2, kp2);
	Mat outimg2;
	drawKeypoints(img2, kp2, outimg2);
	imshow("image2 keypoints", outimg2);


	SiftDescriptorExtractor extractor;
	Mat descriptor1, descriptor2;
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> matches;
	Mat img_matches;
	extractor.compute(img, kp1, descriptor1);
	extractor.compute(img2, kp2, descriptor2);


	imshow("desc", descriptor1);
	cout << endl << descriptor1 << endl;
	matcher.match(descriptor1, descriptor2, matches);

	drawMatches(img, kp1, img2, kp2, matches, img_matches);
	imshow("matches", img_matches);

	//此函数等待按键，按键盘任意键就返回
	waitKey();
	return 0;
}
