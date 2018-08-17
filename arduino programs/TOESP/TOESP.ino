#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFiMulti.h>
//#include "ThingSpeak.h"

ESP8266WiFiMulti wifiMulti;

#define wait_time 10000 // In milliseconds
#define upload_freq 15000 // In milliseconds
//1909.7687n7506.1303e

unsigned long myChannelNumber = 230325;
const char * myWriteAPIKey = "F6EBNSPJ00KBG0G5";

volatile unsigned char upload_enable = 1;

// Scheduler essentials
unsigned long schedule_time = 0;
bool copy_flag = true;
// Hold 1 min lemme checkok 

// IoT essentials
String apiKey = "myapikey"; // paste your Api key here for this value follow steps ahead....
const char* server_tp = "api.thingspeak.com"; // "184.106.153.149" or api.thingspeak.com

WiFiClient client;

void setup() 
{
  wifi_init();
  Serial.begin(9600);
 // ThingSpeak.begin(client);
 
}

void loop() 
{
  if (copy_flag == true)
  {
    schedule_time = millis();
    copy_flag = false;
  }

  if ((millis() - schedule_time) >= upload_freq)
  {
    upload_data(); // Gets and uploads flow and totalizer
  copy_flag = true;
  }
}

void upload_data (void)
{
  if (client.connect(server_tp,80) && (upload_enable))
  {
    
    String postStr = apiKey;
    
    postStr +="&field1=";//Voltage value for testing....
    postStr+="1000";
    //Serial.println("Waiting for serial");
      
   String readString;

       //Accept data at 0,1 Rx,Tx from Arduino Flora
    int av=Serial.available();
    if (av>0)
    {
      
      for(int i=0;i<av;++i)
      {
        int c=Serial.read();
       readString+=(char(c));
        }
        
    }
    
  
     postStr +="&field2=";//latitude
    
     String latitude=readString.substring(0,6);
     String longitude=readString.substring(6,12);
     
    postStr += latitude;
  
     postStr +="&field3=";//longitude
    postStr +=longitude;
   
    postStr+="&field4=";//Device Id is
    postStr+="444";
   
    Serial.print("postSTR="+postStr);

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+ apiKey +"\n");
    
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    //ThingSpeak.setLatitude(latitude.substring(0,5).toFloat());
//ThingSpeak.setLongitude(longitude.substring(0,5).toFloat());
    
  }
  client.stop();
 
//ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  

}

void wifi_init (void)
{
  unsigned char available_networks = 0;
  unsigned long timeout = 0;

  WiFi.mode(WIFI_STA); //Operating in station

  //Autoselects AP
  wifiMulti.addAP("MyWifiName", "MyWifiPassword");
 
 // WiFi.begin(ssid, password);
  //Serial.println("ATTEMPTING NEW CONNECTION TO GIVEN AP"); // FOR DEBUG ONLY
  // Wait for connection
  timeout = millis();
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(1);
    if ((millis() - timeout) > wait_time)
    {
      return;
    }
  }
  // Check if successful connection established to given SSID
  if(wifiMulti.run() == WL_CONNECTED)
  {
    if (WiFi.getAutoConnect() == false) WiFi.setAutoConnect(true); //Configure module to automatically connect on power on to the last used access point. 
    //Serial.println("WIFI CONNECTED SUCCESSFULLY"); // FOR DEBUG ONLY
  }
  else if (WiFi.status() == WL_CONNECT_FAILED)
  {
    //Serial.println("WIFI PASSWORD FAILED"); // FOR DEBUG ONLY
  }
  else if (WiFi.status() == WL_NO_SSID_AVAIL)
  {
    //Serial.println("SSID GIVEN IS WRONG"); // FOR DEBUG ONLY
  }
}
