#!/usr/bin/python
import socket
import sys
import re
import threading
import time
 
tournamentSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

currentIndex = 0

pid = ""
dictionary = {}
startingTile = ""

#Used for the main client
def socketSetup(port):          # Create a socket object
    host = "localhost"          # Get local machine name
    port = int(port)            # Reserve a port for your service.
    tournamentSocket.connect((host, port))     # connect

#Used for the infinite read by the main client
def readlines(sock, recv_buffer=4096, delim='\n'):
    buffer = ''
    valid = True
    while valid:
        data = sock.recv(recv_buffer)
        #print data
        data = re.sub('[\x00\r]', '', data)
        buffer += data

        while buffer.find(delim) != -1:
            line, buffer = buffer.split('\n', 1)
            yield line
    return

#First part of the initial authentication
def joinTourn(tokens):
    print tokens
    usrIn = "JOIN " + raw_input("Enter tournament password : ") + "\r\n"
    print usrIn
    s.sendall(usrIn)
    print "finished send"

#Final part of the authentication
def identifyMyself(tokens):
    print tokens
    usrIn = "I AM " + raw_input("Enter usr password : ") + "\r\n"
    s.sendall(usrIn)

def printWelcome(tokens):
    print tokens
    pid = tokens[1]
    print pid

def newChallenge(tokens):
    print tokens
    global currentIndex
    dictionary[tokens[2]] = currentIndex
    currentIndex = currentIndex + 1

def beginRound(tokens):
    print tokens
    print "insert thread"

def regularPrint(tokens):
    print tokens

def firstTile(tokens):
    print tokens
    global startingTile
    startingTile += startingTile + tokens[3] + tokens[5] + tokens[6] + tokens[7]

def tileStack(tokens):
    print tokens
    print startingTile[0:5]
    for i in xrange(6, int(tokens[2])+6):
        print tokens[i]

def getMoveFromGame(tokens):
    print "ask for a move from the game"
    s.sendall("filled in")

def standardMoveMessage(tokens):
    print "regular move type"

def unplaceableTile(tokens):
    print "unplaceable tile found"

def badTileMessage(tokens):
    print "received bad tile placed"

def gameOverMessage(tokens):
    print "game over received"

def notifyGameOfMove(tokens):
    print "interpret message and send struct"
    moveTypes[tokens[6]](tokens)





moveTypes = {   "PLACED" : standardMoveMessage,
                "TILE" : unplaceableTile,
                "FORFEITED" : badTileMessage,    
                "PLAYER" : badTileMessage,    
}


#END and GAME need to handle multiple conditions
messageType = {     "THIS" : joinTourn,
                    "HELLO!" : identifyMyself,
                    "WELCOME" : printWelcome,
                    "NEW" : newChallenge,
                    "BEGIN" : beginRound,
                    "YOUR" : regularPrint,
                    "STARTING" : firstTile,
                    "THE" : tileStack,
                    "MATCH" : regularPrint,
                    "MAKE" : getMoveFromGame,
                    "GAME" : notifyGameOfMove,
                    "END" : regularPrint,
                    "PLEASE" : regularPrint,
                    "THANK" : regularPrint,
              }


socketSetup(50000)

for line in readlines(s):
    l = line
    tokens = l.split(" ")
    #print "selecting function"
    messageType[tokens[0]](tokens)