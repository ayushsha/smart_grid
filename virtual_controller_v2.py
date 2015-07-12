import paho.mqtt.client as mqtt
import numpy as np

def on_connect(client, userdata, rc):
	print ("Connect with result" + str(rc))
	if client.subscribe("/grid/s0/data"): print "subscribed to sensor1"
	if client.subscribe("/grid/s1/data"): print "subscribed to sensor2"
	if client.subscribe("/grid/s2/data"): print "subscribed to sensor3"
	if client.subscribe("/grid/s3/data"): print "subscribed to sensor4"


def on_message(client, userdata, msg):
	global count
	if msg.topic == "/grid/s0/data"	:
		x[0] = float(msg.payload)
		#print "x[0] = " + msg.payload
		count = count+1

	if msg.topic == "/grid/s1/data"	:
		x[1] = float(msg.payload)
		#print "x[1] = " + msg.payload
		count = count+1

	if msg.topic == "/grid/s2/data"	:
		x[2] = float(msg.payload)
		#print "x[2] = " + msg.payload
		count = count+1

	if msg.topic == "/grid/s3/data"	:
		x[3] = float(msg.payload)
		#print "x[3] = " + msg.payload
		count = count+1


x = np.matrix([0.0,0.0,0.0,0.0]).T
u = np.matrix([0.0,0.0,0.0,0.0]).T
count = 0		
k = np.matrix([(0.0, 1.3877, 0.0, 0.0),
 		(0.0, -0.7842, -1.3757, 0.0), 
 		(0.2672, 0.0, 1.6004, 0.0), 
 		(1.4442, 0.0, 0.0, 0.8173)])

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)
client.loop_start()

while True:
	if  count ==4:
		print "count: " + str(count)
		count = 0 # count reset
		print "x:", x

		u = np.dot(k,x)

		# Sending output to Virtual Grid
		for i in range(4):
			address = "/grid/c" + str(i) + "/data"
			client.publish(address, "{0}".format(np.asscalar(u[i]))	)

		print "u:", u	









