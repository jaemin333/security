#간단한  TCP Connect형 port scanner
import pyfiglet
import sys
import socket
from datetime import datetime

ascii_banner = pyfiglet.figlet_format("PORT SCANNER")
print(ascii_banner)

target = input(str("Target IP: "))

#banner
print("_" * 50)
print("Scanning Target: " + target)
print("Scanning started at: " + str(datetime.now()))
print("_" * 50)

try:
	for port in range(1,65535): #valid port range
		s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		socket.setdefaulttimeout(0.5)

	result = s.connect_ex((target,port))
	if result == 0:
		print("[*] Port {} is open".formant(port))
	s.close()

except KeyboardInterrupt:
	print("\n Exiting :(")
	sys.exit()

except socket.error:
	print("\ Host not responding :(")
	sys.exit()
