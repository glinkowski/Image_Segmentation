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

	Mat imKMeans = imOrig.clone();

	uint8_t K = 8;
	uint8_t dimensions = 3;	// either 3 or 5
	uint8_t stopError = 5;
	uint8_t stopCount = 10;

	// Create the array to hold K centroids
	uint8_t cLen = K * dimensions;

/*	int ** centroids;
	centroids = malloc(K * sizeof(int*));
	for (int i = 0; i < K; i++) {
		centroids[i] = malloc(dimensions * sizeof(int));
	}
*/
//	int centroids[8][3];

	char * centroids;
	centroids = (char *) malloc( cLen * sizeof(char) );
//	char * centColor;
//	centColor = (char *) malloc( K * 3 * sizeof(char) );
	float * centSum;
//	centSum = (float *) calloc( K * dimensions * sizeof(float) );
	centSum = (float *) calloc( K * 3, sizeof(float) );
	float * centCount;
	centCount = (float *) calloc( K, sizeof(float) );
//	float * centDist;
//	centDist = (float *) malloc( K * sizeof(float) );

//	centroids[8] = 0;
//	printf("%d\n", centroids[8]);

	// the array to hold pixel labels
//	Mat pixLabels(imKMeans.rows, imKMeans.cols, CV_8UC1, 0);
	Mat pixLabels;
	pixLabels = Mat::zeros(imKMeans.rows, imKMeans.cols, CV_8UC1);

	// get dimensions
//	Mat imKMeans = imKMeans.clone();
	int height = imKMeans.rows;
	int width = imKMeans.cols;

	// Select K random centroids
	uint8_t idx;
	for (uint8_t i = 0; i < K; i++) {
		for (uint8_t j = 0; j < dimensions; j++) {

			idx = (i * dimensions) + j;
			centroids[idx] = rand() % 255;
//			printf("%d\n", (uint8_t)centroids[idx]);

		}
	}


// TODO : access as pointer, not .at
// http://stackoverflow.com/questions/1844736/accesing-a-matrix-element-in-the-mat-object-not-the-cvmat-object-in-opencv-c

	// perform the K-means 
	float error = stopError + 1;
	uint8_t runs = 0;
	float dist, newDist;
//	uint8_t winner;
	Vec3b * getPixel;
//	int tempX, tempY;
	while ((error > stopError) & (runs < stopCount)) {
		printf("Run: %d\n", runs);

		for (int x = 0; x < width; x++){
			for (int y = 0; y < height; y++){

				getPixel = & imKMeans.at<Vec3b>(y, x);
//				printf("got pixel\n");

				// measure (squared) dist to first centroid
				dist = 16581375;

				for (uint8_t i = 0; i < K; i++) {
					// calculate the squared dist
					newDist = 0;
					for (uint8_t j = 0; j < 3; j ++) {
						idx = j + (i * dimensions);
						newDist += (getPixel->val[j] - centroids[idx]) *
							(getPixel->val[j] - centroids[idx]);
					}
//					printf("newDist: %f\n", newDist);
					if (dimensions == 5) {
						newDist += (x - centroids[idx + 1]) * (x - centroids[idx + 1]);
						newDist += (y - centroids[idx + 2]) * (y - centroids[idx + 2]);
					}
					// test this dist against winner
					// save the label of the winning centroid
					if (newDist < dist) {
						dist = newDist;
//						printf("newDist wins, label: %d\n", i);
						// TODO: doesn't like this line?
						pixLabels.at<uint8_t>(y, x) = i;
//						pixLabels.at<unsigned char>(y, x) = i;
//						printf("got here\n");
					}
				}
				// done with this pixel
			}
		}
		runs++;
	}

	printf("Found the K centroids\n");

	// Calculate average values for each K
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){

			getPixel = & imKMeans.at<Vec3b>(y, x);

			// which label is this pixel?
			int i = pixLabels.at<uint8_t>(y, x);

			// calculate average (first: sum)
			for (int j = 0; j < 3; j ++) {
				idx = j + (i * dimensions);
				centSum[idx] += getPixel->val[j];
			}
		/*	if (dimensions == 5) {
				// TODO: normalize x/y over 255?
				centSum[idx + 1] += x;
				centSum[idx + 2] += y;
			}*/
			// done with this pixel
		}
	}
	// calculate average (second: divide)
	for (int i = 0; i < K; i++) {
		for (int j = 0; j < 3; j++) {
			idx = j + (i * dimensions);
			centSum[idx] = centSum[idx] / centCount[i];
		}
	}

	printf("Calculated average values\n");

	// Fnally: Place the new pixel value into the image
	uint8_t thisLabel;
	for (int x = 0; x < width; x++){
		for (int y = 0; y < height; y++){

			// Read the label for this pixel
			thisLabel = pixLabels.at<uint8_t>(y, x);

			// Place the new value
			getPixel = & imKMeans.at<Vec3b>(y, x);

			for (int i = 0; i < K; i++) {
				for (int j = 0; j < 3; j ++) {
					idx = j + (i * dimensions);
					getPixel->val[j] = centSum[idx];
				}
			}
			// done with this pixel
		}
	}

	printf("Painted the output image\n");

	namedWindow("KMeans Image", WINDOW_AUTOSIZE);
	imshow("KMeans Image", imKMeans);


//TODO: free stuff
	free (centroids);
	free (centSum);
	free (centCount);





	// Say 'no' to Seg Faults!
	destroyWindow("Original Image");
	destroyWindow("Threshold Image");
	destroyWindow("KMeans Image");
//	destroyAllWindows();

	printf("\n--Done.--\n\n");
	return 0;
}