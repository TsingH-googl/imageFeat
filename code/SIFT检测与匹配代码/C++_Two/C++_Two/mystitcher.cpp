
#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
using namespace std;
using namespace cv;

int stitcher_main(void)
{
	string dir = "D://stitching_result//kalaqi//";//modify
	string savedir = "D://stitching_result//kalaqi//";//modify
	string srcFile[9] = { "down.jpg","21222324.jpg"};//modify
	string dstFile = savedir + "top4.jpg";//modify
	vector<Mat> imgs;
	for (int i = 0; i<2; ++i)//modify
	{
		Mat img = imread(dir + srcFile[i]);
		if (img.empty())
		{
			cout << "Can't read image '" << srcFile[i] << "'\n";
			system("pause");
			return -1;
		}
	
			resize(img, img, Size(img.cols, img.rows));//modify
	
		imgs.push_back(img);
	}
	cout << "Please wait..." << endl;
	Mat pano;
	Stitcher stitcher = Stitcher::createDefault(false);
	Stitcher::Status status = stitcher.stitch(imgs, pano);
	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code=" << int(status) << endl;
		system("pause");
		return -1;
	}
	imwrite(dstFile, pano);
	namedWindow("Result");
	imshow("Result", pano);

	waitKey(0);

	destroyWindow("Result");
	system("pause");
	return 0;
}