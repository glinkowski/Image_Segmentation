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
 *    perform thresholding seg
 *    perform k-means segmentation
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
//  function declaration for manual color conversion
//int convertYUVtoARGB(int, int, int);

// //////////////// //////////////// ////////////////
//  pixel-by-pixel color conversion, YUV -> RGB
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

  // ERROR CHECK: write to the eclipse terminal
  //    sprintf(buf,"RGB= %d, %d, %d; YUV= %d, %d, %d", red,green, blue, y, u, v);
  //    __android_log_write(ANDROID_LOG_INFO, "RGB", buf);

  return out_pix;
}


// //////////////// //////////////// ////////////////
//  convert entire frame to RGB & display, YUV -> RGB
JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_YUV2RGB(JNIEnv*, jobject, jlong addrYuv, jlong addrRgba) {
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

      //int Upos = (y / 2) * (yuvWidth / 2) + (x / 2) + yuvTotal;
      //int Upos = yuvTotal + (y * yuvWidth / 4) + (x / 2);
      int Upos = yuvTotal + block_y*yuvWidth+block_x*2;
      int U = 0;
      U = U | pYUV->at<uint8_t>( (Upos / yuvWidth), (Upos % yuvWidth) );

      //int Vpos = (y / 2) * (yuvWidth / 2) + (x / 2) + yuvTotal + (yuvTotal / 4);
      //int Vpos = yuvTotal + (yuvTotal / 2) + (y * yuvWidth / 4) + (x / 2);
      int Vpos = yuvTotal + block_y*yuvWidth+block_x*2 + 1;
      int V = 0;
      V = V | pYUV->at<uint8_t>( (Vpos / yuvWidth), (Vpos % yuvWidth) );

      // ERROR CHECK: write to the eclipse terminal
      //sprintf(buf,"YUV= %d, %d, %d", Y, U, V);
      //__android_log_write(ANDROID_LOG_INFO, "preConversion", buf);

      pRGB->at<int32_t>(y,x) = convertYUVtoARGB(Y, U, V);

    }
  }
}



// //////////////// //////////////// ////////////////
//  k-means Image Segmentation on a single frame
JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_segKMeans(JNIEnv*, jobject, jlong addrYuv, jlong addrRgba) {

  // Parameter(s)
  uint8_t K = 8;
  uint8_t dimensions = 3; // either 3 or 5
  uint8_t stopError = 15;
  uint8_t stopCount = 11;


  // Input & Output images
  Mat * matImYUV = (Mat *) addrYuv;
  uint8_t * imYUV = (uint8_t *) addrYuv
  // Mat * imKMeans = (Mat *) addrRgba;
  uint32_t * imKMeans = (uint32_t *) addrRgba

  // get image dimensions
  int height = matImYUV->rows * 2/3;
  int width = matImYUV->cols;
  int size = height * width;



  // Declare centroid spaces
  uint8_t * centroids, * centPrev;
  centroids = (uint8_t *) malloc( K * dimensions * sizeof(uint8_t));
  centPrev = (uint8_t *) malloc( K * dimensions * sizeof(uint8_t));
  float * centSum, * centCount;
  centSum = (float *) calloc( K * dimensions, sizeof(float) );
  centCount = (float *) calloc( K, sizeof(float) );

  uint8_t idx;      // for stepping through centroids


  // array to hold pixel labels
  uint8_t pixLabels[height * width];


  // Variables for finding the k-means


  // Find the k-means ////////

  // Select rand vals for K centroids
  for (uint8_t i = 0; i < (K * dimensions); i++) {
    centroids[i] = rand() % 255;
  }


  // Variables for finding the k-means
  uint8_t error = stopError + 1;
  uint8_t tempErr;
  uint8_t runs = 0;
  uint8_t label = 0;
  float distToBeat, newDist;
  float xScale = 255.0 / width;
  float yScale = 255.0 / height;
  uint8_t xNew, yNew;
  //  Vec3b * getPixel;
  int Y, U, V;
  int block_x, block_y;
  int Upos, Vpos;


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
    for (int x = 0; x < width; x = x+2){
      for (int y = 0; y < height; y = y+2){

        block_x = x/2;
        block_y = y/2;

        Y = 0;
        // Y = Y | imYUV->at<uint8_t>(y,x);
        Y = Y | imYUV[(y * width) + x];
  
        Upos = size + block_y*width+block_x*2;
        U = 0;
        // U = U | imYUV->at<uint8_t>( (Upos / width), (Upos % width) );
        U = U | imYUV[(Upos / width) + (Upos % width)];
  
        Vpos = size + block_y*width+block_x*2 + 1;
        V = 0;
        // V = V | imYUV->at<uint8_t>( (Vpos / width), (Vpos % width) );
        V = V | imYUV[(Vpos / width) + (Vpos % width)];

        // measure (squared) dist to first centroid
        distToBeat = 16581375;

        // Determine which centroid is closest to this pixel
        for (uint8_t i = 0; i < K; i++) {
          // calculate the squared dist
          newDist = 0;
          idx = i * dimensions;
          newDist += (Y - centroids[idx]) * (Y - centroids[idx]);
          newDist += (U - centroids[idx+1]) * (U - centroids[idx+1]);
          newDist += (V - centroids[idx+2]) * (V - centroids[idx+2]);
          if (dimensions == 5) {
            xNew = x * xScale;
            yNew = y * yScale;
            newDist += (xNew - centroids[idx + 3]) *
              (xNew - centroids[idx + 3]);
            newDist += (yNew - centroids[idx + 4]) *
              (yNew - centroids[idx + 4]);
          }
          // test this dist against winner
          // save the label of the winning centroid
          if (newDist < distToBeat) {
            distToBeat = newDist;
            // pixLabels.at<uint8_t>(y, x) = i;
            pixLabels[(y * width) + x] = i;
          }
        }

        // Update the centroid values 1: sum of members
        // label = pixLabels.at<uint8_t>(y, x);
        label = pixLabels[(y * width) + x];

        idx = label * dimensions;
        centSum[idx] += Y;
        centSum[idx+1] += U;
        centSum[idx+2] += V;
        if (dimensions == 5) {
          xNew = x * xScale;
          centSum[idx + 3] += xNew;
          yNew = y * yScale;
          centSum[idx + 4] += yNew;
        }
        centCount[label] += 1;
      } // // // done with this pixel
    }
    runs++;

    // TODO: combine these loops ...

    // Update centroid values 2: find the average
    for (uint8_t i = 0; i < K; i++) {
      for (uint8_t j = 0; j < dimensions; j++) {
        idx = j + (i * dimensions);
        // If centroid has ~0 matches, create new centroid
        if (centCount[i] > 3) {
          centroids[idx] = (uint8_t) (centSum[idx] / centCount[i]);
        } else {
          centroids[idx] = rand() % 255;
        }
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

  } // // // Done with the final K centroids


  // Fnally: Place the new pixel value into the image
  uint8_t thisLabel;
  for (int x = 0; x < width; x = x+2){
  for (int y = 0; y < height; y = y+2){

      // Read the label for this pixel
      // thisLabel = pixLabels.at<uint8_t>(y, x);
      thisLabel = pixLabels[(y * width) + x];

      // Place the new value
      Y = centroids[(thisLabel * dimensions)];
      U = centroids[(thisLabel * dimensions) + 1];
      V = centroids[(thisLabel * dimensions) + 2];

      // imKMeans->at<int32_t>(y,x) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      // imKMeans->at<int32_t>(y,x+1) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      // imKMeans->at<int32_t>(y+1,x) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      // imKMeans->at<int32_t>(y+1,x+1) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[(y * width) + x] = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[(y * width) + x +1] = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[((y+1) * width) + x] = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[((y+1) * width) + x+1] = convertYUVtoARGB((int) Y, (int) U, (int) V);

    } // // // done with this pixel
  } // // // done with the output image


  // free stuff
  free (centroids);
  free (centPrev);
  free (centSum);
  free (centCount);
}



// //////////////// //////////////// ////////////////
//  Live K-means Image Segmentation
//    centroids calculated once each frame
JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_segKMeansLive(JNIEnv*, jobject, jlong addrYuv, jlong addrRgba, jlong addrCentroids, jint dimensions) {

  // Parameter(s)
  uint8_t K = 8;


  // Input & Output images
  Mat * matImYUV = (Mat *) addrYuv;
  uint8_t * imYUV = (uint8_t *) addrYuv
  // Mat * matImKMeans = (Mat *) addrRgba;
  uint32_t * imKMeans = (uint32_t *) addrRgba

  // get image dimensions
  int height = matImYUV->rows * 2/3;
  int width = matImYUV->cols;
  int size = height * width;


  // Centroid storage
  uint8_t * centroids = (uint8_t *) addrCentroids;
  uint8_t idx;      // for stepping through centroids

  // Declare centroid calculation helpers
  float * centSum, * centCount;
  centSum = (float *) calloc( K * dimensions, sizeof(float) );
  centCount = (float *) calloc( K, sizeof(float) );


  // array to hold pixel labels
  uint8_t pixLabels[height * width];


  // Variables for finding the k-means
  uint8_t label = 0;
  float distToBeat, newDist;
  float xScale = 255.0 / width;
  float yScale = 255.0 / height;
  uint8_t xNew, yNew;
  uint8_t Y, U, V;
  int block_x, block_y;
  int Upos, Vpos;


  // Find the k-means ////////

  // For each pixel, find closest k
  for (int x = 0; x < width; x = x+2){
    for (int y = 0; y < height; y = y+2){

      // Get the original pixel color
      block_x = x/2;
      block_y = y/2;

      // Y = imYUV->at<uint8_t>(y,x);
      Y = imYUV[(y * width) + x];

      Upos = size + block_y*width + block_x*2;
      // U = imYUV->at<uint8_t>( (Upos / width), (Upos % width) );
      U = imYUV[Upos + (Upos % width)];

      Vpos = size + block_y*width + block_x*2 + 1;
      // V = imYUV->at<uint8_t>( (Vpos / width), (Vpos % width) );
      V = imYUV[Vpos + (Vpos % width)];

      // measure (squared) dist to first centroid
      distToBeat = 16581375;

      // Determine which centroid is closest to this pixel
      for (uint8_t i = 0; i < K; i++) {
        // calculate the squared dist
        newDist = 0;
        idx = i * dimensions;
        newDist += (Y - centroids[idx]) * (Y - centroids[idx]);
        newDist += (U - centroids[idx+1]) * (U - centroids[idx+1]);
        newDist += (V - centroids[idx+2]) * (V - centroids[idx+2]);
        if (dimensions == 5) {
          xNew = x * xScale;
          yNew = y * yScale;
          newDist += (xNew - centroids[idx + 3]) *
            (xNew - centroids[idx + 3]);
          newDist += (yNew - centroids[idx + 4]) *
            (yNew - centroids[idx + 4]);
        }
        // test this dist against winner
        // save the label of the winning centroid
        if (newDist < distToBeat) {
          distToBeat = newDist;
          pixLabels[(y * width) + x] = i;
        }
      } // // // found appropriate centroid

      // Update the centroid values 1: sum of all members
      label = pixLabels[(y * width) + x];

      idx = label * dimensions;
      centSum[idx] += Y;
      centSum[idx+1] += U;
      centSum[idx+2] += V;
      if (dimensions == 5) {
        xNew = x * xScale;
        centSum[idx + 3] += xNew;
        yNew = y * yScale;
        centSum[idx + 4] += yNew;
      }
      centCount[label] += 1;

    } // // // done with this pixel
  } // // // done reading from camera


  // Update centroid values 2: find the average
  for (uint8_t i = 0; i < K; i++) {
    for (uint8_t j = 0; j < dimensions; j++) {
      idx = j + (i * dimensions);
      // Dropouts: If centroid has ~0 matches, create new centroid
      if (centCount[i] > 3) {
        centroids[idx] = (uint8_t) (centSum[idx] / centCount[i]);
      } else {
        centroids[idx] = rand() % 255;
      }
    }
  } // // // Done with the K centroids for this round


  // Fnally: Place the new pixel value into the image
  uint8_t thisLabel;
  for (int x = 0; x < width; x = x+2){
    for (int y = 0; y < height; y = y+2){

      // Read the label for this pixel
      thisLabel = pixLabels[(y * width) + x];

      // Place the new value
      Y = centroids[(thisLabel * dimensions)];
      U = centroids[(thisLabel * dimensions) + 1];
      V = centroids[(thisLabel * dimensions) + 2];


      // imKMeans->at<int32_t>(y,x) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      // imKMeans->at<int32_t>(y,x+1) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      // imKMeans->at<int32_t>(y+1,x) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      // imKMeans->at<int32_t>(y+1,x+1) = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[(y * width) + x] = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[(y * width) + x +1] = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[((y+1) * width) + x] = convertYUVtoARGB((int) Y, (int) U, (int) V);
      imKMeans[((y+1) * width) + x+1] = convertYUVtoARGB((int) Y, (int) U, (int) V);
    }
  } // // // Done drawing to the output


  // free stuff
  free (centSum);
  free (centCount);
}



JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_HistEQ(JNIEnv* env, jobject thiz, jlong addrYuv, jlong addrRgba) {
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

      //int Upos =  (y / 2) * (yuvWidth / 2) + (x / 2)  + yuvTotal;
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

JNIEXPORT void JNICALL Java_org_ece420_lab5_Sample4View_segThresh(JNIEnv*, jobject, jlong addrYuv, jlong addrRgba) {

  // Input & Output images
  Mat * imYUV = (Mat *) addrYuv;
  Mat * imThresh = (Mat *) addrRgba;


  // get dimensions
  uint32_t height = imYUV->rows * 2/3;
  uint32_t width = imYUV->cols;
  uint32_t size = height * width;


  uint8_t Y, U, V;
  uint8_t Ynew, Unew, Vnew;
  uint32_t block_x, block_y;


  // For each pixel, threshold the value
  for (int x = 0; x < width; x++){
    for (int y = 0; y < height; y++){

      // Get the pixel value
      block_x = x/2;
      block_y = y/2;
      Y = imYUV->at<uint8_t>(y,x);
      uint32_t Upos = size + block_y*width+block_x*2;
      U = imYUV->at<uint8_t>( (Upos / width), (Upos % width) );
      uint32_t Vpos = size + block_y*width+block_x*2 + 1;
      V = imYUV->at<uint8_t>( (Vpos / width), (Vpos % width) );

      // Threshold it
      if (Y <= 128) {
        Ynew = 64;
      } else {
        Ynew = 196;
      }
      if (U <= 128) {
        Unew = 64;
      } else {
        Unew = 196;
      }
      if (V <= 128) {
        Vnew = 64;
      } else {
        Vnew = 196;
      }

  //TODO: paint with average pixel value ?

      // Convert & place it
      imThresh->at<int32_t>(y,x) = convertYUVtoARGB(Ynew, Unew, Vnew);

    } // // // done with this pixel
  }
} // // // End of thresholding function


} // End of C encapsulation
