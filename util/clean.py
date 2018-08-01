# Evan Pezent (epezent@rice.edu)
# MEL Universal Clean Script
# 7/2018

# This script cleans the build and binary directories associated with MEL. If
# a particular directory does not exist, it is created.

import os
import shutil

keep = ['bin/sftp-config.json',
        'bin/NiFpga_quadrature.lvbitx',
        'bin/Release/myo32.dll',
        'bin/Release/myo64.dll'
        'lib/sftp-config.json']

folders = ['build',
           'bin',
           'lib']

files = []

keep    = ['../' + f for f in keep]
folders = ['../' + f for f in folders]

for folder in folders:
    try:
        files += [folder + "/" + f for f in os.listdir(folder)]
    except:
        print('Creating: ' + folder[3:])
        os.mkdir(folder)
        files += [folder + "/" + f for f in os.listdir(folder)]

files = [f for f in files if f not in folders + keep]

if not files:
    print('Already Clean')
else:
    for file in files:
        try:
            print('Removing: ' + file[3:])
            os.remove(file)
        except:
            pass
        try:
            print('Removing: ' + file[3:])
            shutil.rmtree(file)
        except:
            pass
    print('Finished Cleaning')

