#include "hls_video.h"
#include "ap_fixed.h"

#define INPUT_NODES 3136
#define IMG_WIDTH_OR_COLS 14
#define IMG_HEIGHT_OR_ROWS 14

#define OUTPUT_NODES 128
#define WEIGHTS_SIZE INPUT_NODES * OUTPUT_NODES
#define BIASES_SIZE OUTPUT_NODES

#define IS_FIRST_FC 1


#define INPUT_IMAGE_CORE ""
#define OUTPUT_IMAGE_CORE ""
#define OUTPUT_IMAGE_REF ""

// Use the axi stream side-channel (TLAST,TKEEP,TUSR,TID)
#include <ap_axi_sdata.h>
typedef ap_axiu<16,2,5,6> uint_16_side_channel;
typedef ap_axis<8,2,5,6> int_8_side_channel;

//typedef ap_fixed<28,8> FixedType; //ap_fixed<a, b> 'a' bits of data with 'b' integer and 'a-b' fractional bits

//typedef half FixedType;

struct FixedType{
  half data;
  bool last;
};
//typedef ap_fixed<18, 7> FixedType;

// Reference functions
//short convolve2d(hls::Window<WINDOW_POOL_DIM,WINDOW_POOL_DIM,char> *window, char kernel[WINDOW_POOL_DIM*WINDOW_POOL_DIM]);

// Our IP core
void doFullyConnected(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream, hls::stream<half> &biases, hls::stream<half> &weights);
//FixedType sigmoid(FixedType res);


