#include <stdio.h>
#include "core.h"
#include <hls_opencv.h>
#include <opencv2/core/core.hpp>

//char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS]

//void conv2dByHand(cv::Mat imgSrc, char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS], char kernel[KERNEL_DIM*KERNEL_DIM], int kernelDim);
void outputHand(float inputs[INPUT_NODES], float outputs[INPUT_NODES]);
