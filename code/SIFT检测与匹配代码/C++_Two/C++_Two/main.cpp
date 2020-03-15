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


using namespace cv;//opencv库
using namespace std;

#include <iostream>
#include <ctime> 
#include "cv.h"    
#include "stdio.h"  
#include <fstream>  
#include  "io.h"
using namespace cv;
using namespace std;


/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

/******************************** Globals ************************************/

//读取一个文件夹下所有的图像文件，保存路径到files容器中
void getFiles(string path, vector<string>& files)
{
	//文件句柄
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("//*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("//").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("//").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

//该函数实现匹配两张图像时，把两张待匹配的图像在显示窗口中连成一张图像，便于后续draw匹配线
 IplImage* expand_imgs(IplImage* img1, IplImage* img2)
{
	IplImage* expanded = cvCreateImage(cvSize(
		img1->width + img2->width, MAX(img1->height, img2->height)),
		IPL_DEPTH_8U, 3);

	cvZero(expanded);
	cvSetImageROI(expanded, cvRect(0, 0, img1->width, img1->height));
	cvAdd(img1, expanded, expanded, NULL);
	cvSetImageROI(expanded, cvRect(img1->width, 0, img2->width, img2->height));
	cvAdd(img2, expanded, expanded, NULL);
	cvResetImageROI(expanded);

	return expanded;
}
// 把img1与img2进行特征匹配
void match(IplImage *img1, IplImage *img2)
{
	   IplImage *mystacked = expand_imgs(img1, img2);// 连接两张图像，用于后续画匹配线
	   IplImage *mystacked_ransac = expand_imgs(img1, img2);// 连接两张图像，用于后续画匹配线
       IplImage *img1_Feat = (img1);//复制img1，用来画特征点  
	   IplImage *img2_Feat = (img2);//复制img2，用来画特征点  
	   IplImage *stacked, *stacked_ransac;
	   struct feature *feat1, *feat2;//feat1：图1的特征点结构体，feat2：图2的特征点结构体  
	   int n1, n2;//n1:图1中的特征点个数，n2：图2中的特征点个数  
	   struct feature *feat;//每个特征点  
	   struct kd_node *kd_root;//k-d树的树根  
	   struct feature **nbrs;//当前特征点的最近邻点数组  
	   int matchNum;//经距离比值法筛选后的匹配点对的个数，距离比值法  
	   struct feature **inliers;//精RANSAC筛选后的内点数组  
	   int n_inliers;//经RANSAC算法筛选后的“内点个数”,即feat1中具有符合要求的特征点的个数  
	   //保存图像所需参数设置，即设置它们的保存位置以及名字
	   char *ransac = "D://stitching_result//ransac.jpg";
	   char *imgone = "D://stitching_result//FPS1.jpg";
	   char *imgtwo = "D://stitching_result//FPS2.jpg";
	   char *match = "D://stitching_result//match.jpg";
	   
	   
	
	  //默认提取的是LOWE格式的SIFT特征点  
	  //提取并显示第1幅图片上的特征点  
       n1 = sift_features(img1, &feat1);//检测图1中的SIFT特征点,n1是图1的特征点个数  
	   cout << "图一的特征点个数" << n1 << endl;
	   if (n1 == 0){
		   system("pause");
		   return;
	   }
	   //export_features("feature1.txt", feat1, n1);//将特征向量数据写入到文件  
	   draw_features(img1_Feat, feat1, n1);//画出特征点
       cvNamedWindow("img1_Feat", 0);//创建窗口
       cvShowImage("img1_Feat", img1_Feat);//显示  
       cvResizeWindow("img1_Feat", 600, 350);//缩小窗口的大小
	   cvSaveImage(imgone, img1_Feat);//保存图像到imgone指定的目录

	   //提取并显示第2幅图片上的特征点  
	   n2 = sift_features(img2, &feat2);//检测图2中的SIFT特征点，n2是图2的特征点个数  
	   cout << "图二的特征点个数" << n2 << endl;
	   if (n2 == 0){
		   system("pause");
		   return;
	   }
	   //export_features("feature2.txt", feat2, n2);//将特征向量数据写入到文件  
	   draw_features(img2_Feat, feat2, n2);//画出特征点  
	   cvNamedWindow("img2_Feat", 0);//创建窗口
	   cvShowImage("img2_Feat", img2_Feat);//显示  
	   cvResizeWindow("img2_Feat", 600, 350);//缩小窗口的大小
	   cvSaveImage(imgtwo, img2_Feat);//保存图像到imgone指定的目录
	
	   Point pt1, pt2;//连线的两个端点  
	   double d0, d1;//feat1中每个特征点到最近邻和次近邻的距离  
	   matchNum = 0;//经距离比值法筛选后的匹配点对的个数  
	
	   //根据图2的特征点集feat2建立k-d树，返回k-d树根给kd_root  
	   kd_root = kdtree_build(feat2, n2);
	
	   //遍历特征点集feat1，针对feat1中每个特征点feat，选取符合距离比值条件的匹配点，放到feat的fwd_match域中  
	   for (int i = 0; i < n1; i++)
		    {
		        feat = feat1 + i;//第i个特征点的指针  
		        //在kd_root中搜索目标点feat的2个最近邻点，存放在nbrs中，返回实际找到的近邻点个数  
			    int k = kdtree_bbf_knn(kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS);
		        if (k == 2)
			       {
			           d0 = descr_dist_sq(feat, nbrs[0]);//feat与最近邻点的距离的平方  
			           d1 = descr_dist_sq(feat, nbrs[1]);//feat与次近邻点的距离的平方  
		               //若d0和d1的比值小于阈值NN_SQ_DIST_RATIO_THR，则接受此匹配，否则剔除  
				            if (d0 < d1 * NN_SQ_DIST_RATIO_THR)
				            {   //将目标点feat和最近邻点作为匹配点对  
				                pt1 = Point(cvRound(feat->x), cvRound(feat->y));//图1中点的坐标  
			                    pt2 = Point(cvRound(nbrs[0]->x), cvRound(nbrs[0]->y));//图2中点的坐标(feat的最近邻点)  
								pt2.x += img1->width;
								//pt2.y += img1->height;//由于两幅图是上下排列的，pt2的纵坐标加上图1的高度，作为连线的终点  
				                cvLine(mystacked, pt1, pt2, CV_RGB(255, 0, 0), 1, 8, 0);//画出连线  
			                    matchNum++;//统计匹配点对的个数  
				                feat1[i].fwd_match = nbrs[0];//使点feat的fwd_match域指向其对应的匹配点  
				            }
			       }
		        free(nbrs);//释放近邻数组  
		    }
	    cout<<"经欧式距离比值法筛选后的匹配点对个数："<< matchNum << endl;
	    //显示并保存经距离比值法筛选后的匹配图  
		//cvNamedWindow("IMG_MATCH1",1);//创建窗口  
	  //  cvShowImage("IMG_MATCH1", mystacked);//显示  
		cvSaveImage(match, mystacked);
		//利用RANSAC算法筛选匹配点,计算变换矩阵H，如果存在变换矩阵H，说明两幅图有重叠的部分，可以拼接  
		CvMat * H = ransac_xform(feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01, homog_xfer_err, 3.0, &inliers, &n_inliers);
	    cout<< "经RANSAC算法筛选后的匹配点对个数：" << n_inliers << endl;
        //遍历经RANSAC算法筛选后的特征点集合inliers，找到每个特征点的匹配点，画出连线  
		   for (int i = 0; i<n_inliers; i++)
		    {
		       feat = inliers[i];//第i个特征点  
		       pt1 = Point(cvRound(feat->x), cvRound(feat->y));//图1中点的坐标  
		       pt2 = Point(cvRound(feat->fwd_match->x), cvRound(feat->fwd_match->y));//图2中点的坐标(feat的匹配点)  
		       //cout<< "(" << pt1.x << "," << pt1.y << ")--->(" << pt2.x << "," << pt2.y << ")" << endl;
			   pt2.x += img1->width;
			   //pt2.y += img1->height;//由于两幅图是上下排列的，pt2的纵坐标加上图1的高度，作为连线的终点  
		       cvLine(mystacked_ransac, pt1, pt2, CV_RGB(255, 0, 0), 1, 8, 0);//画出连线  	   
		   }
          cvNamedWindow("IMG_MATCH2",0);//创建窗口
          cvResizeWindow("IMG_MATCH2",800,250);//缩小窗口的大小
          cvShowImage("IMG_MATCH2", mystacked_ransac);//显示
          cvSaveImage(ransac, mystacked_ransac);
	}


	
//主函数 
int main()
{
	cout << "准备就绪" << endl;
	        //char * filePath = "D://8篇相关论文//图像数据库//Google Earth//NEW_DATABASE//newdeli//01.png";
	        //加载图像指针img1指向它，并且把它的大小下采样至指定的分辨率大小
			IplImage *img1 = cvLoadImage("D://8篇相关论文//图像数据库//Google Earth//NEW_DATABASE//kalaqi//01.png", 1);
			IplImage* imgresize1 = cvCreateImage(cvSize(800, 500), 8, 3);//指定通道数为3，指定分辨率为800*500
			cvResize(img1, imgresize1, CV_INTER_NN);

			//加载图像指针img2指向它，并且把它的大小下采样至指定的分辨率大小
			IplImage *img2 = cvLoadImage("D://8篇相关论文//图像数据库//Google Earth//NEW_DATABASE//kalaqi//02.png", 1);
			IplImage* imgresize2 = cvCreateImage(cvSize(800, 500), 8, 3);//指定通道数为3，指定分辨率为800*500
			cvResize(img2, imgresize2, CV_INTER_NN);

			if (img1 == NULL || img2 == NULL){
				cout << "读入图片失败，图片不存在或内部错误" << endl;
				system("pause");
				return -1;
			}
			cout << "已经读入两张图像" << endl << "准备匹配" << endl;
			match(imgresize1, imgresize2);
			cout << "------------------------------------------------------" << endl;
			
			
	waitKey(0);
	system("pause");
	return 0;
}
	//加载图像 
	/*
	Mat img_object = imread("D://images_integration//DJI_0031.JPG", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_scene = imread("D://images_integration//DJI_0032.JPG", CV_LOAD_IMAGE_GRAYSCALE);
	resize(img_object, img_object, Size(img_object.cols / 10, img_object.rows / 10));
	resize(img_scene, img_scene, Size(img_scene.cols / 10, img_scene.rows / 10));
	if (!img_object.data || !img_scene.data)
	{
		std::cout << " --(!) Error reading images " << std::endl;
		return -1;
	}

	double begin = clock();
	//第一步：用SIFT算子检测关键点 

	SiftFeatureDetector detector;   //构造函数采用默认的   
	std::vector<KeyPoint> keypoints_object, keypoints_scene;      //构造2个专门由点组成的点向量用来存储特征点   
	detector.detect(img_object, keypoints_object);  //将img_object图像中检测到的特征点存储起来放在keypoints_object中 
	//cout << keypoints_object.capacity << endl;
	detector.detect(img_scene, keypoints_scene);    //同理   


	//在图像中画出特征点 

	Mat img_keypoints_object, img_keypoints_scene;
	//先在内存中绘制 

	drawKeypoints(img_object, keypoints_object, img_keypoints_object, Scalar::all(-1), DrawMatchesFlags::DEFAULT);//在内存中画出特征点   
	drawKeypoints(img_scene, keypoints_scene, img_keypoints_scene, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	//再显示 

	imshow("sift_keypoints_object", img_keypoints_object);
	imshow("sift_keypoints_scene", img_keypoints_scene);

	//第二步：计算特征向量 

	SiftDescriptorExtractor extractor;  //定义描述子对象   
	Mat descriptors_1, descriptors_2;        //存放特征向量的矩阵   
	//计算特征向量 

	extractor.compute(img_object, keypoints_object, descriptors_1);
	extractor.compute(img_scene, keypoints_scene, descriptors_2);

	//第三步，用FLANN进行匹配特征向量 

	FlannBasedMatcher matcher;  //定义一个FlannBasedMatcher对象   
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	double max_dist = 0;
	double min_dist = 100;

	//计算特征点间的最大最小距离 

	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//只保留好的匹配：特征点距离小于3倍最小距离 ,RANSAC算法

	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//在内存中绘制保留的好的匹配 

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//在场景中定位目标图像 

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	std::cout << "good_matches.size(): " << good_matches.size() << "\n";
	for (int i = 0; i < good_matches.size(); i++)
	{
		//从好的匹配中找到特征点 

		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	//基于匹配的关键点找出相应的变换 

	cv::Mat H = cv::findHomography(cv::Mat(obj), cv::Mat(scene), CV_RANSAC);

	//找到目标的角点 

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows);
	obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	//映射点群，在场景中获取目标的坐标 

	cv::perspectiveTransform(cv::Mat(obj_corners), cv::Mat(scene_corners), H);

	//目标的角点之间连线（框出目标） 

	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);

	//显示保留的好的匹配 

	imshow("Good Matches & Object detection - SIFT", img_matches);
	imwrite("Good Matches & Object detection - SIFT.png", img_matches);

	double end = clock();
	cout << "\nSURF - elapsed time is: " << (end - begin) / CLOCKS_PER_SEC * 1000 << " ms\n";
*/



/*
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
*/

/*
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
*/


/*
拼接
#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
using namespace std;
using namespace cv;

int main(void)
{
	string dir = "D://images_integration//";
	string savedir = "D://stitching_result//";
	string srcFile[3] = { "DJI_0031.JPG", "DJI_0032.JPG", "DJI_0033.JPG" };
	string dstFile = savedir+"result.jpg";
	vector<Mat> imgs;
	for (int i = 0; i<3; ++i)
	{
		Mat img = imread(dir+srcFile[i]);
		if (img.empty())
		{
			cout << "Can't read image '" << srcFile[i] << "'\n";
			system("pause");
			return -1;
		}
		//resize(img, img, Size(img.cols / 2, img.rows / 2));
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
*/



//循环读取每一幅图像，特征提取，再匹配，最后保存
/*
//主函数 
int main()
{
	
	cout << "准备就绪" << endl;
	char * filePath = "D://8篇相关论文//图像数据库//Google Earth//NEW_DATABASE//newdeli//";
	vector<string> files;

	////获取该路径下的所有文件  
	getFiles(filePath, files);
	int size = files.size();
	int i = 0, j = 0;
	for (i = 2; i <3; i++){
		for (j = 3; j < 4; j++)
		{

			cout << "图一：" << files[i].c_str() << endl;
			cout << "图二：" << files[j].c_str() << endl;


			IplImage *img1 = cvLoadImage(files[i].c_str(), 1);
			IplImage* imgresize1 = cvCreateImage(cvSize(800, 500), 8, 3);//指定通道数为3
			cvResize(img1, imgresize1, CV_INTER_NN);

			IplImage *img2 = cvLoadImage(files[j].c_str(), 1);
			IplImage* imgresize2 = cvCreateImage(cvSize(800, 500), 8, 3);
			cvResize(img2, imgresize2, CV_INTER_NN);

			if (img1 == NULL || img2 == NULL){
				cout << "读入图片失败，图片不存在或内部错误" << endl;
				system("pause");
				return -1;
			}
			cout << "已经读入两张图像" << endl << "准备匹配" << endl;
			match(imgresize1, imgresize2);
			cout << "------------------------------------------------------" << endl;


		}
	}
	waitKey(0);
	system("pause");
	return 0;
}
*/