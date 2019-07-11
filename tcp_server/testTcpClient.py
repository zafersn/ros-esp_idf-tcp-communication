#!/usr/bin/env python

import socket
import time


TCP_IP = '192.168.1.28'
TCP_PORT = 3333
BUFFER_SIZE = 1024
MESSAGE = "Hello, World ESP!"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
while 1:
	s.send(MESSAGE)
	data = s.recv(BUFFER_SIZE)
	print "received data:", data
	time.sleep(1)

s.close()


