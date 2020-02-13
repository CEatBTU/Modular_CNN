#include "core.h"
#include <stdio.h>
#include <hls_math.h>

void doFullyConnected(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream, hls::stream<half> &biases, hls::stream<half> &weights){

#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE axis port = weights
#pragma HLS INTERFACE axis port = biases
#pragma HLS INTERFACE s_axilite port = return bundle=CRTL_BUS


	//Defining the line buffer and setting the inter dependency to false through pragmas
	hls::LineBuffer<1,INPUT_NODES, half> inputBuff;
	hls::LineBuffer<1,BIASES_SIZE, half> biasesBuff;

	FixedType dataOutSideChannel;
	FixedType currInSideChannel;

	//FixedType currWeightSideChannel;
	//FixedType currBiasSideChannel;

	for(int outindx=0; outindx<OUTPUT_NODES; outindx++){
//#pragma HLS PIPELINE
		//Might not be necessary because it is only used once

		half currBiasSideChannel = biases.read();
		//FixedType currBiasInHalf = ((static_cast<FixedType>(currBiasSideChannel.data)) - 128 )/255;


		//biasesBuff.shift_up(outindx);
		biasesBuff.insert_top(currBiasSideChannel,  outindx);
		half res;

		for(int inindx=0; inindx<INPUT_NODES; inindx++){
#pragma HLS PIPELINE
			if(outindx==0){
				//read input only the first loop
				currInSideChannel = inStream.read();
				//FixedType currInHalf = ((static_cast<FixedType>(currInSideChannel.data)) - 128 )/255;
				//FixedType currInHalf = static_cast<FixedType>(currInSideChannel.data);
				half currInHalf = currInSideChannel.data;
				//inputBuff.shift_up(inindx);
				inputBuff.insert_top(currInHalf,  inindx);
			}
			//read weights
			half currWeightSideChannel = weights.read();

			//FixedType currWeightInHalf = ((static_cast<FixedType>(currWeightSideChannel.data)) - 128 )/255;

			//the first time coming from max pool the data will be parsed from char
			//might replace both with just one normalization

			//res += (currWeightSideChannel*(inputBuff.getval(0, inindx)) ) /255;// / 255) ;
			res += currWeightSideChannel*(inputBuff.getval(0, inindx));
		}

		//res = sigmoid(res + biasesBuff.getval(0,outindx));

		res = (1/ (1+ hls::exp(-(res + biasesBuff.getval(0,outindx)) )));
		// Fast sigmoid function:
		//f(x) = x / (1 + abs(x))
		//dataOutSideChannel.data = static_cast<ap_uint<16> >(res*255 +128);
		//unsigned short temp = static_cast<ap_uint<16> >(res);
		//float tempf= res;
		//dataOutSideChannel.data = static_cast<ap_uint<16> >(res);
		//dataOutSideChannel.data = res*255;
		dataOutSideChannel.data = res;

		if(outindx== OUTPUT_NODES-1){
			dataOutSideChannel.last = 1;
		}
		else{
			dataOutSideChannel.last = 0;
		}
		/*dataOutSideChannel.keep = currInSideChannel.keep;
		dataOutSideChannel.strb = currInSideChannel.strb;
		dataOutSideChannel.user = currInSideChannel.user;
		//dataOutSideChannel.last = currInSideChannel.last;
		if(outindx== OUTPUT_NODES-1){
			dataOutSideChannel.last = 1;
		}
		else{
			dataOutSideChannel.last = 0;
		}
		dataOutSideChannel.id = currInSideChannel.id;
		dataOutSideChannel.dest = currInSideChannel.dest;*/
		// Send to the stream (Block if the FIFO receiver is full)
		outStream.write(dataOutSideChannel);

	}
}
