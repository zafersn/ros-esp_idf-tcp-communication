#!/usr/bin/env python

'''
@zafersn93
'''
import rospy
from std_msgs.msg import String
from geometry_msgs.msg  import Twist
from sensor_msgs.msg import Joy
import socket

TCP_IP = '192.168.1.28'
TCP_PORT = 3333
BUFFER_SIZE = 1024


class ESP_IDF_TEST(object):
	def __init__(self):
		self.blink=rospy.Subscriber("/turtle1/cmd_vel", Twist, self.blink_callback)
		self.vel_msg=Twist()		   
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


	def blink_callback (self, msg):
		self.vel_msg.linear.x=msg.linear.x
		self.vel_msg.linear.y=msg.linear.y
		self.vel_msg.linear.z=msg.linear.z

		self.vel_msg.angular.x = msg.angular.x
        	self.vel_msg.angular.y = msg.angular.y
	        self.vel_msg.angular.z = msg.angular.z
		
		if msg.linear.x == 2:
			self.yonVer(1)
		elif msg.linear.x == -2:
			self.yonVer(2)
		
		if msg.angular.z == 2:
			self.yonVer(3)
		elif msg.angular.z == -2:
			self.yonVer(4)

	
	def yonVer(self,Lnr):
		mData=str(int(Lnr))+"\0"	
		#rospy.loginfo(rospy.get_caller_id() + "I heard %s", mData)
		self.s.send(mData)
		

if __name__ == '__main__':
    try:
	rospy.init_node("esp_idf_test")
	r=rospy.Rate(50)
	lr=ESP_IDF_TEST()
	isOp=lr.s.connect((TCP_IP, TCP_PORT))
	if isOp:
		print("serialOpen")

	while not rospy.is_shutdown():
		data = lr.s.recv(BUFFER_SIZE)
		#if not data:
		rospy.loginfo(rospy.get_caller_id() + "I heard %s", data)
		r.sleep()
        
    except rospy.ROSInterruptException:
        pass 


