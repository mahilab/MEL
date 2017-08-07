# Evan Pezent
# MELShare Demo (see mel_share.cpp)
# http://starship.python.net/crew/theller/ctypes/tutorial.html

# import required ctypes module
import ctypes
# load MELShare.dll
mel_share = ctypes.WinDLL("MELShare.dll")
# create arbitrary data types (type, size  setto match C++ data)
CharArray3 = ctypes.c_char * 3;
IntArray5 = ctypes.c_int * 5;
IntArray7 = ctypes.c_int * 7;
DoubleArray10 = ctypes.c_double * 10
# make instances of our data types
my_chars = CharArray3()
my_ints = IntArray5();
my_doubles = DoubleArray10()

# read and print our values
print "Read:"

result = mel_share.read_char_map("map0", my_chars, 3)
print "map0:    ",
if (result < 0): print 'ERROR:', result
for value in my_chars:
    print value,
print ' '

result = mel_share.read_int_map("map1", my_ints, 5)
print "map1:    ",
if (result < 0): print 'ERROR:', result
for value in my_ints:
    print value,
print ' '

result = mel_share.read_double_map("map2", my_doubles, 10)
print "map2:    ",
if (result < 0): print 'ERROR:', result
for value in my_doubles:
    print value,
print ' '

# modify and write our values
print "Wrote:"

my_chars = CharArray3('x','y','z')
result = mel_share.write_char_map("map0", my_chars, 3)
print "map0:    ",
if (result != 1): print 'ERROR:', result
for value in my_chars:
    print value,
print ' '

my_doubles[:] = [value * 2 for value in my_doubles]
result = mel_share.write_double_map("map1", my_doubles, 10)
print "map1:    ",
if (result < 0): print 'ERROR:', result
for value in my_doubles:
    print value,
print ' '

my_ints = IntArray7(1,1,2,3,5,8,13)
result = mel_share.write_int_map("map2", my_ints, 7)
print "map2:    ",
if (result < 0): print 'ERROR:', result
for value in my_ints:
    print value,
print ' '

# MELShare Error Codes
#  1 = successful read/write
# -1 = failed to open shared memory map
# -2 = failed to open mutex
# -3 = wait on mutex abandoned
# -4 = wait on mutex timed out
# -5 = wait on mutex failed
# -6 = failed to release mutex
# -7 = failed to close mutex handle
