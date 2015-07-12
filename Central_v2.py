import paho.mqtt.client as mqtt
from time import sleep

k = [(0.0, 1.3877, 0.0, 0.0),
 		(0.0, -0.7842, -1.3757, 0.0), 
 		(0.2672, 0.0, 1.6004, 0.0), 
 		(1.4442, 0.0, 0.0, 0.8173)]


def on_connect(client, userdata, rc):
	print ("Connect with result" + str(rc))
	

client = mqtt.Client()
client.on_connect = on_connect

client.connect("localhost", 1883, 60)

for i in range(0, 4):
	command = str(k[i][0])  + "," + str(k[i][1])  + "," + str(k[i][2])   + ","+ str(k[i][3]) #formats the command
	path = "/grid/c" + str(i) + "/config" # defining path to publish
	print ("published to: " + path + " " + "K1: " + command)
	client.publish(path, command) # publishes to controller i with command

client.disconnect()
print ("disconnected!")


