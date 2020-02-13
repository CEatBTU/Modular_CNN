#include "core.h"
#include <stdio.h>

void doMaxpool(hls::stream<FixedType> &inStream, hls::stream<FixedType> &outStream){

#pragma HLS INTERFACE axis port = inStream
#pragma HLS INTERFACE axis port = outStream
#pragma HLS INTERFACE s_axilite port = return bundle=CRTL_BUS

	//igual al num de imágenes
	for(uint_8_type idImg=0; idImg<NUM_IMAGES; idImg++){
#pragma HLS unroll

		//Defining the line buffer and setting the inter dependency to false through pragmas
		hls::LineBuffer<WINDOW_POOL_DIM, IMG_WIDTH_OR_COLS, half> lineBuff;
		hls::Window<WINDOW_POOL_DIM, WINDOW_POOL_DIM, half> window;
		//Index used to keep track of row, col
		int idxCol = 0;

		FixedType dataOutSideChannel;
		FixedType currPixelSideChannel;

		short count = 0;
		for (short idxPixel = 0; idxPixel < (IMG_WIDTH_OR_COLS*IMG_HEIGHT_OR_ROWS); idxPixel++){
#pragma HLS PIPELINE
			//Read and cache (Block here if FIFO sender is empty)
			currPixelSideChannel = inStream.read();

			//Get the pixel data
			half pixelIn = currPixelSideChannel.data;

			//Put data on the LineBuffer
			lineBuff.shift_up(idxCol);
			lineBuff.insert_top(pixelIn,  idxCol);	///Will put in val[2] of the line buffer (Check Debug)

			if(count == (IMG_WIDTH_OR_COLS * WINDOW_POOL_DIM)-1){	//las primeras dos lineas están llenas

				for(uint_8_type i=0; i<IMG_WIDTH_OR_COLS;i+=STRIDE){ //llenar el arreglo para encontra el maximo

					for(uint_4_type helperRow=0; helperRow<WINDOW_POOL_DIM; helperRow++){
						for(uint_4_type helperCol=0; helperCol<WINDOW_POOL_DIM;helperCol++){
							//val[WINDOW_POOL_DIM*helperRow + helperCol]=(short)lineBuff.getval((WINDOW_POOL_DIM-1)-helperRow,i+helperCol);
							window.insert(lineBuff.getval((WINDOW_POOL_DIM-1)-helperRow,i+helperCol),
									helperRow,
									helperCol);
						}
					}

					//el arreglo ya está lleno
					//short valOut= maxWindow(val);

					half valOut = maxWindow(&window);
					//enviralo a outputStream

					dataOutSideChannel.data = valOut;
//					dataOutSideChannel.keep = currPixelSideChannel.keep;
//					dataOutSideChannel.strb = currPixelSideChannel.strb;
//					dataOutSideChannel.user = currPixelSideChannel.user;
					if(idxPixel<(IMG_WIDTH_OR_COLS*IMG_HEIGHT_OR_ROWS)-1){
						dataOutSideChannel.last = 0;
					}
					else{
						dataOutSideChannel.last=1;
					}
//					dataOutSideChannel.id = currPixelSideChannel.id;
//					dataOutSideChannel.dest = currPixelSideChannel.dest;
					// Send to the stream (Block if the FIFO receiver is full)
					outStream.write(dataOutSideChannel);

				}
				count = 0;
			}
			else{
				count++;
			}
			// Calculate row and col index
			if (idxCol < IMG_WIDTH_OR_COLS-1){
				idxCol++;
			}
			else{
				// New line
				idxCol = 0;
			}

		}
	}
}
half maxWindow(hls::Window<WINDOW_POOL_DIM,WINDOW_POOL_DIM, half> *window){
	half maxVal = -1;
	// Look for the smalles value on the array
	for (int idxRow = 0; idxRow < WINDOW_POOL_DIM; idxRow++){
		for (int idxCol = 0; idxCol < WINDOW_POOL_DIM; idxCol++){
			half valInWindow;
			valInWindow = window->getval(idxRow,idxCol);
			if (valInWindow > maxVal)
				maxVal = valInWindow;
		}
	}
	return maxVal;
}
