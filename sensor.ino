
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include <LiquidCrystal.h>
#include<stdlib.h>



LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, 8);
MQTT mqtt(&esp);




//---------------------------------------------------------------------------------------------------------------------

boolean wifiConnected = false;
boolean mqttConnect = false;
float x0,x1,x2,x3;
int button = 13;

//---------------------------------------------------------------------------------------------------------------------

//wifi call back
void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      lcd.clear();
      lcd.println("WIFI CONNECTED");
      mqtt.connect("172.26.172.42", 1883, false);
      wifiConnected = true;
      //or mqtt.connect("host", 1883); /*without security ssl*/
    } else {
      wifiConnected = false;
      mqtt.disconnect();
    }
   }
 }

//---------------------------------------------------------------------------------------------------------------------

//mqtt connect callback
void mqttConnected(void* response)
{ 
  mqttConnect = true;
  lcd.clear();
  lcd.print("Connected");
  
  mqtt.subscribe("/grid/s3/in");
}

//---------------------------------------------------------------------------------------------------------------------

//mqtt disconnect callback
void mqttDisconnected(void* response)
{
  mqttConnect = false;
  
  lcd.clear();
  lcd.print("Disconnected!");
}

//---------------------------------------------------------------------------------------------------------------------

// mqtt data calback
void mqttData(void* response)
{
  RESPONSE res(response);
  String topic = res.popString();
  String data = res.popString(); 

  
  char tab2[50];
  strncpy(tab2, data.c_str(), sizeof(tab2));
  tab2[sizeof(tab2) - 1] = 0; 
  mqtt.publish("/grid/s3/data", tab2);
  
  lcd.clear();
  lcd.print("x=");
  lcd.print(tab2);
}


//---------------------------------------------------------------------------------------------------------------------

void mqttPublished(void* response)
{

}

//---------------------------------------------------------------------------------------------------------------------


void setup() {
  
  lcd.begin(16, 2);
  Serial.begin(19200);
  
  esp.enable();
  lcd.print("esp enable");
  delay(500);
  esp.reset();
  lcd.clear();
   delay(500);
   
  while(!esp.ready());

  //lcd.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin("Sensor3", "", "", 120, 1))  //<---NODE NAME HERE
  {
    lcd.println("failed to setup mqtt");
    while(1);
  }
  
    //lcd.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

/*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  lcd.println("Connecting...");
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect("iitk","");
  //lcd.println("ARDUINO: system started");
  //lcd.clear();
}

//---------------------------------------------------------------------------------------------------------------------
void loop() {
  esp.process();
 
  if(wifiConnected) {
   }
}




