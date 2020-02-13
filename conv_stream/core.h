#include "hls_video.h"
#include <ap_int.h>

#define IMG_WIDTH_OR_COLS 28
#define IMG_HEIGHT_OR_ROWS 28

#define NUM_IMAGES	1
#define NUM_KERNELS	16

#define KERNEL_DIM 5

#define INPUT_IMAGE_CORE ""
#define OUTPUT_IMAGE_CORE ""
#define OUTPUT_IMAGE_REF ""

// Use the axi stream side-channel (TLAST,TKEEP,TUSR,TID)
#include <ap_axi_sdata.h>
typedef ap_axiu<8,2,5,6> uint_8_side_channel;
typedef ap_axis<8,2,5,6> int_8_side_channel;
typedef ap_uint<12> uint_12_type;
typedef ap_uint<10> uint_10_type;
typedef ap_uint<8> uint_8_type;


typedef ap_axiu<16,2,5,6> uint_16_side_channel;

struct FixedType{
  half data;
  bool last;
};

// Our IP core
void doConv(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream,  hls::stream<half> &kernel);//uint_10_type height, uint_10_type width, uint_4_type kernel_size){
