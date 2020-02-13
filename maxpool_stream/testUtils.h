/*#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <hls_opencv.h>
#include "core.h"

short processWindowAndKernel(short *kernel, char *window, short numKernelElements);
void conv2dByHand(cv::InputArray imageIn, char imageOut[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS],  char kernel[KERNEL_DIM*KERNEL_DIM], int kSize);
void printSmallMatrixCVShort(char *title, cv::InputArray inArr);
void printSmallMatrixCVChar(char *title, cv::InputArray inArr);
void saveImage(const std::string path, cv::InputArray inArr);*/

#include <stdio.h>
#include "core.h"
#include <hls_opencv.h>
#include <opencv2/core/core.hpp>

//char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS]

//void conv2dByHand(cv::Mat imgSrc, char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS], char kernel[KERNEL_DIM*KERNEL_DIM], int kernelDim);
void maxByHand(cv::Mat imgSrc, cv::Mat outImageRef, int windowDim);
short maxArr(short *tmp);
void printSmallMatrixCVChar(char *title, cv::InputArray inArr);
void saveImage(std::string imagePath, cv::Mat image);

