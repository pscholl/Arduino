#!/usr/bin/env python
import socket, threading
import sys
import time
import re
import signal


						


HOST = ''
PORT = 5000
TIMEOUT = 3
s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(4)
clients = [] #list of clients connected
lock = threading.Lock()
shutdown = 0
startTime = int(time.time())
receivedBytes = 0

testSize = 10

def handleSigTERM():
    shutdown = 1

signal.signal(signal.SIGTERM, handleSigTERM)


def store(length):
    global testSize
    global startTime
    global receivedBytes

    receivedBytes += length

    if testSize > 0:
        testSize -= 1
    else:
        print "Bytes per Second: " + str(receivedBytes / (time.time()-startTime))
        testSize = 50
        receivedBytes = 0
        startTime = time.time()
        
    

  

class chatServer(threading.Thread):
    def __init__(self, (socket,address)):
        threading.Thread.__init__(self)
        self.socket = socket
        self.address= address
        # self.socket.settimeout(TIMEOUT)

    def run(self):
	global receivedBytes
        lock.acquire()
        clients.append(self)
        lock.release()
        #print '%s:%s connected.' % self.address
        #print "connected"
        while True:
            a = 0
            try:
                a = self.socket.send("123456789012345")
                #a = self.socket.send("Don't look up while Dragons fly overhead!")
            except:
                if shutdown == 1:
                    break
                continue
            
            #while True:
            #    pass

            store(a)
        

            # sys.stdout.write(data)

        self.socket.close()
        #print '%s:%s disconnected.' % self.address
        #print "disconnect"
        lock.acquire()
        clients.remove(self)
        lock.release()

while True: # wait for socket to connect
    # send socket to chatserver and start monitoring
    if shutdown == 1:
        break
    chatServer(s.accept()).start()


s.close()

