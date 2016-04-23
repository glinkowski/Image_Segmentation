
#include <stdio.h>
#include <opencv2/opencv.hpp>

//#include <stdlib.h>
//#include <assert.h>
//#include <float.h>
//#include <math.h>

//using namespace std;
using namespace cv;




//////////////// //////////////// ////////////////
// Apply segmentation through basic thresholding
Mat segThresh(Mat * imOrig, uint8_t cSpace) {
// TODO: ...more explanation ...

	Mat imThresh = imOrig->clone();
	

	// Convert to chosen color space
	switch (cSpace) {
		case 1 :
			// Leave in RGB color space
			break;
		case 2 :
			cvtColor(imThresh, imThresh, CV_RGB2Lab);
			break;
		case 3 :
			cvtColor(imThresh, imThresh, CV_RGB2YCrCb);
			break;
		case 4 :
			cvtColor(imThresh, imThresh, CV_RGB2HSV);
			break;
		default :
			break;
	}
	// OPINION: RGB is so-so; Lab & YCrCb better,
	//		HSV requires a different approach (could be interesting)


	// Paint the new image according to cutoffs
	Vec3b * modPixel;
	for ( int i = 0; i < (imThresh.rows); i++ ) {
		for ( int j = 0; j < imThresh.cols; j++ ) {

			modPixel = & imThresh.at<Vec3b>(i, j);

			for ( int c = 0; c < imThresh.channels(); c++ ) {
				if (modPixel->val[c] <= 128) {
					modPixel->val[c] = 64;
				} else {
					modPixel->val[c] = 192;
				}
			}
		}
	}


	// Convert back to RGB color space
	switch (cSpace) {
		case 1 :
			// Leave in RGB color space
			break;
		case 2 :
			cvtColor(imThresh, imThresh, CV_Lab2RGB);
			break;
		case 3 :
			cvtColor(imThresh, imThresh, CV_YCrCb2RGB);
			break;
		case 4 :
			cvtColor(imThresh, imThresh, CV_HSV2RGB);
			break;
		default :
			break;
	}

	return imThresh;
}