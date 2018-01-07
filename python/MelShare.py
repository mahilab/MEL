import mmap
import struct, array
from Mutex import Mutex

class MelShare(object):

    def __init__(self, name, max_size = 256):
        self.name = name
        self.mutex = Mutex(name + '_mutex')
        self.shm = mmap.mmap(0, max_size, name, mmap.ACCESS_WRITE)

    def write_data(self, data):
        self.shm.seek(0)
        size = len(data) * 8
        self.mutex.try_lock()
        self.shm.write(struct.pack('I', size))
        self.shm.write(array.array('d', data).tostring())
        self.mutex.release()

    def read_data(self):
        self.shm.seek(0)
        self.mutex.try_lock()
        size = struct.unpack('I', self.shm.read(4))[0]
        data = []
        if size > 0:
            data = array.array('d', self.shm.read(size)).tolist()
        self.mutex.release()
        return data

    def write_message(self, message):
        self.shm.seek(0)
        message += '\0'
        size = len(message)
        self.mutex.try_lock()
        self.shm.write(struct.pack('I', size))
        self.shm.write(message)
        self.mutex.release()

    def read_message(self):
        self.shm.seek(0)
        self.mutex.try_lock()
        size = struct.unpack('I', self.shm.read(4))[0]
        message = ''
        if size > 0:
            message = self.shm.read(size - 1)
        self.mutex.release()
        return message

    def get_size(self):
        self.shm.seek(0)
        self.mutex.try_lock()
        size = struct.unpack('I', self.shm.read(4))[0]
        self.mutex.release()
        return size

#==============================================================================
# Example:
#==============================================================================
# import sys
#
# if len(sys.argv) > 1:
#     ID = sys.argv[1]
#     if ID is 'A':
#         ms = MelShare('melshare')
#         ms.write_message("Hello from Python! Please send me some data.")
#         raw_input("Press Enter after running B ...")
#         print(ms.read_data())
#     elif ID is 'B':
#         ms = MelShare('melshare')
#         print(ms.read_message())
#         ms.write_data([4.0, 5.0, 6.0])


