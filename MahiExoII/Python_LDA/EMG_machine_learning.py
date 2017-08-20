# Load libraries
#import pandas
#import matplotlib.pyplot as plt
#from sklearn import model_selection
#from sklearn.metrics import confusion_matrix
#from sklearn.metrics import accuracy_score
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
#from perf_measure import perf_measure
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

#Create instance of datatype
myints2 = ctypes.c_int * cols;
# make instances of our data types
labels = myints2();

result = mel_share.read_int_map("label_share", labels, 2)
print "labels:    ",
if (result < 0): print 'ERROR:', result
for value in labels:
    print value,
print ' '

labels = labels.reshape(-1)

#Create instance of datatype
mydoubles = ctypes.c_double * rows*cols
# make instances of our data types
emg_doubles = mydoubles()

result = mel_share.read_double_map("trng_share", emg_doubles, trng_len)
print "trng_share:    ",
if (result < 0): print 'ERROR:', result
for value in emg_doubles:
    print value,
print ' '

emg_features = np.reshape(emg_doubles,(rows,cols))


# Load dataset
#Xdataset = pandas.read_csv('C:/Users/mep9/Desktop/Troy/EMG_X_features.csv')
#Ydataset = pandas.read_csv('C:/Users/mep9/Desktop/Troy/EMG_Y_labels.csv')



#split out validation (testing) dataset
X = emg_features
Y = labels
#validation_size = 0.25
#seed = 7
#X_train, X_validation, Y_train, Y_validation = model_selection.train_test_split(X, Y, test_size=validation_size, random_state=seed)


#	kfold = model_selection.KFold(n_splits=10, random_state=seed)
#	cv_results = model_selection.cross_val_score(model, X_train, Y_train, cv=kfold, scoring=scoring)
#	results.append(cv_results)

#make predictions on validation datatset
lda = LinearDiscriminantAnalysis()
lda.fit(X, Y)

#output in the coefficients of the linear decision function
coeff = lda.coef_
print(coeff)
coeff_mat_size = coeff.shape
c_row = coeff_mat_size[0]
c_col =coeff_mat_size[1]
coeff_len = c_row*c_col

result = mel_share.write_double_map("LDA_coeff", coeff, coeff_len)
print "LDA_coeff:    ",
if (result < 0): print 'ERROR:', result
for value in coeff:
    print value,
print ' '
#predictions = lda.predict(X_validation)
#cm = confusion_matrix(Y_validation, predictions)
#print(accuracy_score(Y_validation, predictions))
#print(cm)
#print(classification_report(Y_validation, predictions))

#perf_data = perf_measure(Y_validation, predictions)

#TP = float(perf_data[0])
#TN = float(perf_data[1])
#FP = float(perf_data[2])
#FN = float(perf_data[3])


#ACC = (TP + TN) / (TP + TN + FP + FN)
#SN = TP/(TP+FN)
#SP = TN/(TN+FP)
#P = TP/(TP+FP)

#print ("Acc = %s" % ACC)
#print("SN = %s" % SN)
#print("SP = %s" % SP)
#print("P = %s" % P)

