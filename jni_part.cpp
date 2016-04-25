/* //////////////// //////////////// ////////////////
 *
 * project: ECE 420, Lab Final
 *
 * authors:
 *    Greg Linkowski
 *
 * purpose:
 *    image segmentation on Android
 *    convert between YUV & RGB
 *	  perform thresholding seg
 *	  perform k-means segmentation
 *
 */ //////////////// //////////////// ////////////////



#include <jni.h>
#include <opencv2/core/core.hpp>
#include <android/log.h>

using namespace std;
using namespace cv;

int TOTAL;
int THISPIXEL;

extern "C" {


// //////////////// //////////////// ////////////////
//	function declaration for manual color conversion
//int convertYUVtoARGB(int, int, int);

// //////////////// //////////////// ////////////////
//	pixel-by-pixel color conversion, YUV -> RGB
int32_t convertYUVtoARGB(int y, int u, int v) {

	float rTmp = y + (1.370705 * (v-128));
	float gTmp = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
	float bTmp = y + (1.732446 * (u-128));

	char buf[128];

	int red;
	int green;
	int blue;
	int max = 200;
	int min = 25;

	// Saturate the red channel
	if(rTmp > 255)
		red = 255;
	else if(rTmp < 0)
		red = 0;
	else
		red = rTmp;

	// Saturate the green channel
	if(gTmp > 255)
			green = 255;
	else if(gTmp < 0)
		green = 0;
	else
		green = gTmp;

	// Saturate the blue channel
	if(bTmp > 255)
		blue = 255;
	else if(bTmp < 0)
		blue = 0;
	else
		blue = bTmp;

	// Create the 32-bit output pixel (A R G B, where A = 255?)
	int32_t out_pix = 255; // NOTE: This CANNOT be zero! In fact, must be 255.
	out_pix = out_pix << 8;
	out_pix = out_pix | red;
	out_pix = out_pix << 8;
	out_pix = out_pix | green;
	out_pix = out_pix << 8;
	out_pix = out_pix | blue;

	//    sprintf(buf,"RGB= %d, %d, %d; YUV= %d, %d, %d", red,green, blue, y, u, v);
	//    __android_log_write(ANDROID_LOG_INFO, "RGB", buf);

	return out_pix;
}


// //////////////// //////////////// ////////////////
//	entire frame color conversion, YUV -> RGB
JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_YUV2RGB(JNIEnv*, jobject, jlong addrYuv, jlong addrRgba)
{
    /*************** INSERT CODE TO CONVERT AN ENTIRE IMAGE FROM FROM YUV420sp TO ARGB ******************/
    Mat* pYUV=(Mat*)addrYuv;
    Mat* pRGB=(Mat*)addrRgba;

    char buf[128];

    //cv::Size yuvSize = pYUV.size();
    int yuvHeight = pYUV->rows * 2 / 3;
    int yuvWidth = pYUV->cols;
    int yuvTotal = yuvWidth * yuvHeight;
    TOTAL = yuvTotal;

    for(int y = 0; y < yuvHeight; y++) {
        for(int x = 0; x < yuvWidth; x++) {
    		THISPIXEL = y * yuvWidth + x;

    		int block_x = x/2;
    		int block_y = y/2;

    		int Y = 0;
    		Y = Y | pYUV->at<uint8_t>(y,x);

    		//int Upos = 	(y / 2) * (yuvWidth / 2) + (x / 2)  + yuvTotal;
    		int Upos = yuvTotal + block_y*yuvWidth+block_x*2;
    		int U = 0;
    		U = U | pYUV->at<uint8_t>( (Upos / yuvWidth), (Upos % yuvWidth) );
//    		U = U - 128;
//    		U = 4 * U;

    		//int Vpos = (y / 2) * (yuvWidth / 2) + (x / 2) + yuvTotal + (yuvTotal / 4);
    		int Vpos = yuvTotal + block_y*yuvWidth+block_x*2 + 1;
    		int V = 0;
    		V = V | pYUV->at<uint8_t>( (Vpos / yuvWidth), (Vpos % yuvWidth) );
//    		V = V - 128;
//    		V = 4 * V;

//    		sprintf(buf,"YUV= %d, %d, %d", Y, U, V);
//    		__android_log_write(ANDROID_LOG_INFO, "preConversion", buf);

    		pRGB->at<int32_t>(y,x) = convertYUVtoARGB(Y, U, V);
    	}
    }
}



//TODO: fix func header
// //////////////// //////////////// ////////////////
//	k-means Image Segmentation
JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_KMeans(JNIEnv*, jobject, jlong addrYuv, jlong addrRgba)
{
	// 
//	Mat imKMeans = imOrig.clone();
//	cvtColor(imKMeans, imKMeans, CV_RGB2Lab);

//	Mat imKMeans 
  //  		pRGB->at<int32_t>(y,x) = convertYUVtoARGB(Y, U, V);

	// Input & Output images
//	Mat* pYUV=(Mat*)addrYuv;
	Mat * imKMeans = (Mat *) addrRgba;


	// Parameters
	uint8_t K = 8;
	uint8_t dimensions = 5;	// either 3 or 5
	uint8_t stopError = 5;
	uint8_t stopCount = 10;
	uint8_t idx;			// for stepping through centroids


	// Declare centroid spaces
	uint8_t * centroids, * centPrev;
	centroids = (uint8_t *) malloc( K * dimensions * sizeof(uint8_t));
	centPrev = (uint8_t *) malloc( K * dimensions * sizeof(uint8_t));
	float * centSum, * centCount;
	centSum = (float *) calloc( K * 3, sizeof(float) );
	centCount = (float *) calloc( K, sizeof(float) );

	// array to hold pixel labels
	Mat pixLabels;
	pixLabels = Mat::zeros(imKMeans->rows, imKMeans->cols, CV_8UC1);

	// get dimensions
//	int height = imKMeans.rows;
//	int width = imKMeans.cols;
	int height = imKMeans->rows;
	int width = imKMeans->cols;

	// Select rand vals for K centroids
	for (uint8_t i = 0; i < (K * dimensions); i++) {
		centroids[i] = rand() % 255;
	}
	
// TODO : access as pointer, not .at
// http://stackoverflow.com/questions/1844736/accesing-a-matrix-element-in-the-mat-object-not-the-cvmat-object-in-opencv-c

	// Variables for finding the k-means
	uint8_t error = stopError + 1;
	uint8_t tempErr;
	uint8_t runs = 0;
	uint8_t label = 0;
	float distToBeat, newDist;
	float xScale, yScale;
	uint8_t xNew, yNew;
	Vec3b * getPixel;

	xScale = 255 / width;
	yScale = 255 / height;

	// Find the k-means
	while ((error > stopError) & (runs < stopCount)) {

		// Clear centSum, centCount
		for (uint8_t i = 0; i < K; i++) {
			for (uint8_t j = 0; j < dimensions; j++) {
				idx = j + (i * dimensions);
				centSum[idx] = 0;
				centCount[i] = 0;
			}
		}

		// For each pixel, find closest k
		for (int x = 0; x < width; x++){
			for (int y = 0; y < height; y++){

				getPixel = & imKMeans->at<Vec3b>(y, x);

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
						pixLabels.at<uint8_t>(y, x) = i;
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
					centSum[idx + 1] = xNew;
					yNew = y * yScale;
					centSum[idx + 2] = yNew;
				}
				centCount[label] += 1;

				// done with this pixel
			}
		}
		runs++;

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
	// Done with the final K centroids

	
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

				getPixel->val[j] = centroids[idx];
			}
			// done with this pixel


		}
	}

	cvtColor(imKMeans, imKMeans, CV_Lab2RGB);

	// free stuff
	free (centroids);
	free (centPrev);
	free (centSum);
	free (centCount);
}






}