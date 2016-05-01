package org.ece420.lab5;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
//import org.opencv.imgproc.Imgproc;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

class Sample4View extends SampleViewBase {
    private static final String TAG = "OCVSample::View";

    public static final int     SEGMENT_THRESHOLD   = 0;
    public static final int     SEGMENT_KMEAN_SLOW  = 1;
    public static final int     SEGMENT_KMEAN_FAST  = 2;
    public static final int     VIEW_MODE_GRAY      = 3;
    public static final int     VIEW_MODE_RGBA      = 4;
    public static final int     VIEW_MODE_HISTEQ    = 5;

    private Mat                 mYuv;
    private Mat                 mRgba;
    private Mat                 mGraySubmat;
    private Bitmap              mBitmap;
    private int                 mViewMode;
    private int                 mCentroids;

    public Sample4View(Context context) {
        super(context);
    }

    @Override
    protected void onPreviewStarted(int previewWidth, int previewHeight) {
        Log.i(TAG, "called onPreviewStarted("+previewWidth+", "+previewHeight+")");

        // initialize Mats before usage
        mYuv = new Mat(getFrameHeight() + getFrameHeight() / 2, getFrameWidth(), CvType.CV_8UC1);
        mGraySubmat = mYuv.submat(0, getFrameHeight(), 0, getFrameWidth());

        // allocate space now because are using our own color conversion function
        mRgba = new Mat(getFrameHeight(), getFrameWidth(), CvType.CV_8UC4);

        mBitmap = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888);

        int k = 10;
        int d = 5;
        mCentroids = new int[k * d];
        for(int i = 0; i < (k * d); i++) {
            mCentroids[i] = 0;
        }

    }

    @Override
    protected void onPreviewStopped() {
        Log.i(TAG, "called onPreviewStopped");

        if (mBitmap != null) {
            mBitmap.recycle();
            mBitmap = null;
        }
        if(mCentroids != null){
            mCentroids.release();
            mCentroids = null;
        }

        synchronized (this) {
            // Explicitly deallocate Mats & such
            if (mYuv != null)
                mYuv.release();
            if (mRgba != null)
                mRgba.release();
            if (mGraySubmat != null)
                mGraySubmat.release();

            mYuv = null;
            mRgba = null;
            mGraySubmat = null;
        }
    }


    @Override
    protected Bitmap processFrame(byte[] data) {
    	// data from camera is in YUV420sp format
        mYuv.put(0, 0, data);

        final int viewMode = mViewMode;

        switch (viewMode) {
        case SEGMENT_THRESHOLD:
            segThresh(mYuv.getNativeObjAddr(), mRgba.getNativeObjAddr());
            break;
        case SEGMENT_KMEAN_SLOW:
            segKMeans(mYuv.getNativeObjAddr(),mRgba.getNativeObjAddr());
            break;
        case SEGMENT_KMEAN_FAST:
            segKMeansLive(mYuv.getNativeObjAddr(), mRgba.getNativeObjAddr(), mCentroids.getNativeObjAddr());
            break;
        case VIEW_MODE_GRAY:
        	// opencv's color conversion function
            Imgproc.cvtColor(mGraySubmat, mRgba, Imgproc.COLOR_GRAY2RGBA, 4);
           break;
        case VIEW_MODE_RGBA:
            // opencv's color converstion function
            //Imgproc.cvtColor(mYuv, mRgba, Imgproc.COLOR_YUV420sp2RGB, 4);
        	// manual implementation
        	YUV2RGB(mYuv.getNativeObjAddr(),mRgba.getNativeObjAddr());
            break;
        case VIEW_MODE_HISTEQ:
        	// apply equalization to Y channel and convert to RGB
            HistEQ(mYuv.getNativeObjAddr(), mRgba.getNativeObjAddr());
            break;
        }

        Bitmap bmp = mBitmap;

        try {
            Utils.matToBitmap(mRgba, bmp);
        } catch(Exception e) {
            Log.e("org.opencv.samples.puzzle15", "Utils.matToBitmap() throws an exception: " + e.getMessage());
            bmp.recycle();
            bmp = null;
        }

        return bmp;
    }

    public native void segKMeansLive(long matAddrYUV, long matAddrRgba, long addrCentroids);
    public native void segKMeans(long matAddrYUV, long matAddrRgba);
    public native void segThresh(long matAddrYUV, long matAddrRgba);
    public native void YUV2RGB(long matAddrYUV, long matAddrRgba);
    public native void HistEQ(long matAddrYUV, long matAddrRgba);

    public void setViewMode(int viewMode) {
        Log.i(TAG, "called setViewMode("+viewMode+")");
        mViewMode = viewMode;
    }
}
