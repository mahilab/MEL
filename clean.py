import os
import shutil

folders = ['build/mingw_linux',
           'build/mingw_win32',
           'build/msvc_win64',
           'bin/linux',
           'bin/win32',
           'lib/linux',
           'lib/win32']

keep_files = ['bin/linux/sftp-config.json',
              'bin/linux/NiFpga_quadrature.lvbitx']

files = []


for folder in folders:
    try:
        files += [folder + "/" + f for f in os.listdir(folder)]
    except:
        print "Couldn't locate /" + folder + ' folder'

files = [f for f in files if f not in keep_files]

if not files:
    print 'Already Clean'
else:
    for file in files:
        try:
            os.remove(file)
        except:
            pass
        try:
            shutil.rmtree(file)
        except:
            pass
    print 'Finished Cleaning'

