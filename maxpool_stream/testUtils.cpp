#include "testUtils.h"



void maxByHand(cv::Mat imgSrc, cv::Mat outImageRef, int windowDim){
	short tmp[imgSrc.rows/WINDOW_POOL_DIM][imgSrc.cols/WINDOW_POOL_DIM];
	short val[WINDOW_POOL_DIM*WINDOW_POOL_DIM];
	short x=0;
	short y=0;
	//la imagen se llena bien pero la de origen no se está tomando bien porque se corta al cuarto...
	for(int i=0; i < imgSrc.rows; i+=STRIDE){              // rows
	    for(int j=0; j < imgSrc.cols; j+=STRIDE){          // columns
	    	//outImageRef.at<unsigned char>(i,j) = imgSrc.at<unsigned char>(i, j);
	    	for(int helperRow=0;helperRow<WINDOW_POOL_DIM; helperRow++){
	    		for(int helperCol=0;helperCol<WINDOW_POOL_DIM; helperCol++){
	    			val[WINDOW_POOL_DIM*helperRow + helperCol]=imgSrc.at<unsigned char>(i+helperRow, j+helperCol);
	    		}
	    	}
	    	tmp[y][x]=maxArr(val);
	    	x++;


	    }
	    x=0;
	    y++;
	}
	for(int x=0; x < imgSrc.rows/WINDOW_POOL_DIM; x++){              // rows
		for(int y=0; y< imgSrc.cols/WINDOW_POOL_DIM; y++){
			outImageRef.at<unsigned char>(x,y) = tmp[x][y];
		}
	}
}

short maxArr(short *tmp){
	unsigned char maxVal = 0;

	for(int idxRow=0; idxRow < WINDOW_POOL_DIM; idxRow++){

		for(int idxCol=0; idxCol<WINDOW_POOL_DIM; idxCol++){
			unsigned char valInWindow;
			valInWindow = tmp[WINDOW_POOL_DIM*idxRow + idxCol];

			if(valInWindow > maxVal){
				maxVal = valInWindow;
			}
		}
	}
	return maxVal;
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
