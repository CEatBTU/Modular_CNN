//#include "core.h"
#include "testUtils.h"
#include "math.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
//Image File path
//#define INPUT_NODES 10

char outImage[8][16];
char outImageRef[8][16];


int main(){


	//Define streams for input and output
	hls::stream<FixedType> inputStream("my_input_stream");
	hls::stream<FixedType>  outputStream("my_output_stream");


	/*while(!inputStream.empty()) { printf("Inside loop\n"); }
	printf("Outside loop\n");*/

	//fill the input node
	float input_nodes[INPUT_NODES];//={2.0, 1.0, 0.1};
	float output_nodes[INPUT_NODES];
	for(int i=0;i<INPUT_NODES; i++){
		input_nodes[i]=(rand() % 255)-128;
		input_nodes[i]=(float)input_nodes[i]/255;
	}
	printf("Random values generated:\n");
	for(int i=0;i<INPUT_NODES; i++){
		printf("%f, ", input_nodes[i]);
	}
	printf("\n");
	for(int i=0; i<INPUT_NODES; i++){
		FixedType valIn;
		//valIn.data = (short)((input_nodes[i]*255)+128);
		valIn.data = input_nodes[i];
		//valIn.keep = 1; valIn.strb = 1; valIn.user = 1; valIn.id = 0; valIn.dest = 0;
		inputStream  << valIn;
	}



	//Do the convolution (Reference)
	printf("Calling Reference function\n");

	outputHand(input_nodes, output_nodes);

	printf("Reference function ended\n");
	float sum;
	for(int i=0;i<INPUT_NODES; i++){
		printf("%f \n", output_nodes[i]);
		sum+=output_nodes[i];
	}
	printf("\n the sum of all are: %f\n", sum);

	//Do the convolution on the core (Third parameter choose operation 0conv 1 erode 2 dilate
	printf("Calling Core function\n");

	doOutput(inputStream, outputStream);
	printf("Core function ended\n");

	sum=0;
	// Take data from the output stream to our array outImage (Pointed in opencv)
	for (int j=0; j < INPUT_NODES; j++){

		FixedType valOut;
		outputStream.read(valOut);
		//outImage[idxRows][idxCols] = valOut.data;
		//outKern[j] = valOut.data;
		//printf("%d \n", valOut.data);
		//float temp = (((float)valOut.data - 128) /255);
		float temp = valOut.data;
		printf("%f \n", temp);
		sum+=temp;
	}
	printf("\n the sum of all are: %f\n", sum);
	return 0;
}














