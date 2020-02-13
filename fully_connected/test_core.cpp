//#include "core.h"
#include "testUtils.h"
#include "math.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
//Image File path
char outImage[8][16];
char outImageRef[8][16];
unsigned short realOutRef[128];

int main(){

	//create weights and biases arrays
	float bias[BIASES_SIZE];
	//create bias
	for(int i=0;i<BIASES_SIZE; i++){
		bias[i]=(rand() % 255)-128;
		bias[i]=(float)bias[i]/255;

	}

	//create weights
	float *weights[BIASES_SIZE];
	    for (int i=0; i<BIASES_SIZE; i++){
	         weights[i] = (float *)malloc(INPUT_NODES * sizeof(float));
	    }
	    // Note that arr[i][j] is same as *(*(arr+i)+j)

	    for (int i = 0; i <  BIASES_SIZE; i++){
	        for (int j = 0; j < INPUT_NODES; j++){
	            float tmp= (rand() % 255)-128;
	            weights[i][j] = (float)tmp/255;

	        }
	    }

	//Read input image
	printf("Load image %s \n", INPUT_IMAGE_CORE);
	cv::Mat imageSrc;
	imageSrc = cv::imread(INPUT_IMAGE_CORE);

	//Convert to grayscale
	cv::cvtColor(imageSrc, imageSrc, CV_BGR2GRAY);
	printf("Image Rows:%d Cols:%d\n", imageSrc.rows, imageSrc.cols);


	//Define streams for input and output
	hls::stream<FixedType> inputStream("my_input_stream");
	hls::stream<FixedType>  outputStream("my_output_stream");
	hls::stream<half> weightsStream("my_weights_stream");
	hls::stream<half> biasesStream("my_biases_stream");

	//OpenCV mat that points to an array (cv::Size(Width, Height))
	cv::Mat imgCvOut(cv::Size(50, 20), CV_16UC1, outImage, cv::Mat::AUTO_STEP);
	//cv::Mat imgCvOutRef(cv::Size(50, 20), CV_32F, outImageRef, cv::Mat::AUTO_STEP);
	cv::Mat imgCvOutRef(cv::Size(50, 20), CV_16UC1, outImageRef, cv::Mat::AUTO_STEP);

	/*while(!inputStream.empty()) { printf("Inside loop\n"); }
	printf("Outside loop\n");*/

	std::fstream newfile;
	newfile.open("/home/hector/Documents/demo/HLS_practice/fully_connected/maxpoolOut.txt", std::ios::in); //open a file to perform read operation using file object

	//Populate input stream
	half number;
	while(newfile >> number){
		FixedType valIn;
		valIn.data = number;
		//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
		inputStream  << valIn;
	}

	//populate weights stream
	for(int idxRows=0; idxRows < OUTPUT_NODES; idxRows++){
		for(int idxCols=0; idxCols < INPUT_NODES; idxCols++){
			half valIn;
			valIn = weights[idxRows][idxCols];
			//valIn.data = (unsigned short)((weights[idxRows][idxCols]*255)+128);
			//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
			weightsStream  << valIn;
		}
	}

	//populate biases stream
	for(int idxRows=0; idxRows < BIASES_SIZE; idxRows++){
		half valIn;
		valIn = bias[idxRows];
		//valIn.data = (unsigned short)((bias[idxRows]*255)+128);
		//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
		biasesStream  << valIn;
	}

	//Do the convolution (Reference)
	printf("Calling Reference function\n");

	//fullyConnectedHand(imageSrc, imgCvOutRef, bias, weights);

	printf("Reference function ended\n");


//	printf("Saving image Ref\n");
//	//saveImage(std::string(OUTPUT_IMAGE_REF), imgCvOutRef);
//	std::ofstream outFileRef;
//	outFileRef.open ("/home/hector/Documents/demo/HLS_practice/fully_connected/FCOutRef.txt");
//
//	// Take data from the output stream to our array outImage (Pointed in opencv)
//	for (int j=0; j < OUTPUT_NODES; j++){
//		FixedType valOut;
//		outputStream.read(valOut);
//		outFileRef << valOut.data<<"\n";
//		//float temp = (((float)valOut.data - 128) /255);
//		//printf("%f \n", temp);
//	}

	//Do the convolution on the core (Third parameter choose operation 0conv 1 erode 2 dilate
	printf("Calling Core function\n");

	doFullyConnected(inputStream, outputStream, biasesStream, weightsStream);//, 5.0);
	printf("Core function ended\n");

	//Take data from the output stream to our array outImage (Pointed in opencv)
	/*while(!outputStream.empty()) { printf("Inside loop\n"); }
		printf("Outside loop\n");
*/

	std::ofstream outFile;
	outFile.open ("/home/hector/Documents/demo/HLS_practice/fully_connected/FCOut.txt");

	// Take data from the output stream to our array outImage (Pointed in opencv)
	for (int j=0; j < OUTPUT_NODES; j++){
		FixedType valOut;
		outputStream.read(valOut);
		outFile << valOut.data<<"\n";
		//float temp = (((float)valOut.data - 128) /255);
		//printf("%f \n", temp);
	}
	outFile.close();
	return 0;
}














