from sklearn.feature_selection import SelectFromModel
from sklearn import linear_model
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
#labels = labels.reshape(-1)

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

#read in X and y from MELShare
#use MELShare to read size, labels, emg data
X = emg_features
Y = labels

# We use linear regression to suit the 
lsqr = linear_model.LinearRegression()

# Set a minimum threshold of 0.25
sfm = SelectFromModel(lsqr, threshold=0.7)
sfm.fit(X, Y)
n_features = sfm.transform(X).shape[1]
n_trials = sfm.transform(X).shape[0]
x_size = n_features*n_trials
sel_feats = sfm.get_support(True)

print("Number of selected features: %s" % n_features)
print("selected features indicies: %s" % sel_feats)

X_transform = sfm.transform(X)
print(X_transform.shape)

#write new training data dimensions to MELShare in trng_size
#buffer of 2 ints
arraydims2 = myints(n_trials, n_features);
result = mel_share.write_int_map("trng_size2", arraydims2, 2)
print "trng_size2:    ",
if (result < 0): print 'ERROR:', result
for value in arraydims2:
    print value,
print ' '

#write indicies of selected features to MELShare in feat_id
#buffer of n_features ints
#Create instance of datatype
feat_ints = ctypes.c_int * n_features;
feat_sel_ints = feat_ints(sel_feats)
result = mel_share.write_int_map("feat_id", feat_sel_ints, n_features)
print "feat_id:    ",
if (result < 0): print 'ERROR:', result
for value in feat_sel_ints:
    print value,
print ' '

#write X-transform to melshare as the down-selected feature matrix feat_sel
#create double array buffer of n_features*n_trials
coeffArray = ctypes.c_double * x_size
coeff_doubles = coeffArray()

coeff_doubles = X_transform
result = mel_share.write_double_map("feat_sel_x", coeff_doubles, x_size)
print "feat_sel_x:    ",
if (result < 0): print 'ERROR:', result
for value in coeff_doubles:
    print value,
print ' '