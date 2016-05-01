ECE 420 - Lab 5: Histogram Equalization

David Jun
University of Illinois
02/18/2013


Location: 

  V:\ece420\Nexus\Lab5



Description: 

This lab uses the OpenCV library to handle and manipulate images in mixed Java and Native C.

The structure of this project was derived from the OpenCV sample tutorial 4, located at:

  C:\NvPack\OpenCV-2.4.2-Tegra-sdk\samples\tutorial-4-mixed

This project has 3 options: 1) View RGB, 2) View Grayscale, 3) View Histogram Equalization. In Lab 5, students are 
required to manually implement the color conversion from YUV to RGB, and also the histogram equalization code.
Grayscale conversion using OpenCV's built-in function is also included for reference.

Finally, to support OpenCV functions, the "OpenCV Library - 2.4.2" must be included in the workspace and built first.
In particular, "opencv library - 2.4.2.jar" must be generated and available before trying to build Lab5.