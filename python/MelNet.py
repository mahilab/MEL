import socket, array, struct

class MelNet(object):

    def __init__(self, local_port, remote_port, remote_address, blocking = True):
        self.remote_host = (remote_address, remote_port)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind(('', local_port))
        if not blocking:
            self.socket.setblocking(0)

    def send_data(self, data):
        self.socket.sendto(
            array.array('d', data).tostring(),
            self.remote_host)

    def receive_data(self):
        try:
            (data, addr) = self.socket.recvfrom(4096)
            return array.array('d', data).tolist()
        except:
            return None

    def send_message(self, message):
        message = struct.pack('I', len(message))[::-1] + message
        self.socket.sendto(
            message,
            self.remote_host)

    def receive_message(self):
        try:
            (data, addr) = self.socket.recvfrom(4096)
            return data[4:]
        except:
            return None

    def request(self):
        self.send_message("request")

    def check_request(self):
        return self.receive_message() == "request"

#==============================================================================
# Example:
#==============================================================================
# import sys

# if len(sys.argv) > 3:
#     mode = sys.argv[1]
#     ID = sys.argv[2]
#     remote_address = sys.argv[3]
#     print 'Mode:', mode
#     print 'ID:', ID
#     print 'Remote Address:', remote_address

#     if mode == 'blocking' and ID == 'A':
#         melnet = MelNet(55001, 55002, remote_address)
#         print melnet.receive_data()
#         melnet.send_data([5, 6, 7, 8, 9])
#         message = melnet.receive_message()
#         print message
#         melnet.send_message(message + ', World!')

#     if mode == 'blocking' and ID == 'B':
#         melnet = MelNet(55002, 55001, remote_address)
#         melnet.send_data([0, 1, 2, 3, 4])
#         print melnet.receive_data()
#         melnet.send_message("Hello")
#         print melnet.receive_message()

#     if mode == 'nonblocking' and ID == 'A':
#         melnet = MelNet(55001, 55002, remote_address, False)
#         while not melnet.check_request():
#             print 'Waiting to Feed B'
#         print 'Feeding B'
#         melnet.send_data([0,1,2,3,4]);
#         while not melnet.check_request():
#             print 'Waiting to Feed B'
#         print "Feeding B again, he's really hungy!"
#         melnet.send_data([5,6,7,8,9])

#     if mode == 'nonblocking' and ID == 'B':
#         melnet = MelNet(55002, 55001, remote_address)
#         melnet.request()
#         print melnet.receive_data()
#         raw_input('Press Enter for More!')
#         melnet.request()
#         print melnet.receive_data()




