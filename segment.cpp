#include "segment.h"

int main(int argc, char** argv) 
{
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

	// Display original image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", imOrig);
	waitKey(0);



	// TODO: break out into a function:
	////////////////////////////////////////////
	// Apply segmentation through thresholding

	// TODO: Which color space is best?
	// convert to LAB ?
	Mat imThreshLAB = imOrig.clone();
//    cvtColor(imOrig, imThreshLAB, CV_RGB2Lab);
//    imshow("LAB image", imThreshLAB);
//    waitKey(0);


	cvtColor(imThreshLAB, imThreshLAB, CV_RGB2Lab);
//	cvtColor(imThreshLAB, imThreshLAB, CV_RGB2YCrCb);
//	cvtColor(imThreshLAB, imThreshLAB, CV_RGB2HSV);

	// 
	Vec3b *modPixel;
	for( int i = 0; i < imThreshLAB.rows; i++ ) {
		for( int j = 0; j < imThreshLAB.cols; j++ ) {

			modPixel = & imThreshLAB.at<Vec3b>(i, j);

			for( int c = 0; c < imThreshLAB.channels(); c++ ) {
				if (modPixel->val[c] <= 128) {
					modPixel->val[c] = 64;
				} else {
					modPixel->val[c] = 192;
				}
			}
		}
	}
	cvtColor(imThreshLAB, imThreshLAB, CV_Lab2RGB);
//	cvtColor(imThreshLAB, imThreshLAB, CV_YCrCb2RGB);
//	cvtColor(imThreshLAB, imThreshLAB, CV_HSV2RGB);
	namedWindow("Threshold Image", WINDOW_AUTOSIZE);
	imshow("Threshold Image", imThreshLAB);
	waitKey(0);


	//










	// Say 'no' to Seg Faults!
//	destroyWindow("Original Image");
//	destroyWindow("Thresh Image");
	destroyAllWindows();

	printf("\n--Done.--\n\n");
	return 0;
}