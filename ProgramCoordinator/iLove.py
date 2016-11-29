#!/usr/bin/python           # This is client.py file
import sys
import socket               # Import socket module

s = socket.socket()         # Create a socket object
host = sys.argv[1]          # Get local machine name
port = int(sys.argv[2])     # Reserve a port for your service.

s.connect((host, port))
print s.recv(1024)      #THIS IS SPARTA

#Tournament Parameters
tournamentPassword = sys.argv[3]
username = sys.argv[4]
password = sys.argv[5]

#AUTHENTICATION PROTOCOL
buffer = "JOIN "+tournamentPassword
s.send(buffer)
print buffer

print s.recv(1024)


s.close
