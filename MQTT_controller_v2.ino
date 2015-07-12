
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

char conf[] = "/grid/c0/config";
char dat[] = "/grid/c0/data";
char name[] = "Controller0";

int temp=0;


float sensor0, sensor1, sensor2, sensor3;
float k0, k1, k2, k3;

int flag[4];
float value = 0.0;


//---------------------------------------------------------------------------------------------------------------------
// lcd print sensor format
void printSensor(int s0, int s1, int s2, int s3)
  {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("S0:");
      lcd.print(s0);
      
      lcd.setCursor(7,0);
      lcd.print("S1:");
      lcd.print(s1);
      
      lcd.setCursor(0,1);
      lcd.print("S2:");
      lcd.print(s2);
      
      lcd.setCursor(7,1);
      lcd.print("S3:");
      lcd.print(s3);
      
      //lcd.setCursor(0,0);
  }
//---------------------------------------------------------------------------------------------------------------------

void printGain(int s0, int s1, int s2, int s3)
  {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("k0:");
      lcd.print(s0);
      
      lcd.setCursor(7,0);
      lcd.print("k1:");
      lcd.print(s1);
      
      lcd.setCursor(0,1);
      lcd.print("k2:");
      lcd.print(s2);
      
      lcd.setCursor(7,1);
      lcd.print("k3:");
      lcd.print(s3);
      
     // lcd.setCursor(0,0);
  }

//---------------------------------------------------------------------------------------------------------------------
// String parsing function
String getValue(String data, char separator, int index)
{

    int maxIndex = data.length()-1;
    int j=0;
    String chunkVal = "";

    for(int i=0; i<=maxIndex && j<=index; i++)
    {
      chunkVal.concat(data[i]);

      if(data[i]==separator)
      {
        j++;

        if(j>index)
        {
          chunkVal.trim();
          return chunkVal;
        }    

        chunkVal = "";    
      }  
    }  
}

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
  
  mqtt.subscribe(conf);
  mqtt.subscribe("/grid/s0/data");
  mqtt.subscribe("/grid/s1/data");
  mqtt.subscribe("/grid/s2/data");
  mqtt.subscribe("/grid/s3/data");  

  
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
  if(topic == conf)
  {
    
       k0 = getValue(data, ',', 0).toFloat();
       //if (k0 != 0) mqtt.subscribe("/grid/s0/data");
       
       
       k1 = getValue(data, ',', 1).toFloat();
      // if (k1 != 0) mqtt.subscribe("/grid/s1/data");
       
       k2 = getValue(data, ',', 2).toFloat();
       //if (k2 != 0) mqtt.subscribe("/grid/s2/data");
       
       k3 = getValue(data, ',', 3).toFloat();
      // if (k3 != 0) mqtt.subscribe("/grid/s3/data");
  }  
  
  else if(topic == "/grid/s0/data")   
    {
      sensor0 = data.toFloat();
      flag[0] = 1;
    }
  else if(topic == "/grid/s1/data")  
    { 
      sensor1 = data.toFloat();
      flag[1] = 1;
    }
  else if(topic == "/grid/s2/data")   
    {
      sensor2 = data.toFloat();
      flag[2] = 1;
    }
  else if(topic == "/grid/s3/data") 
    {
      sensor3 = data.toFloat();
      flag[3] = 1;
    }
 
}


//---------------------------------------------------------------------------------------------------------------------

void mqttPublished(void* response)
{

}



void setup() {
  
  lcd.begin(16, 2);
  Serial.begin(19200);
  
  pinMode(A0, INPUT);
  
  esp.enable();
  lcd.print("esp enable");
  delay(500);
  esp.reset();
  lcd.clear();
   delay(500);
   
  while(!esp.ready());

  //lcd.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin(name, "", "", 120, 1)) //<--- NODE NAME HERE
  {
    lcd.println("ARDUINO: fail to setup mqtt");
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
  int del = millis()-temp;
//  int buttons= analogRead(A0);
  
  if(wifiConnected && mqttConnect && del>10) 
 {

    
    
  if( flag[0]==1 && flag[1]==1 && flag[2]==1 && flag[3]==1)
    {
        lcd.clear();
        flag[0]=0 ; flag[1]=0 ; flag[2]=0 ; flag[3]=0 ;  
        value = sensor0*k0 + sensor1*k1 + sensor2*k2 + sensor3*k3;
        
        char char_value[10];
        dtostrf(value, 4, 3, char_value);
      //  char_value[sizeof(char_value) - 1] = 0;
      
         mqtt.publish(dat, char_value);
     
         
         lcd.println(value);
    }
    
    
//    else if(buttons<10)
//    printSensor(sensor0, sensor1, sensor2, sensor3);
//    
//    
//    else if(buttons<300)
//    printGain(k0, k1, k2, k3);
    
temp = millis();

}
 
}




