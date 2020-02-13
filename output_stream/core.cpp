#include "core.h"
#include <stdio.h>
#include <hls_math.h>


void doOutput(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream){
#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE s_axilite port = return bundle=CRTL_BUS

	half res;
	half max;
	hls::LineBuffer<1,INPUT_NODES, half> inputBuff;

	FixedType dataOutSideChannel;
	FixedType currInSideChannel;
	half currInHalf;
	//Find maximum
	for(int inindx=0; inindx<INPUT_NODES; inindx++){
#pragma HLS PIPELINE
		currInSideChannel = inStream.read();

		//Get the pixel data
		currInHalf = currInSideChannel.data;

		//Put data on the LineBuffer
		inputBuff.insert_top(currInHalf,  inindx);	///Will put in val[2] of the line buffer (Check Debug)
		if(inindx==0){
			max = currInHalf;
		}
		if(inputBuff.getval(0, inindx)>max){
			max=currInHalf;
		}
	}

	//Sum
	for(int i=0; i<INPUT_NODES; i++){
#pragma HLS PIPELINE

		res +=  hls::exp((inputBuff.getval(0, i))-max);
	}

	//output
	half offset = (hls::log(res)) + max;//static_cast<FixedType>(hls::logf((float)res));

	for(int outindx=0; outindx<INPUT_NODES; outindx++){
#pragma HLS unroll

		half partial= hls::exp((inputBuff.getval(0, outindx))-offset);

		dataOutSideChannel.data = partial;
		if(outindx==INPUT_NODES-1){
			dataOutSideChannel.last = 1;
		}
		else{
			dataOutSideChannel.last = 0;
		}
		outStream.write(dataOutSideChannel);
	}
}
