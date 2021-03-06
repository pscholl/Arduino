import socket, threading
import sys

HOST = ''
PORT = 10002 

s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(4)
clients = [] #list of clients connected
lock = threading.Lock()


class chatServer(threading.Thread):
    def __init__(self, (socket,address)):
        threading.Thread.__init__(self)
        self.socket = socket
        self.address= address

    def run(self):
        lock.acquire()
        clients.append(self)
        lock.release()
        #print '%s:%s connected.' % self.address
        print "connected"
        while True:
            data = self.socket.recv(1024)
            if not data:
                break
            sys.stdout.write(data)
        self.socket.close()
        #print '%s:%s disconnected.' % self.address
        print "disconnect"
        lock.acquire()
        clients.remove(self)
        lock.release()

while True: # wait for socket to connect
    # send socket to chatserver and start monitoring
    chatServer(s.accept()).start()
