//#include "core.h"
#include "testUtils.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
//Image File path
char outImage[IMG_HEIGHT_OR_ROWS/WINDOW_POOL_DIM][IMG_WIDTH_OR_COLS/WINDOW_POOL_DIM];
char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];

int main(){
	//Read input image
	printf("Load image %s \n", INPUT_IMAGE_CORE);
	cv::Mat imageSrc;
	imageSrc = cv::imread(INPUT_IMAGE_CORE);

	//Convert to grayscale
	cv::cvtColor(imageSrc, imageSrc, CV_BGR2GRAY);
	printf("Image Rows:%d Cols:%d\n", imageSrc.rows, imageSrc.cols);


	//Define streams for input and output
	hls::stream<FixedType> inputStream("my_input_stream");
	hls::stream<FixedType> outputStream("my_output_stream");

	//OpenCV mat that points to an array (cv::Size(Width, Height))
	cv::Mat imgCvOut(cv::Size(imageSrc.cols/WINDOW_POOL_DIM, imageSrc.rows/WINDOW_POOL_DIM), CV_16UC1, outImage, cv::Mat::AUTO_STEP);
	cv::Mat imgCvOutRef(cv::Size(imageSrc.cols/WINDOW_POOL_DIM, imageSrc.rows/WINDOW_POOL_DIM), CV_16UC1, outImageRef, cv::Mat::AUTO_STEP);


	/*while(!inputStream.empty()) { printf("Inside loop\n"); }
	printf("Outside loop\n");*/
	//populate the input stream with the image bytes

	//std::ifstream infile("convOut.txt");
	std::fstream newfile;
	newfile.open("/home/hector/Documents/demo/HLS_practice/maxpool_stream/convOut.txt", std::ios::in); //open a file to perform read operation using file object

	unsigned short a;
	half number;
	while(newfile >> number){
		FixedType valIn;
		valIn.data = static_cast<half>(number);
		//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
		inputStream  << valIn;
	}
    newfile.close();

	//Do the convolution (Reference)
	printf("Calling Reference function\n");

	maxByHand(imageSrc, imgCvOutRef, WINDOW_POOL_DIM);
	printf("Reference function ended\n");

	/*//Save image out file or display
	if(imageSrc.rows <12){
		printSmallMatrixCVChar("Ref Core", imgCvOutRef);
	}*/
	//else{
		printf("Saving image Ref\n");
		saveImage(std::string(OUTPUT_IMAGE_REF), imgCvOutRef);
	//}

	//Do the convolution on the core (Third parameter choose operation 0conv 1 erode 2 dilate
	printf("Calling Core function\n");
	doMaxpool(inputStream, outputStream);
	printf("Core function ended\n");


	std::ofstream outFile;
	outFile.open ("/home/hector/Documents/demo/HLS_practice/maxpool_stream/maxpoolOut.txt");

	// Take data from the output stream to our array outImage (Pointed in opencv)
	for (int j=0; j < NUM_IMAGES*((IMG_HEIGHT_OR_ROWS/WINDOW_POOL_DIM)*(IMG_WIDTH_OR_COLS/WINDOW_POOL_DIM)); j++){

			FixedType valOut;
			outputStream.read(valOut);
			//outImage[idxRows][idxCols] = valOut.data;
			//outKern[j] = valOut.data;
			outFile << valOut.data<<"\n";

	}
	outFile.close();
	return 0;
}














