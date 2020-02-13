#include "testUtils.h"
#include "math.h"


void outputHand(float inputs[INPUT_NODES], float outputs[INPUT_NODES]){
	float res;
	float temp;
	float max=inputs[0];
	for(int inindx=0; inindx<INPUT_NODES; inindx++){
		if(inputs[inindx]>max){
			max=inputs[inindx];
		}
	}
	for(int inindx=0; inindx<INPUT_NODES; inindx++){
		temp=(float)inputs[inindx];
		res+=expf(temp - max);
		//printf("%f \n", res);
	}
	float offset = max + logf(res);
	for(int outindx=0; outindx<INPUT_NODES; outindx++){
		outputs[outindx] = (float)(expf(inputs[outindx] - offset));
	}
}
