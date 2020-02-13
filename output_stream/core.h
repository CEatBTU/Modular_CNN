#include "hls_video.h"
#include "ap_fixed.h"

#define INPUT_NODES 10

#define OUTPUT_NODES 10

#define INPUT_IMAGE_CORE ""
#define OUTPUT_IMAGE_CORE ""
#define OUTPUT_IMAGE_REF ""

// Use the axi stream side-channel (TLAST,TKEEP,TUSR,TID)
#include <ap_axi_sdata.h>
typedef ap_axiu<16,2,5,6> uint_16_side_channel;
typedef ap_axis<16,2,5,6> int_8_side_channel;

//typedef half FixedType;
//typedef ap_fixed<16, 5> FixedType;

struct FixedType{
  half data;
  bool last;
};

/*
typedef union{
	uint_16_side_channel i;
	FixedType f;
} mix_union;*/


//typedef ap_fixed<28,8> FixedType; //ap_fixed<a, b> 'a' bits of data with 'b' integer and 'a-b' fractional bits


// Reference functions
//short convolve2d(hls::Window<WINDOW_POOL_DIM,WINDOW_POOL_DIM,char> *window, char kernel[WINDOW_POOL_DIM*WINDOW_POOL_DIM]);

// Our IP core
void doOutput(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream);//, FixedType test);//, int operation);



