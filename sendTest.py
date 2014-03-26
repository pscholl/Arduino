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


def getValues(line):
    r = re.compile('\{([0-9]+),(?:([0-9]+\.[0-9]+)|[0-9]+),(?:([0-9]+\.[0-9]+)|[0-9]+)\}')

    streamId = None
    val = None
    checkSum = None
    
    try:
        streamId = int(r.match(line).group(1))
        val =  float(r.match(line).group(2))
        checkSum =  float(r.match(line).group(3))
    except:
        return None
 
    approx_equal = lambda a, b, t: abs(a - b) < t
    
    if approx_equal(streamId + val, checkSum, 0.001):
        # print "got some good value"
        return len(line.strip())

    # print "got some BAD value"
    return None


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
        self.socket.settimeout(TIMEOUT)

    def run(self):
	global receivedBytes
        lock.acquire()
        clients.append(self)
        lock.release()
        #print '%s:%s connected.' % self.address
        #print "connected"
        while True:
            data = None
            try:
                data = self.socket.recv(1024)
            except:
                if shutdown == 1:
                    break
                continue
        
            if not data:
                break

            v = getValues(data)
            if not None == v:
                store(v)
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

