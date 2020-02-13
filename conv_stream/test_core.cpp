#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <hls_opencv.h>
//#include "core.h"
#include "testUtils.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "normalized.h"
// Impulse
/*float kernel[KERNEL_DIM*KERNEL_DIM] = {
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0
};*/

half kernel[KERNEL_DIM*KERNEL_DIM] = {
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0
};
float kernel_float[KERNEL_DIM*KERNEL_DIM] = {
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0
};

// Image File path
char outImage[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];
char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];

int main(){
	// Read input image
	printf("Load image %s\n",INPUT_IMAGE_CORE);
	cv::Mat imageSrc;
	imageSrc = cv::imread(INPUT_IMAGE_CORE);
	// Convert to grayscale
	cv::cvtColor(imageSrc, imageSrc, CV_BGR2GRAY);
	printf("Image Rows:%d Cols:%d\n",imageSrc.rows, imageSrc.cols);

	// Define streams for input and output
	hls::stream<FixedType> inputStream("my_input_stream");
	hls::stream<half> kernelStream("my_kernel_stream");
	hls::stream<FixedType> outputStream("my_output_stream");

	// OpenCV mat that point to a array (cv::Size(Width, Height))
	cv::Mat imgCvOut(cv::Size(imageSrc.cols, imageSrc.rows), CV_16U, outImage, cv::Mat::AUTO_STEP);
	cv::Mat imgCvOutRef(cv::Size(imageSrc.cols, imageSrc.rows), CV_16U, outImageRef, cv::Mat::AUTO_STEP);

	//std::fstream newfile;

//	unsigned char a;
//	newfile.open("../../../convOut.txt", std::ios::in); //open a file to perform read operation using file object
//
//	int number;
//	while(newfile >> number){
//		a= (unsigned char)number;
//		printf("%d, ",a);
//
//	}
	std::fstream newfile;
	newfile.open("/home/hector/Documents/demo/HLS_practice/conv_stream/normalized.txt", std::ios::in); //open a file to perform read operation using file object


	half number;
	while(newfile >> number){
		FixedType valIn;
		valIn.data = static_cast<half>(number);
		//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
		inputStream  << valIn;
	}
	newfile.close();
//	// Populate the input stream with the image bytes
//	for (int idx=0; idx < imageSrc.rows * imageSrc.cols; idx++){
//		//for (int idxCols=0; idxCols < imageSrc.cols; idxCols++){
//		FixedType valIn;
//		//uint_8_side_channel temp;
//		//valIn.data = imageSrc.at<unsigned char>(idxRows,idxCols);
//		valIn.data = static_cast<half>(img[idx]);
//		//temp.data = imageSrc.at<unsigned char>(idxRows,idxCols);
//		//printf("%c, %d\n",temp.data, valIn.data);
//		//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
//		inputStream << valIn;
//		//}
//	}
	/*uint16_t kernel_temp[KERNEL_DIM*KERNEL_DIM];
	for(int i=0; i<KERNEL_DIM*KERNEL_DIM; i++){
		kernel_temp[i] = FloatToUint(kernel[i]);
	}*/
	//populate kernel stream
	for (int idxKernel=0; idxKernel < NUM_KERNELS; idxKernel++){
		for(int i=0; i<KERNEL_DIM*KERNEL_DIM;i++){
			half valIn;
//			uint_16_side_channel valIn;
//			conv temp;
//			temp.f= kernel[i];
//			valIn.data = temp.i;
//			valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
//			kernelStream  << valIn;
			//valIn.data = (unsigned short)((kernel[i]*255)+128);
			valIn = kernel[i];
			kernelStream  << valIn;

		}
	}
	// Do the convolution (Reference)
	//int kernNum = 16;

	//std::string imgName =std::string(OUTPUT_IMAGE_REF);
	printf("Calling Reference function\n");
	for(int idxImg = 0; idxImg<NUM_IMAGES;idxImg++){
		for(int idxKer=0; idxKer<NUM_KERNELS; idxKer++){

			conv2dByHand(imageSrc,outImageRef, kernel_float, KERNEL_DIM);
			std::string imgName =std::string(OUTPUT_IMAGE_REF) + std::to_string(idxKer) + std::string(".png");
			//printf("number of image: %d", kernNum);
			saveImage(imgName, imgCvOutRef);
		}
	}

	printf("Reference function ended\n");


	// Do the convolution on the core (Third parameter choose operation 0(conv),1(erode),2(dilate)
	printf("Calling Core function\n");

	doConv(inputStream, outputStream, kernelStream);
	printf("Core function ended\n");

	std::ofstream outFile;
	outFile.open ("/home/hector/Documents/demo/HLS_practice/conv_stream/convOut.txt");

	// Take data from the output stream to our array outImage (Pointed in opencv)
	for (int j=0; j < NUM_KERNELS*IMG_HEIGHT_OR_ROWS*IMG_WIDTH_OR_COLS; j++){

			FixedType valOut;
			outputStream.read(valOut);
			//outImage[idxRows][idxCols] = valOut.data;
			//outKern[j] = valOut.data;
			outFile << valOut.data<<"\n";

	}
	outFile.close();

	return 0;
}
