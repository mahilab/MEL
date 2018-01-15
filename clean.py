import os
import shutil

keep_files = ['bin/linux/sftp-config.json',
              'bin/linux/NiFpga_quadrature.lvbitx']

folders = ['build',
           'build/linux_mingw',
           'build/windows_mingw',
           'build/windows_msvc',
           'bin',
           'bin/linux',
           'bin/windows',
           'lib',
           'lib/linux',
           'lib/windows']

files = []


for folder in folders:
    try:
        files += [folder + "/" + f for f in os.listdir(folder)]
    except:
        os.mkdir(folder)
        files += [folder + "/" + f for f in os.listdir(folder)]

files = [f for f in files if f not in folders + keep_files]

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

