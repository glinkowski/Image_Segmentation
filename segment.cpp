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
	Vec3b * modPixel;
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




	////////////////////////////////////////////
	// Apply segmentation through k-means

	// choose k, error
	// create array to hold labels
	// create array to hold centroids
	// arrays to hold cent. sums & counts
	//	to calculate average at the end
	// randomly choose k centroids
	// while error above XX
		// for each pixel
			// measure dist, label each pixel
			// increment appropriate centroid count
			// add to the centroid sum
			// track highest error
	// calculate new centroids



	// K: 8
	// dimensions: 3 colors (+ 2 directions?)
	// error = 

	uint8_t K = 8;
	uint8_t dimensions = 3;	// either 3 or 5
	uint8_t stopError = 5;
	uint8_t stopCount = 10;

	// Create the array to hold K centroids
	uint8_t cLen = K * dimensions;

/*	int ** centroids;
	centroids = malloc(K * sizeof(int*));
	for(int i = 0; i < K; i++) {
		centroids[i] = malloc(dimensions * sizeof(int));
	}
*/
//	int centroids[8][3];

	char * centroids;
	centroids = (char *) malloc( cLen * sizeof(char) );
//	char * centColor;
//	centColor = (char *) malloc( K * 3 * sizeof(char) );
	float * centSum;
	centSum = (float *) malloc( K * 3 * sizeof(float) );
	float * centCount;
	centCount = (float *) malloc( K * 3 * sizeof(float) );
//	float * centDist;
//	centDist = (float *) malloc( K * sizeof(float) );

//	centroids[8] = 0;
//	printf("%d\n", centroids[8]);

	// the array to hold pixel labels
	Mat pixLabels(imOrig.rows, imOrig.cols, CV_8UC1, 0);

	// get dimensions
//	Mat imKMeans = imOrig.clone();
	int height = imOrig.rows;
	int width = imOrig.cols;

	// Select K random centroids
	uint8_t idx;
	for(uint8_t i = 0; i < K; i++) {
		for(uint8_t j = 0; j < dimensions; j++) {

			idx = (i * dimensions) + j;
			centroids[idx] = rand() % 255;
//			printf("%d\n", (uint8_t)centroids[idx]);

		}
	}


	// perform the K-means 
	float error = stopError + 1;
	uint8_t runs = 0;
	float dist, newDist;
	uint8_t winner;
	Vec3b * getPixel;
//	int tempX, tempY;
	while ((error > stopError) & (runs < stopCount)) {
		for(int x = 0; x < width; x++){
			for(int y = 0; y < height; y++){

				getPixel = & imOrig.at<Vec3b>(y, x);

				// measure (squared) dist to first centroid
				dist = 16581375;

				for(int i = 0; i < K; i++) {
					// calculate the squared dist
					newDist = 0;
					for(int j = 0; j < 3; j ++) {
						idx = j + (i * dimensions);
						newDist += (getPixel->val[j] - centroids[idx]) *
							(getPixel->val[j] - centroids[idx]);
					}
					if (dimensions == 5) {
						newDist += (x - centroids[idx + 1]) * (x - centroids[idx + 1]);
						newDist += (y - centroids[idx + 2]) * (y - centroids[idx + 2]);
					}
					// test this dist against winner
					// save the label of the winning centroid
					if (newDist < dist) {
						dist = newDist;
						pixLabels.at<uint8_t>(y, x) = i;
					}
				}
				// done with this pixel
			}
		}
		runs++;
	}

//TODO: calculate avg values for K (up to dimension 3)




//TODO: free stuff
	free (centroids);
	free (centSum);
	free (centCount);





	// Say 'no' to Seg Faults!
	destroyWindow("Original Image");
	destroyWindow("Threshold Image");
//	destroyAllWindows();

	printf("\n--Done.--\n\n");
	return 0;
}