/* ///////////////////////////////
 *
 *	author: Greg Linkowski
 *	
 *	Goal: apply segmentation through basic thresholding
 *
 *	Description:
 *	Treat color space as 3-dimensional. Divide into 8
 *	equal-sized segments (divide with an orthogonal
 *	planes at each midpoint, ie: (:,128,:) for values
 *	ranging from 0 to 255). If a pixel falls within
 *	a region (ex: (20, 30, 205)), paint it with that
 *	region's mean (ex: (64, 64, 192)).
 *
 */ //////////////////////////////



#include "segment.h"

int main(int argc, char** argv) 
{

	////////////////////////////////////////////
	// Verify arguments & load image

	// Error Check: ensure image was specified
	if (argc != 2) {
		printf("No image was specified.\n");
		printf("  ie: ./segment.exe <image_name>\n");
		return -1;
	}

	// Read in the image
	Mat imOrig;
	imOrig = imread(argv[1], 1);

	// Error Check: verify non-empty image
	if (!imOrig.data) {
		printf("The image file is emtpy.\n");
		return -1;
	}


	////////////////////////////////////////////
	// Display original image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", imOrig);
	waitKey(0);



// TODO: break out into a function:
	////////////////////////////////////////////
	// Apply segmentation

// TODO: Which color space is best?
	// convert to LAB ?
	Mat imThreshLAB = imOrig.clone();
//    cvtColor(imOrig, imThreshLAB, CV_RGB2Lab);
//    imshow("LAB image", imThreshLAB);
//    waitKey(0);


//	cvtColor(imThreshLAB, imThreshLAB, CV_RGB2Lab);
//	cvtColor(imThreshLAB, imThreshLAB, CV_RGB2YCrCb);
	cvtColor(imThreshLAB, imThreshLAB, CV_RGB2HSV);

	// 
	Vec3b * modPixel;
	for ( int i = 0; i < imThreshLAB.rows; i++ ) {
		for ( int j = 0; j < imThreshLAB.cols; j++ ) {

			modPixel = & imThreshLAB.at<Vec3b>(i, j);

			for ( int c = 0; c < imThreshLAB.channels(); c++ ) {
				if (modPixel->val[c] <= 128) {
					modPixel->val[c] = 64;
				} else {
					modPixel->val[c] = 192;
				}
			}
		}
	}
//	cvtColor(imThreshLAB, imThreshLAB, CV_Lab2RGB);
//	cvtColor(imThreshLAB, imThreshLAB, CV_YCrCb2RGB);
	cvtColor(imThreshLAB, imThreshLAB, CV_HSV2RGB);
	namedWindow("Threshold Image", WINDOW_AUTOSIZE);
	imshow("Threshold Image", imThreshLAB);
	waitKey(0);



	// Say 'no' to Seg Faults!
//	destroyWindow("Original Image");
//	destroyWindow("Threshold Image");
	destroyAllWindows();


	printf("\n--Done.--\n\n");
	return 0;
}