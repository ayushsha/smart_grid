import paho.mqtt.client as mqtt
#from time import sleep
#from pymatbridge import Matlab
import numpy as np



def on_connect(client, userdata, rc):
	print ("Connect with result" + str(rc))
	if client.subscribe("/grid/c0/data"): print "subscribed to controller1"
	if client.subscribe("/grid/c1/data"): print "subscribed to controller2"
	if client.subscribe("/grid/c2/data"): print "subscribed to controller3"
	if client.subscribe("/grid/c3/data"): print "subscribed to controller4"

	# Sending output to controller
	for i in range(4):
		address = "/grid/s" + str(i) + "/data"
		client.publish(address, np.asscalar(x1[i]))

def on_message(client, userdata, msg):
	global count
	if msg.topic == "/grid/c0/data"	:
		u[0] = float(msg.payload)
		#print "u[0] = " + msg.payload
		count = count+1

	if msg.topic == "/grid/c1/data"	:
		u[1] = float(msg.payload)
		#print "u[1] = " + msg.payload
		count = count+1

	if msg.topic == "/grid/c2/data"	:
		u[2] = float(msg.payload)
		#print "u[2] = " + msg.payload
		count = count+1

	if msg.topic == "/grid/c3/data"	:
		u[3] = float(msg.payload)
		#print "u[3] = " + msg.payload
		count = count+1



u = np.matrix((0.0,0.0,0.0,0.0)).T
count = 0
itr = 0
delta_t = 0.00015
x1 = np.matrix((0.5,0.5,0.5,0.5)).T
x2 = np.matrix([0.0,0.0,0.0,0.0]).T

A = np.matrix([[  175.9,   176.8,   511.0 ,  1036.0 ],
        [ -350.0 ,    -0.0 ,    -0.0 ,    -0.0 ],
        [ -544.2,  -474.8,  -408.8,  -828.8],
        [ -119.7,  -554.6,  -968.8, -1077.5]])

B = 1000*np.matrix([[  8.00000000e-04,   3.34200000e-01,   5.25100000e-01,
          -1.03600000e+00],
        [ -3.50000000e-01,  -0.00000000e+00,  -0.00000000e+00,
          -0.00000000e+00],
        [ -6.93000000e-02,  -6.61000000e-02,  -4.20100000e-01,
          -8.28800000e-01],
        [ -4.34900000e-01,  -4.14200000e-01,  -1.08700000e-01,
          -1.07750000e+00]])




# creating mqtt client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)
client.loop_start()

while True:
	if  count ==4:
		count = 0 # count reset
		#print "u: ",u
		# State Space form
		x2 = x1 + delta_t*( np.dot(A,x1) + np.dot(B,u))

		# Reassigning output value
		x1 = x2
		itr = itr + 1

		
		# Sending output to sensors
		for i in range(4):
			address = "/grid/s" + str(i) + "/data"
			client.publish(address,"{:.2f}".format(np.asscalar(x2[i])) )
		print "x: ",x2, itr



	
	