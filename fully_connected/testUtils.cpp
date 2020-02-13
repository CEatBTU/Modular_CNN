#include "testUtils.h"
#include "math.h"


void fullyConnectedHand(cv::Mat imgSrc, cv::Mat outImageRef, float *biases, float *weights[BIASES_SIZE]){
	float res=0;
	/*float temp1=0;
	float temp2=0;
	float temp3=0;
	float temp4=0;*/
	int inRow=0;
	int inCol=0;
	int outRow=0;
	int outCol=0;



	for(int outindx=0; outindx<OUTPUT_NODES; outindx++){
		for(int inindx=0; inindx<INPUT_NODES; inindx++){

			res +=weights[outindx][inindx] * ((((float)imgSrc.at<unsigned char>(inRow, inCol))+  biases[outindx]) / 255);
			if(inCol<IMG_WIDTH_OR_COLS){
				inCol++;
			}
			else{
				inCol = 0;
				inRow++;
			}
		}
		inCol = 0;
		inRow = 0;

		outImageRef.at<float>(outRow,outCol)= sigmoid_hand(res);// + biases[outindx]);
		res = 0;

		if(outCol<50){
			outCol++;
		}
		else{
			outCol = 0;
			outRow++;
		}
	}
}

float sigmoid_hand(float res){
	return (1/ (1+ exp(-res)));
}

float tanh_hand(float res){
	return ((2*sigmoid_hand(res*2))-1);
}

void saveImage(std::string imagePath, cv::Mat image){
	cv::imwrite(imagePath, image );
printf("\n");
}

void printSmallMatrixCVChar(char *title, cv::InputArray inArr)
{
	printf("\%s\n",title);
	printf("----------------------------------\n");
	for (int rows = 0; rows < inArr.getMat().rows; rows++)
	{
		printf("ROW: %02d --->  ",rows);
		for (int cols = 0; cols < IMG_WIDTH_OR_COLS; cols++)
		{
			printf(" %04d ",inArr.getMat().at<unsigned char>(rows,cols));
		}
		printf("\n");
	}
	printf("\n");
}
