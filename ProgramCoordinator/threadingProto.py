#!/usr/bin/python
import datetime
import threading
import time

threadLocks = []
killedThreads = []


#Used for handling outward bound messages
msgNotRead = False
messageLock = threading.Lock()
outMessage = ""
outMessageID = 0

#Array to hold the current game message
inMessages = []
gameIDToThreadID = {}

tournamentSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
currentIndex = 0

pid = ""
startingTile = ""

#Used for the main client
def socketSetup(port):          # Create a socket object
    host = "localhost"          # Get local machine name
    port = int(port)            # Reserve a port for your service.
    tournamentSocket.connect((host, port))     # connect

#Used for the infinite read by the main client
def readlines(sock, blocking = 1, delim='\r\n'):
    sock.setblocking(blocking)
    returnString = ""
    currentChar = ''
    while (currentChar != delim):
        currentChar = tournamentSocket.recv(1)
        returnString += currentChar
    returnString = re.sub('\x00', '', returnString)
    return returnString

def sendlines(sock, msg, blocking = 1):
    sock.setblocking(blocking)
    sock.sendall(msg)

def createNewThread(gameID):
    global currentIndex
    t = myThread(1, gameID, currentIndex)
    currentIndex += 1
    threads.append(t)


def matchProtocol(tokens):
    print tokens
    msg = readlines(tournamentSocket, 0)
    t = msg.split(" ")
    firstTile(t)

    msg = readlines(tournamentSocket, 0)
    t = msg.split(" ")
    tileStack(t)

    msg = readlines(tournamentSocket, 0)
    t = msg.split(" ")
    regularPrint(t)

def authenticationProtocol(tokens):
    #insert code here
    tournPass = raw_input("Enter tournament password: ")
    usr = raw_input("Enter username: ")
    pswd = raw_input("Enter password: ")

    msg = "JOIN " + tournPass + "\r\n"
    print msg
    sendlines(tournamentSocket, msg)

    msg = readlines(tournamentSocket)
    print msg

    msg = "I AM " + usr + " " + pswd  + "\r\n"
    print msg
    sendlines(tournamentSocket, msg)


def challengeProtocol(tokens):
    #insert code here
    for i in xrange(int(tokens[6])):
        roundProtocol()

def roundProtocol():
    #insert code here
    msg = readlines(tournamentSocket)
    

def matchProtocol(tokens):
    #insert code here

def moveProtocol(tokens):
    #insert code here

def doWork():
    print "working"

def setupThreadStuff():
    tl = threading.Lock()
    global threadLocks
    threadLocks.append(t1)
    global inMessages
    inMessages.append("")
    global killedThreads
    killedThreads.append(False)

class myThread (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        setupThreadStuff()

    def run(self):
        while True:
            if(killedThreads[self.threadID]):
                print "exiting"
                break
            threadLocks[self.threadID].acquire()
            doWork()
            threadLocks[self.threadID].release()
            #time.sleep(.1)

    def getName(self):
        return self.name

    def getThreadID(self):
        return self.threadID



#############################################################################################################
####################################### Main Function #######################################################
#############################################################################################################

