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


using namespace cv;//opencv��
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

//��ȡһ���ļ��������е�ͼ���ļ�������·����files������
void getFiles(string path, vector<string>& files)
{
	//�ļ����
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("//*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
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

//�ú���ʵ��ƥ������ͼ��ʱ�������Ŵ�ƥ���ͼ������ʾ����������һ��ͼ�񣬱��ں���drawƥ����
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
// ��img1��img2��������ƥ��
void match(IplImage *img1, IplImage *img2)
{
	   IplImage *mystacked = expand_imgs(img1, img2);// ��������ͼ�����ں�����ƥ����
	   IplImage *mystacked_ransac = expand_imgs(img1, img2);// ��������ͼ�����ں�����ƥ����
       IplImage *img1_Feat = (img1);//����img1��������������  
	   IplImage *img2_Feat = (img2);//����img2��������������  
	   IplImage *stacked, *stacked_ransac;
	   struct feature *feat1, *feat2;//feat1��ͼ1��������ṹ�壬feat2��ͼ2��������ṹ��  
	   int n1, n2;//n1:ͼ1�е������������n2��ͼ2�е����������  
	   struct feature *feat;//ÿ��������  
	   struct kd_node *kd_root;//k-d��������  
	   struct feature **nbrs;//��ǰ�����������ڵ�����  
	   int matchNum;//�������ֵ��ɸѡ���ƥ���Եĸ����������ֵ��  
	   struct feature **inliers;//��RANSACɸѡ����ڵ�����  
	   int n_inliers;//��RANSAC�㷨ɸѡ��ġ��ڵ������,��feat1�о��з���Ҫ���������ĸ���  
	   //����ͼ������������ã����������ǵı���λ���Լ�����
	   char *ransac = "D://stitching_result//ransac.jpg";
	   char *imgone = "D://stitching_result//FPS1.jpg";
	   char *imgtwo = "D://stitching_result//FPS2.jpg";
	   char *match = "D://stitching_result//match.jpg";
	   
	   
	
	  //Ĭ����ȡ����LOWE��ʽ��SIFT������  
	  //��ȡ����ʾ��1��ͼƬ�ϵ�������  
       n1 = sift_features(img1, &feat1);//���ͼ1�е�SIFT������,n1��ͼ1�����������  
	   cout << "ͼһ�����������" << n1 << endl;
	   if (n1 == 0){
		   system("pause");
		   return;
	   }
	   //export_features("feature1.txt", feat1, n1);//��������������д�뵽�ļ�  
	   draw_features(img1_Feat, feat1, n1);//����������
       cvNamedWindow("img1_Feat", 0);//��������
       cvShowImage("img1_Feat", img1_Feat);//��ʾ  
       cvResizeWindow("img1_Feat", 600, 350);//��С���ڵĴ�С
	   cvSaveImage(imgone, img1_Feat);//����ͼ��imgoneָ����Ŀ¼

	   //��ȡ����ʾ��2��ͼƬ�ϵ�������  
	   n2 = sift_features(img2, &feat2);//���ͼ2�е�SIFT�����㣬n2��ͼ2�����������  
	   cout << "ͼ�������������" << n2 << endl;
	   if (n2 == 0){
		   system("pause");
		   return;
	   }
	   //export_features("feature2.txt", feat2, n2);//��������������д�뵽�ļ�  
	   draw_features(img2_Feat, feat2, n2);//����������  
	   cvNamedWindow("img2_Feat", 0);//��������
	   cvShowImage("img2_Feat", img2_Feat);//��ʾ  
	   cvResizeWindow("img2_Feat", 600, 350);//��С���ڵĴ�С
	   cvSaveImage(imgtwo, img2_Feat);//����ͼ��imgoneָ����Ŀ¼
	
	   Point pt1, pt2;//���ߵ������˵�  
	   double d0, d1;//feat1��ÿ�������㵽����ںʹν��ڵľ���  
	   matchNum = 0;//�������ֵ��ɸѡ���ƥ���Եĸ���  
	
	   //����ͼ2�������㼯feat2����k-d��������k-d������kd_root  
	   kd_root = kdtree_build(feat2, n2);
	
	   //���������㼯feat1�����feat1��ÿ��������feat��ѡȡ���Ͼ����ֵ������ƥ��㣬�ŵ�feat��fwd_match����  
	   for (int i = 0; i < n1; i++)
		    {
		        feat = feat1 + i;//��i���������ָ��  
		        //��kd_root������Ŀ���feat��2������ڵ㣬�����nbrs�У�����ʵ���ҵ��Ľ��ڵ����  
			    int k = kdtree_bbf_knn(kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS);
		        if (k == 2)
			       {
			           d0 = descr_dist_sq(feat, nbrs[0]);//feat������ڵ�ľ����ƽ��  
			           d1 = descr_dist_sq(feat, nbrs[1]);//feat��ν��ڵ�ľ����ƽ��  
		               //��d0��d1�ı�ֵС����ֵNN_SQ_DIST_RATIO_THR������ܴ�ƥ�䣬�����޳�  
				            if (d0 < d1 * NN_SQ_DIST_RATIO_THR)
				            {   //��Ŀ���feat������ڵ���Ϊƥ����  
				                pt1 = Point(cvRound(feat->x), cvRound(feat->y));//ͼ1�е������  
			                    pt2 = Point(cvRound(nbrs[0]->x), cvRound(nbrs[0]->y));//ͼ2�е������(feat������ڵ�)  
								pt2.x += img1->width;
								//pt2.y += img1->height;//��������ͼ���������еģ�pt2�����������ͼ1�ĸ߶ȣ���Ϊ���ߵ��յ�  
				                cvLine(mystacked, pt1, pt2, CV_RGB(255, 0, 0), 1, 8, 0);//��������  
			                    matchNum++;//ͳ��ƥ���Եĸ���  
				                feat1[i].fwd_match = nbrs[0];//ʹ��feat��fwd_match��ָ�����Ӧ��ƥ���  
				            }
			       }
		        free(nbrs);//�ͷŽ�������  
		    }
	    cout<<"��ŷʽ�����ֵ��ɸѡ���ƥ���Ը�����"<< matchNum << endl;
	    //��ʾ�����澭�����ֵ��ɸѡ���ƥ��ͼ  
		//cvNamedWindow("IMG_MATCH1",1);//��������  
	  //  cvShowImage("IMG_MATCH1", mystacked);//��ʾ  
		cvSaveImage(match, mystacked);
		//����RANSAC�㷨ɸѡƥ���,����任����H��������ڱ任����H��˵������ͼ���ص��Ĳ��֣�����ƴ��  
		CvMat * H = ransac_xform(feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01, homog_xfer_err, 3.0, &inliers, &n_inliers);
	    cout<< "��RANSAC�㷨ɸѡ���ƥ���Ը�����" << n_inliers << endl;
        //������RANSAC�㷨ɸѡ��������㼯��inliers���ҵ�ÿ���������ƥ��㣬��������  
		   for (int i = 0; i<n_inliers; i++)
		    {
		       feat = inliers[i];//��i��������  
		       pt1 = Point(cvRound(feat->x), cvRound(feat->y));//ͼ1�е������  
		       pt2 = Point(cvRound(feat->fwd_match->x), cvRound(feat->fwd_match->y));//ͼ2�е������(feat��ƥ���)  
		       //cout<< "(" << pt1.x << "," << pt1.y << ")--->(" << pt2.x << "," << pt2.y << ")" << endl;
			   pt2.x += img1->width;
			   //pt2.y += img1->height;//��������ͼ���������еģ�pt2�����������ͼ1�ĸ߶ȣ���Ϊ���ߵ��յ�  
		       cvLine(mystacked_ransac, pt1, pt2, CV_RGB(255, 0, 0), 1, 8, 0);//��������  	   
		   }
          cvNamedWindow("IMG_MATCH2",0);//��������
          cvResizeWindow("IMG_MATCH2",800,250);//��С���ڵĴ�С
          cvShowImage("IMG_MATCH2", mystacked_ransac);//��ʾ
          cvSaveImage(ransac, mystacked_ransac);
	}


	
//������ 
int main()
{
	cout << "׼������" << endl;
	        //char * filePath = "D://8ƪ�������//ͼ�����ݿ�//Google Earth//NEW_DATABASE//newdeli//01.png";
	        //����ͼ��ָ��img1ָ���������Ұ����Ĵ�С�²�����ָ���ķֱ��ʴ�С
			IplImage *img1 = cvLoadImage("D://8ƪ�������//ͼ�����ݿ�//Google Earth//NEW_DATABASE//kalaqi//01.png", 1);
			IplImage* imgresize1 = cvCreateImage(cvSize(800, 500), 8, 3);//ָ��ͨ����Ϊ3��ָ���ֱ���Ϊ800*500
			cvResize(img1, imgresize1, CV_INTER_NN);

			//����ͼ��ָ��img2ָ���������Ұ����Ĵ�С�²�����ָ���ķֱ��ʴ�С
			IplImage *img2 = cvLoadImage("D://8ƪ�������//ͼ�����ݿ�//Google Earth//NEW_DATABASE//kalaqi//02.png", 1);
			IplImage* imgresize2 = cvCreateImage(cvSize(800, 500), 8, 3);//ָ��ͨ����Ϊ3��ָ���ֱ���Ϊ800*500
			cvResize(img2, imgresize2, CV_INTER_NN);

			if (img1 == NULL || img2 == NULL){
				cout << "����ͼƬʧ�ܣ�ͼƬ�����ڻ��ڲ�����" << endl;
				system("pause");
				return -1;
			}
			cout << "�Ѿ���������ͼ��" << endl << "׼��ƥ��" << endl;
			match(imgresize1, imgresize2);
			cout << "------------------------------------------------------" << endl;
			
			
	waitKey(0);
	system("pause");
	return 0;
}
	//����ͼ�� 
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
	//��һ������SIFT���Ӽ��ؼ��� 

	SiftFeatureDetector detector;   //���캯������Ĭ�ϵ�   
	std::vector<KeyPoint> keypoints_object, keypoints_scene;      //����2��ר���ɵ���ɵĵ����������洢������   
	detector.detect(img_object, keypoints_object);  //��img_objectͼ���м�⵽��������洢��������keypoints_object�� 
	//cout << keypoints_object.capacity << endl;
	detector.detect(img_scene, keypoints_scene);    //ͬ��   


	//��ͼ���л��������� 

	Mat img_keypoints_object, img_keypoints_scene;
	//�����ڴ��л��� 

	drawKeypoints(img_object, keypoints_object, img_keypoints_object, Scalar::all(-1), DrawMatchesFlags::DEFAULT);//���ڴ��л���������   
	drawKeypoints(img_scene, keypoints_scene, img_keypoints_scene, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	//����ʾ 

	imshow("sift_keypoints_object", img_keypoints_object);
	imshow("sift_keypoints_scene", img_keypoints_scene);

	//�ڶ����������������� 

	SiftDescriptorExtractor extractor;  //���������Ӷ���   
	Mat descriptors_1, descriptors_2;        //������������ľ���   
	//������������ 

	extractor.compute(img_object, keypoints_object, descriptors_1);
	extractor.compute(img_scene, keypoints_scene, descriptors_2);

	//����������FLANN����ƥ���������� 

	FlannBasedMatcher matcher;  //����һ��FlannBasedMatcher����   
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	double max_dist = 0;
	double min_dist = 100;

	//�����������������С���� 

	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//ֻ�����õ�ƥ�䣺���������С��3����С���� ,RANSAC�㷨

	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//���ڴ��л��Ʊ����ĺõ�ƥ�� 

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//�ڳ����ж�λĿ��ͼ�� 

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	std::cout << "good_matches.size(): " << good_matches.size() << "\n";
	for (int i = 0; i < good_matches.size(); i++)
	{
		//�Ӻõ�ƥ�����ҵ������� 

		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	//����ƥ��Ĺؼ����ҳ���Ӧ�ı任 

	cv::Mat H = cv::findHomography(cv::Mat(obj), cv::Mat(scene), CV_RANSAC);

	//�ҵ�Ŀ��Ľǵ� 

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows);
	obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	//ӳ���Ⱥ���ڳ����л�ȡĿ������� 

	cv::perspectiveTransform(cv::Mat(obj_corners), cv::Mat(scene_corners), H);

	//Ŀ��Ľǵ�֮�����ߣ����Ŀ�꣩ 

	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);

	//��ʾ�����ĺõ�ƥ�� 

	imshow("Good Matches & Object detection - SIFT", img_matches);
	imwrite("Good Matches & Object detection - SIFT.png", img_matches);

	double end = clock();
	cout << "\nSURF - elapsed time is: " << (end - begin) / CLOCKS_PER_SEC * 1000 << " ms\n";
*/



/*
const char* imagename = "D:\\NEW_NO_FOG_DATABASE\\below\\DJI_0050.JPG";

//���ļ��ж���ͼ��
Mat img = imread(imagename);
resize(img, img, Size(img.cols / 10, img.rows / 10));
Mat img2 = imread("D:\\NEW_NO_FOG_DATABASE\\below\\DJI_0051.JPG");
resize(img2, img2, Size(img2.cols / 100, img2.rows / 100));
//�������ͼ��ʧ��
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
//��ʾͼ��
imshow("image before", img);
imshow("image2 before", img2);


//sift�������
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

//�˺����ȴ�������������������ͷ���
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

	//���ļ��ж���ͼ��
	Mat img = imread(imagename);
	resize(img, img, Size(img.cols / 10, img.rows / 10));
	Mat img2 = imread("D:\\NEW_NO_FOG_DATABASE\\below\\DJI_0051.JPG");
	resize(img2, img2, Size(img2.cols / 100, img2.rows / 100));
	//�������ͼ��ʧ��
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
	//��ʾͼ��
	imshow("image before", img);
	imshow("image2 before", img2);


	//sift�������
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

	//�˺����ȴ�������������������ͷ���
	waitKey();
	return 0;
}
*/


/*
ƴ��
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



//ѭ����ȡÿһ��ͼ��������ȡ����ƥ�䣬��󱣴�
/*
//������ 
int main()
{
	
	cout << "׼������" << endl;
	char * filePath = "D://8ƪ�������//ͼ�����ݿ�//Google Earth//NEW_DATABASE//newdeli//";
	vector<string> files;

	////��ȡ��·���µ������ļ�  
	getFiles(filePath, files);
	int size = files.size();
	int i = 0, j = 0;
	for (i = 2; i <3; i++){
		for (j = 3; j < 4; j++)
		{

			cout << "ͼһ��" << files[i].c_str() << endl;
			cout << "ͼ����" << files[j].c_str() << endl;


			IplImage *img1 = cvLoadImage(files[i].c_str(), 1);
			IplImage* imgresize1 = cvCreateImage(cvSize(800, 500), 8, 3);//ָ��ͨ����Ϊ3
			cvResize(img1, imgresize1, CV_INTER_NN);

			IplImage *img2 = cvLoadImage(files[j].c_str(), 1);
			IplImage* imgresize2 = cvCreateImage(cvSize(800, 500), 8, 3);
			cvResize(img2, imgresize2, CV_INTER_NN);

			if (img1 == NULL || img2 == NULL){
				cout << "����ͼƬʧ�ܣ�ͼƬ�����ڻ��ڲ�����" << endl;
				system("pause");
				return -1;
			}
			cout << "�Ѿ���������ͼ��" << endl << "׼��ƥ��" << endl;
			match(imgresize1, imgresize2);
			cout << "------------------------------------------------------" << endl;


		}
	}
	waitKey(0);
	system("pause");
	return 0;
}
*/