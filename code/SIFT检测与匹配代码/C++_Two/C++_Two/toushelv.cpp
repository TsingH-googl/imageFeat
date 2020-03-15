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


void mygetFiles(string path, vector<string>& files)
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
					mygetFiles(p.assign(path).append("//").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("//").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
int toushelvmain(){

	string filePath= "F://Databaseonewithsky//alldownsample//";


		vector<string> files;
		struct feature *feat1;

		////��ȡ��·���µ������ļ�  
		mygetFiles(filePath, files);
		string filename = "F://Databaseonewithsky//alldownsample.txt";
		FILE *f = fopen(filename.c_str(), "w+");//the txt file which we write to the disk
		int file_num = files.size();
		int *data = new int[file_num];//get data
		int i = 0;
		IplImage *timg;
		for (i = 0; i < file_num; i++){
			cout << files[i].c_str() << endl;
			timg = cvLoadImage(files[i].c_str(), CV_LOAD_IMAGE_COLOR);
			int n1 = sift_features(timg, &feat1);//���ͼ1�е�SIFT������,n1��ͼ1����������� 
			cout << "after dehaze�����������" << n1 << endl;
			data[i] = n1;
			/*		if ((i + 1) % 13 == 0)
			printf( "-------------------------------------------\n");*/
	
		}
		
		for (i = 0; i < file_num; i++){
			//if ((i) % 13 == 0){
			//	fprintf(f, "%s", "n");
			//	//if( ((i / 13) + 1)<10)
			//	//	fprintf(f, "%d", 0);
			//	//
			//	    fprintf(f, "%d\t", (i / 13) + 1);
			//}

			fprintf(f, "%d", data[i]);

			/*	if ((i+1)%13==0)*/
			fprintf(f, "\n");

		}
		delete data;
		fclose(f);

	//fclose(f);
	waitKey(0);
	system("pause");
	return 0;
}