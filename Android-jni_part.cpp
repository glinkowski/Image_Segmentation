/* ///////////////////////////////
 *
 * project: ECE 420, Lab 5
 *
 * authors:
 *    Greg Linkowski
 *    Trevor Bush
 *    yces3404 (name missing)
 *
 * purpose:
 *    image manipulation on Android
 *    convert between YUV & RGB
 *
 */ //////////////////////////////

#include <jni.h>
#include <opencv2/core/core.hpp>
#include <android/log.h>

using namespace std;
using namespace cv;

int TOTAL;
int THISPIXEL;

extern "C" {

// function declaration for manual color conversion
int convertYUVtoARGB(int, int, int);

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

/*    		uint8_t Y = pYUV->at<uchar>(y * yuvWidth + x);
    		uint8_t U = pYUV->at<uchar>( (y / 2) * (yuvWidth / 2) + (x / 2) + yuvTotal );
    		uint8_t V = pYUV->at<uchar>( (y / 2) * (yuvWidth / 2) + (x / 2) + yuvTotal + (yuvTotal / 4) );
    		pRGB->at<uchar>(y * yuvWidth  + x) = convertYUVtoARGB(Y, u, v);
*/

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

/*	int rgbHeight = pRGB->rows;
	int rgbWidth = pRGB->cols;
	int rgbTotal = rgbWidth * rgbHeight;

    for(int x = 0; x < rgbWidth; x++) {
		for(int y = 0; y < rgbHeight; y++) {
			uint8_t Y = pYUV->at<uchar>(y,x);
			int Upos = (y / 2) * (rgbWidth / 2) + (x / 2) + rgbTotal;
			uint8_t U = pYUV->at<uchar>( (Upos / rgbWidth), (Upos % rgbWidth) );
			int Vpos = (y / 2) * (rgbWidth / 2) + (x / 2) + rgbTotal + (rgbTotal / 4);
			uint8_t V = pYUV->at<uchar>( (Vpos / rgbWidth), (Vpos % rgbWidth) );
			pRGB->at<int32_t>( y , x ) = convertYUVtoARGB(Y, U, V);
		}
    }
*/

}

JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_HistEQ(JNIEnv* env, jobject thiz, jlong addrYuv, jlong addrRgba)
{
    Mat* pYUV=(Mat*)addrYuv;
    Mat* pRGB=(Mat*)addrRgba;;

    int yuvHeight = pYUV->rows * 2 / 3;
    int yuvWidth = pYUV->cols;
    int yuvTotal = yuvWidth * yuvHeight;
    TOTAL = yuvTotal;
    int histogram[256] = {0};
    int cdf[256] = {0};
    int value;

    for(int x = 0; x < yuvWidth; x++) {
    	for(int y = 0; y < yuvHeight; y++) {
    	value = pYUV->at<uint8_t>(y,x);
    	histogram[value]++;
    	}
    }


    for(int i = 0; i < 256; i++) {
    	for(int j = 0; j < i; j++) {
    		cdf[i] += histogram[j];
    	}
    }

    for(int i = 0; i < 256; i++) {
    	cdf[i] *= (255/TOTAL);
    }

    for(int x = 0; x<yuvWidth/2; x++) {
    	for(int y = 0; y<yuvHeight; y++) {
    		int val = pYUV->at<uint8_t>(y,x);
    		pYUV->at<uint8_t>(y,x) = cdf[val];
    	}
    }

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

       		//int Vpos = (y / 2) * (yuvWidth / 2) + (x / 2) + yuvTotal + (yuvTotal / 4);
       		int Vpos = yuvTotal + block_y*yuvWidth+block_x*2 + 1;
       		int V = 0;
       		V = V | pYUV->at<uint8_t>( (Vpos / yuvWidth), (Vpos % yuvWidth) );

       		pRGB->at<int32_t>(y,x) = convertYUVtoARGB(Y, U, V);

       	}
       }


}



int32_t convertYUVtoARGB(int y, int u, int v) {

	float rTmp = y + (1.370705 * (v-128));
	float gTmp = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
	float bTmp = y + (1.732446 * (u-128));

    char buf[128];

//	int rTmp = y + (int)(1.370705 * (v));
//	int gTmp = y - (int)(0.698001 * (v)) - (0.337633 * (u));
//	int bTmp = y + (int)(1.732446 * (u));

	int red;
	int green;
	int blue;
	int max = 200;
	int min = 25;
	//rTmp = 255*(rTmp-min)/(max-min);
	//gTmp = 255*(gTmp-min)/(max-min);
	//bTmp = 255*(bTmp-min)/(max-min);

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

	// Create the 32-bit output pixel (A R G B, where A=x0000? or A = 255?)
/*	int32_t out_pix = 0;
	out_pix += red;
	out_pix = out_pix << 8;
//	int out_pix = red << 8;
	out_pix += green;
	out_pix = out_pix << 8;
	out_pix += blue;
*/

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

}
