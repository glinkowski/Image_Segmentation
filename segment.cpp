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







    // Say 'no' to Seg Faults!
    destroyAllWindows();

	printf("\n--Done.--\n");
	return 0;
}