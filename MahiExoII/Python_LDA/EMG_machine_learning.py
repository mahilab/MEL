# Load libraries
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
import ctypes
import numpy as np

#load MELShare
mel_share = ctypes.WinDLL("MELShare.dll")

#Create instance of datatype
myints = ctypes.c_int * 2;
# make instances of our data types
arraydims = myints();

result = mel_share.read_int_map("trng_size", arraydims, 2)
print "array dimensions:    ",
if (result < 0): print 'ERROR:', result
for value in arraydims:
    print value,
print ' '

rows = arraydims[0]
cols = arraydims[1]
trng_len = rows*cols
print(rows)

#Create instance of datatype
myints2 = ctypes.c_int * rows;
# make instances of our data types
labels = myints2();

result = mel_share.read_int_map("label_share", labels, rows)
print "labels:    ",
if (result < 0): print 'ERROR:', result
for value in labels:
    print value,
print ' '

#labels = labels.reshape(-1)

#Create instance of datatype
mydoubles = ctypes.c_double * rows*cols
# make instances of our data types
emg_doubles = mydoubles()

result = mel_share.read_double_map("trng_share", emg_doubles, trng_len)
print "trng_share:    ",
if (result < 0): print 'ERROR:', result
#for value in emg_doubles:
 #   print value,
for i in range(0, trng_len):
	print str(emg_doubles[i]),
print ' '

emg_features = np.reshape(emg_doubles,(rows,cols))

#split out validation (testing) dataset
X = emg_features
Y = labels

#make predictions on validation datatset
lda = LinearDiscriminantAnalysis()
lda.set_params(solver='lsqr')
lda.fit(X, Y)

#output in the coefficients of the linear decision function
coeff = lda.coef_
print(coeff)
print(coeff.shape)
coeff_mat_size = coeff.shape
c_row = coeff_mat_size[0]
c_col =coeff_mat_size[1]
print(c_row)
print(c_col)
coeff_len = c_row*c_col

coeffArray = ctypes.c_double * coeff_len
coeff_doubles = coeffArray()

#print(coeffArray.shape)
print(coeff.shape)
for i in range(0,coeff_len):
	print str(coeff_doubles[i]) + ' ',
	coeff_doubles[i] = coeff[0][i]

#coeff = np.vectorize(coeff)
#Figure out how to make same size to read into MELShare buffer
#my_doubles[:] = [value * 2 for value in my_doubles]

result = mel_share.write_double_map("LDA_coeff", coeff_doubles, coeff_len)
print "LDA_coeff:    ",
if (result < 0): print 'ERROR:', result
for value in coeff:
    print value,
print ' '