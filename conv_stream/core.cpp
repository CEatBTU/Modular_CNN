#include "core.h"
/*
 * Core that apply a 3x3(Configurable) 2d Convolution, Erode, Dilate on grayscale images
 * http://www.xilinx.com/support/documentation/sw_manuals/xilinx2014_1/ug902-vivado-high-level-synthesis.pdf
 * */
void doConv(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream,  hls::stream<half> &kernel){
#pragma HLS INTERFACE axis port=inStream
#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE axis port=kernel
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS

	FixedType dataOutSideChannel;
	FixedType currPixelSideChannel;
	//uint_16_side_channel currKernelPixel;
	half currHalfKernel;
	//conv temporal;
	//do the conv for each image
	for(uint_8_type idxImg=0;idxImg<NUM_IMAGES;idxImg++){	//may be possible to join both for loops in num_input_images*num_kernels
		for(uint_8_type idxKernel=0;idxKernel<NUM_KERNELS;idxKernel++){
//#pragma HLS PIPELINE

			// Index used to keep track of row,col
			uint_10_type idxCol = 0;
			uint_10_type idxRow = 0;
			int pixConvolved = 0;//4bytes
			// Calculate delay to fix line-buffer offset
			uint_10_type waitTicks = (IMG_WIDTH_OR_COLS*(KERNEL_DIM-1)+KERNEL_DIM)/2;
			uint_10_type countWait = 0;

			// Defining the line buffer and setting the inter dependency to false through pragmas
			hls::LineBuffer<KERNEL_DIM,IMG_WIDTH_OR_COLS,half> lineBuff; //1 byte
			hls::Window<KERNEL_DIM,KERNEL_DIM,half> kernelTemp;	//2bytes
			hls::Window<KERNEL_DIM,KERNEL_DIM,half> window;		//2bytes

			//convolve the whole image with kernel n
			for (uint_12_type idxPixel = 0; idxPixel < (IMG_WIDTH_OR_COLS*IMG_HEIGHT_OR_ROWS); idxPixel++){
//#pragma HLS PIPELINE

				//read the complete image once every cycle (one image for every kernel, then second image for every kernel, etc)
				if(idxKernel == 0){
					// Read and cache (Block here if FIFO sender is empty)
					currPixelSideChannel = inStream.read();

					// Get the pixel data
					half pixelIn = currPixelSideChannel.data;

					// Put data on the LineBuffer
					lineBuff.shift_up(idxCol);
					lineBuff.insert_top(pixelIn,idxCol); // Will put in val[2] of line buffer (Check Debug)
				}
				// Put data on the window and multiply with the kernel
				for (int idxWinRow = 0; idxWinRow < KERNEL_DIM; idxWinRow++){
					for (int idxWinCol = 0; idxWinCol < KERNEL_DIM; idxWinCol++){
#pragma HLS PIPELINE
						//fill the kernel every first time
						if(idxPixel==0){
							//currKernelPixel = kernel.read();

							//currHalfKernel = ((static_cast<FixedType>(currKernelPixel.data)) - 128 )/255;
							//temporal.i=currKernelPixel.data;
							currHalfKernel = kernel.read();
							kernelTemp.insert(currHalfKernel, idxWinRow, idxWinCol);
						}
						// idxWinCol + pixConvolved, will slide the window ...
						//FixedType val = static_cast<FixedType>(lineBuff.getval(idxWinRow,idxWinCol+pixConvolved));
						half val = lineBuff.getval(idxWinRow,idxWinCol+pixConvolved);
						//short val = (short)lineBuff.getval(idxWinRow,idxWinCol+pixConvolved);

						// Multiply kernel by the sampling window
						//val = (short)kernel[(idxWinRow*KERNEL_DIM) + idxWinCol ] * val;
						val = kernelTemp.getval(idxWinRow, idxWinCol) * val;
						window.insert(val,idxWinRow,idxWinCol);
					}
				}

				// Avoid calculate out of the image boundaries and if we can convolve
				half valOutput = 0;
				if ((idxRow >= KERNEL_DIM-1) && (idxCol >= KERNEL_DIM-1)){
					// Convolution
					//valOutput = sumWindow(&window);

					// Iterate on the window multiplying and accumulating the kernel and sampling window
					for (int idxSumRow = 0; idxSumRow < KERNEL_DIM; idxSumRow++){
						for (int idxSumCol = 0; idxSumCol < KERNEL_DIM; idxSumCol++){
#pragma HLS PIPELINE
							valOutput += window.getval(idxSumRow,idxSumCol);
						}
					}

					//valOutput = valOutput / 8;

					//RELu
					// Avoid negative values
					if (valOutput < 0){
						valOutput = 0;
					}

					pixConvolved++;
				}

				// Calculate row and col index
				if (idxCol < IMG_WIDTH_OR_COLS-1){
					idxCol++;
				}
				else{
					// New line
					idxCol = 0;
					idxRow++;
					pixConvolved = 0;
				}
				countWait++;
				if (countWait > waitTicks){
					//dataOutSideChannel.data = valOutput;
					//dataOutSideChannel.data = static_cast<ap_uint<16> >(valOutput);

//					dataOutSideChannel.keep = currPixelSideChannel.keep;
//					dataOutSideChannel.strb = currPixelSideChannel.strb;
//					dataOutSideChannel.user = currPixelSideChannel.user;
					dataOutSideChannel.data = valOutput;
					dataOutSideChannel.last = 0;
//					dataOutSideChannel.id = currPixelSideChannel.id;
//					dataOutSideChannel.dest = currPixelSideChannel.dest;
					// Send to the stream (Block if the FIFO receiver is full)
					outStream.write(dataOutSideChannel);
					//sentPixels++;
				}
			}

			// Now send the remaining zeros (Just the (Number of delayed ticks)
			for (countWait = 0; countWait < waitTicks; countWait++){
				dataOutSideChannel.data = 0;
//				dataOutSideChannel.keep = currPixelSideChannel.keep;
//				dataOutSideChannel.strb = currPixelSideChannel.strb;
//				dataOutSideChannel.user = currPixelSideChannel.user;
				// Send last on the last item
				if ((countWait < waitTicks - 1)&&(idxImg!=NUM_IMAGES-1)){
					dataOutSideChannel.last = 0;
				}
				else{
					dataOutSideChannel.last = 1;
				}
//				dataOutSideChannel.id = currPixelSideChannel.id;
//				dataOutSideChannel.dest = currPixelSideChannel.dest;
				// Send to the stream (Block if the FIFO receiver is full)
				outStream.write(dataOutSideChannel);
			}
		}
	}
}
