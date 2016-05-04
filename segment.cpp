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

	// downsize large images
	if ((imOrig.rows > 720) | (imOrig.cols > 1280)) {
		resize(imOrig, imOrig, Size(640, 480), 0, 0, INTER_AREA);
	}

	// Display original image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", imOrig);
	waitKey(0);



// TODO: add color space to the run arguments ?
	uint8_t cSpace = 3;



	//////////////// //////////////// ////////////////
	// Apply segmentation through basic thresholding
	Mat imThresh = segThresh(& imOrig, cSpace);

	namedWindow("Threshold Image", WINDOW_AUTOSIZE);
	imshow("Threshold Image", imThresh);
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
	cvtColor(imKMeans, imKMeans, CV_RGB2Lab);
//	cvtColor(imKMeans, imKMeans, CV_RGB2YCrCb);

	uint8_t K = 7;
	uint8_t dimensions = 3;	// either 3 or 5
	uint8_t stopError = 5;
	uint8_t stopCount = 25;

	// Create the array to hold K centroids
//	uint8_t cLen = K * dimensions;

/*	int ** centroids;
	centroids = malloc(K * sizeof(int*));
	for (int i = 0; i < K; i++) {
		centroids[i] = malloc(dimensions * sizeof(int));
	}
*/
//	int centroids[8][3];

	uint8_t * centroids, * centPrev;
//	centroids = (char *) malloc( K * 3 * sizeof(char) );
	centroids = (uint8_t *) malloc( K * dimensions * sizeof(uint8_t));
	centPrev = (uint8_t *) malloc( K * dimensions * sizeof(uint8_t));

//	centroids = (unsigned char *) malloc( cLen * sizeof(unsigned char) );
//	char * centColor;
//	centColor = (char *) malloc( K * 3 * sizeof(char) );
	float * centSum;
//	centSum = (float *) calloc( K * dimensions * sizeof(float) );
	centSum = (float *) calloc( K * dimensions, sizeof(float) );
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




// TODO : access as pointer, not .at
// http://stackoverflow.com/questions/1844736/accesing-a-matrix-element-in-the-mat-object-not-the-cvmat-object-in-opencv-c

	// perform the K-means 
	uint8_t error = stopError + 1;
	uint8_t tempErr;
	uint8_t runs = 0;
	uint8_t label = 0;
	float distToBeat, newDist;
	float xScale, yScale;
	uint8_t xNew, yNew;
//	uint8_t winner;
	Vec3b * getPixel;
//	int tempX, tempY;

	xScale = 255.0 / width;
	yScale = 255.0 / height;
	printf("%d, %d\n", width, height);
	printf("%f, %f\n", xScale, yScale);

/*	// Select K random centroids
	uint8_t idx;
	for (uint8_t i = 0; i < K; i++) {
		for (uint8_t j = 0; j < dimensions; j++) {

			idx = (i * dimensions) + j;
			centroids[idx] = rand() % 255;
//			printf("%d\n", (uint8_t)centroids[idx]);

		}
	}
*/	

	// Select K random centroids
	uint8_t idx;
	int xTemp, yTemp;
	for (uint8_t i = 0; i < K; i++) {
		// xNew = rand() % 255;
		// yNew = rand() % 255;
		// getPixel = & imKMeans.at<Vec3b>((int) yNew/yScale, (int) xNew/xScale);

		xTemp = rand() % width;
		yTemp = rand() % height;
		getPixel = & imKMeans.at<Vec3b>(yTemp, xTemp);
		for (uint8_t j = 0; j < 3; j++) {
			idx = (i * dimensions) + j;
			centroids[idx] = getPixel->val[j];
		}
		if (dimensions == 5) {
			xNew = xTemp * xScale;
			printf("%d\n", xNew);
			centroids[idx+1] = (uint8_t) (xTemp * xScale);
			centroids[idx+2] = (uint8_t) ((float) yTemp * yScale);
		}

	}

// REMOVE: print the centroids
	for (uint8_t i = 0; i < K; i++) {
		printf("centroid %d: ", i);
		for (uint8_t j = 0; j < dimensions; j++) {
			idx = j + (i * dimensions);
			printf(" %d ", centroids[idx]);
		}
		printf("\n");
	}

	while ((error > stopError) & (runs < stopCount)) {
		printf("Run: %d\n", runs);

		// TODO: calc stopError

		// Clear centSum, centCount
		for (uint8_t i = 0; i < K; i++) {
			for (uint8_t j = 0; j < dimensions; j++) {
				idx = j + (i * dimensions);
				centSum[idx] = 0;
				centCount[i] = 0;
			}
		}

		for (int x = 0; x < width; x++){
			for (int y = 0; y < height; y++){

				getPixel = & imKMeans.at<Vec3b>(y, x);
//				printf("got pixel\n");

				// measure (squared) dist to first centroid
				distToBeat = 16581375;

				// Determine which centroid is closest to this pixel
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
						xNew = x * xScale;
						yNew = y * yScale;
						newDist += (xNew - centroids[idx + 1]) *
							(xNew - centroids[idx + 1]);
						newDist += (yNew - centroids[idx + 2]) *
							(yNew - centroids[idx + 2]);
					}
					// test this dist against winner
					// save the label of the winning centroid
					if (newDist < distToBeat) {
						distToBeat = newDist;
//						printf("newDist wins, label: %d, newDist: %f\n", i, newDist);
						// TODO: doesn't like this line?
						pixLabels.at<uint8_t>(y, x) = i;
//						pixLabels.at<unsigned char>(y, x) = i;
//						printf("got here\n");
					}
				}

				// Update the centroid values (1: sum of members)
				label = pixLabels.at<uint8_t>(y, x);
				for (uint8_t j = 0; j < 3; j++) {
					idx = j + (label * dimensions);
					centSum[idx] += getPixel->val[j];
				}
				if (dimensions == 5) {
					xNew = x * xScale;
					centSum[idx + 1] += xNew;
					yNew = y * yScale;
					centSum[idx + 2] += yNew;
				}
				centCount[label] += 1;
			//TODO: if centroid is all zeros, choose new one ?
				// ?? Why do they zero out ??

		// 	/*	if (dimensions == 5) {
		// 			// TODO: normalize x/y over 255?
		// 			centSum[idx + 1] += x;
		// 			centSum[idx + 2] += y;
		// 		}*/

				// done with this pixel
			}
		}
		runs++;

		/*// REMOVE: print the centroids
		for (uint8_t i = 0; i < K; i++) {
			printf("centroid %d: ", i);
			for (uint8_t j = 0; j < dimensions; j++) {
				idx = j + (i * dimensions);
				printf("\t%d ", centroids[idx]);
			}
			printf("\n");
		}
*/
		// TODO: combine these loops ...

		// Update centroid values
		for (uint8_t i = 0; i < K; i++) {
			for (uint8_t j = 0; j < dimensions; j++) {
				idx = j + (i * dimensions);
				centroids[idx] = (uint8_t) (centSum[idx] / centCount[i]);
			}
		}

		// Calculate the error (simple)
		error = 0;
		for (uint8_t i = 0; i < (K * dimensions); i++) {
			// get error magnitude on only this dimension
			tempErr = centroids[i] - centPrev[i];
			if (tempErr < 0)
				tempErr = tempErr * -1;
			// save as new error if larger than prev
			if (tempErr > error)
				error = tempErr;

			// Save centroids for the next round
			centPrev[i] = centroids[i];
		}

	}

	printf("Found the K=%d centroids\n", K);

	// // Calculate average values for each K
	// for (int x = 0; x < width; x++){
	// 	for (int y = 0; y < height; y++){

	// 		getPixel = & imKMeans.at<Vec3b>(y, x);

	// 		// which label is this pixel?
	// 		int i = pixLabels.at<uint8_t>(y, x);

	// 		// calculate average (first: sum)
	// 		for (int j = 0; j < 3; j ++) {
	// 			idx = j + (i * dimensions);
	// 			centSum[idx] += getPixel->val[j];
	// 		}
	// 	/*	if (dimensions == 5) {
	// 			// TODO: normalize x/y over 255?
	// 			centSum[idx + 1] += x;
	// 			centSum[idx + 2] += y;
	// 		}*/
	// 		// done with this pixel
	// 	}
	// }

	// // REMOVE: print the current values
	// for (int i = 0; i < K; i++) {
	// 	printf("%f\n", centCount[i]);
	// 	for (int j = 0; j < dimensions; j++) {
	// 		idx = j + (i * dimensions);

	// 		printf(" %f ", centSum[idx]);

	// 	}
	// 	printf("\n");
	// }

	// // calculate average (second: divide)
	// for (int i = 0; i < K; i++) {
	// 	for (int j = 0; j < 3; j++) {
	// 		idx = j + (i * dimensions);
	// 		centSum[idx] = centSum[idx] / centCount[i];
	// 	}
	// }

	// TODO: output values from centSum, centCount


// REMOVE: print the centroids
	for (uint8_t i = 0; i < K; i++) {
		printf("centroid %d: ", i);
		for (uint8_t j = 0; j < dimensions; j++) {
			idx = j + (i * dimensions);
			printf(" %d ", centroids[idx]);
		}
		printf("\n");
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

			for (int j = 0; j < 3; j++) {
				idx = j + (thisLabel * dimensions);

//				getPixel->val[j] = 64;
				getPixel->val[j] = centroids[idx];
			}
			// done with this pixel


		}
	}

	printf("Painted the output image\n");

	cvtColor(imKMeans, imKMeans, CV_Lab2RGB);
//	cvtColor(imKMeans, imKMeans, CV_YCrCb2RGB);

	namedWindow("KMeans Image", WINDOW_AUTOSIZE);
	imshow("KMeans Image", imKMeans);
	waitKey(0);


//	printf("Painted the output image\n");
//TODO: free stuff
	free (centroids);
	free (centPrev);
	free (centSum);
	free (centCount);
//	printf("Painted the output image\n");



//	try {
		imwrite("imThresh.png", imThresh);
		imwrite("imKMeans.png", imKMeans);
//	}
//	catch (runtime_error& err) {
//		fprintf(stderr, "Exception converting image to PNG format: %s\n", err.what());
//		return 1;
//	}





	// Say 'no' to Seg Faults!
	destroyWindow("Original Image");
	destroyWindow("Threshold Image");
	destroyWindow("KMeans Image");
//	destroyAllWindows();

	printf("\n--Done.--\n\n");
	return 0;
}