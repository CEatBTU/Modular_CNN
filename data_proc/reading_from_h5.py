import h5py
import numpy as np

f = h5py.File('../cnn_accel_fpga.h5', 'r')
list(f.keys())

#convolutional layer
dset = f['layer_conv1']
#list(dset.keys())
conv_layer=dset['layer_conv1']
#list(dset1.keys())

    #bias
conv_bias=conv_layer['bias:0']
#dset2.shape
conv_bias[0] # 16   # dentro de bias --> conv layer

    #kernel
conv_kernel=conv_layer['kernel:0']
conv_kernel[0,0,0,0] #5, 5, 1, 16  # [1 2 3 4] 1 y 2 primero (25 veces) una vez que se llega a [4,4,0,0] el siguiente es [0,0,0,1]

# la única manera de saber si 1 o 2 es LSB es prueba y error. Sospecho que LSB es 2

# Fully connected 1 
dset2 = f['dense']
fc_1= dset2['dense']

    #bias
fc_1_bias = fc_1['bias:0']
fc_1_bias[0]    #128

    #weights
fc_1_weights=fc_1['kernel:0']
fc_1_weights[0,0]       #3136,128 #[1 2] 1 son entradas, 2 son salidas. LSB es 1  i.e. cambiar primero [0,0] [1,0] [2,0]....


# Fully connected 2
dset3 = f['dense_1']
fc_2= dset3['dense_1']

    #bias
fc_2_bias = fc_2['bias:0']
fc_2_bias[0]    # 10 

    #weights
fc_2_weights=fc_2['kernel:0']
fc_2_weights[0,0]       #128,10   # [1 2] 1 son entradas, 2 son salidas. LSB es 1  i.e. cambiar primero [0,0] [1,0] [2,0]....

file = open("kernels.h", "w")
file.write("float kernel[400] = {")
i=0
j=0
k=0
for i in range(16):
    for j in range(5):
        for k in range(5):
            file.write('{}'.format(conv_kernel[j][k][0][i]))
            
            if i != 399:
                file.write(", ")
                
file.write("};")
file.close()

file = open("bias_1.h", "w")
file.write("float bias[128] = {")
i=0
for i in range(128):
    file.write('{}'.format(fc_1_bias[i]))
    if i != 127:
        file.write(", ")
file.write("};")
file.close()

file = open("weights.h", "w")
file.write("float weights[401408] = {")
i=0
j=0
helper=0
for i in range(3136):
    for j in range(128):
        file.write('{}'.format(fc_1_weights[j][i]))
    
        if helper != 401407:
            file.write(", ")
        
        helper+=1
    
file.write("};")
file.close()

file = open("bias_2.h", "w")
file.write("float bias_2[10] = {")
i=0
for i in range(10):
    file.write('{}'.format(fc_2_bias[i]))
    if i != 9:
        file.write(", ")
file.write("};")
file.close()

file = open("weights_2.h", "w")
file.write("float weights_2[1280] = {")
i=0
j=0
helper=0
for i in range(128):
    for j in range(10):
        file.write('{}'.format(fc_2_weights[j][i]))
    
        if helper != 1279:
            file.write(", ")
        
        helper+=1
    
file.write("};")
file.close()

