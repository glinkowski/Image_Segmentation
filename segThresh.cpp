/* //////////////// //////////////// ////////////////
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
 */ //////////////// //////////////// ////////////////



#include "segment.h"

int main(int argc, char** argv) 
{

	//////////////// //////////////// ////////////////
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


	//////////////// //////////////// ////////////////
	// Downsize extra-large images
	if ((imOrig.rows > 720) | (imOrig.cols > 1280)) {
		resize(imOrig, imOrig, Size(640, 480), 0, 0, INTER_AREA);
	}


	//////////////// //////////////// ////////////////
	// Display original image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", imOrig);
	waitKey(0);


	// Examine different color spaces
	char winName[17];
	for(int i = 1; i < 5; i++) {

		//////////////// //////////////// ////////////////
		// Apply segmentation through basic thresholding
		Mat imThresh = segThresh(& imOrig, i);

		// display the results
		sprintf(winName, "Threshold Image %d", i);
		namedWindow(winName, WINDOW_AUTOSIZE);
		imshow(winName, imThresh);
		waitKey(0);
	}


	// Say 'no' to Seg Faults!
	destroyAllWindows();


	printf("\n--Done.--\n\n");
	return 0;
}