#include <ESP8266WiFi.h>
#include <DHT.h>
String apiKey = "S7LN3O5T6J1ORP4U";     //  The Write API key from ThingSpeak
const char *ssid =  "dlink-9478";     // Wifi ssid and wpa2 key
const char *pass =  "2xjialjl";
const char* server = "api.thingspeak.com";
DHT dht(4, DHT11);
WiFiClient client;
const int rp=14;
const int gp=12;
const int bp=13;
int calculaterpm(double t){
  int rpm;
  rpm=(t-15)/45*1000;
  rpm+=1500;//
  return rpm;
}
void setup() 
{
       Serial.begin(115200);
       delay(10);
       analogWrite(gp, 0);
       analogWrite(rp, 0);
       analogWrite(bp, 0);
       Serial.println("Connecting to ");
       Serial.println(ssid);
       dht.begin();
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
 
}
 
void loop() 
{
      noTone(0);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      int fanspeed;
      if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        delay(2000);
        return;
      } 
      if(t<30){
          analogWrite(2, 20);
          analogWrite(gp, 500);
          analogWrite(rp, 0);
      }
      else if(t>=30&&t<40){
          analogWrite(2, 200);
          analogWrite(rp, 200);
          analogWrite(gp, 400);
      }
      else if(t>=40&&t<50){
          analogWrite(2, 350);
          analogWrite(gp, 500);
          analogWrite(rp, 500);
      }
      else if(t>=45&&t<55){
          analogWrite(2, 600);
          analogWrite(gp, 200);
          analogWrite(rp, 600);
      }
      else if(t>=55){
          analogWrite(2, 800);
          analogWrite(gp, 0);
          analogWrite(rp, 900);
          tone(0,523);
      }           
      fanspeed=calculaterpm(t);     
      Serial.print("RPM=");   
      Serial.println(fanspeed);  
      if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
      {  
          String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(t);
           postStr +="&field2=";
           postStr += String(h);
           postStr += "\r\n\r\n";
           client.print("POST /update HTTP/1.1\n");
           client.print("Host: api.thingspeak.com\n");
           client.print("Connection: close\n");
           client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
           client.print("Content-Type: application/x-www-form-urlencoded\n");
           client.print("Content-Length: ");
           client.print(postStr.length());
           client.print("\n\n");
           client.print(postStr);
           Serial.print("Temperature: ");
           Serial.print(t);
           Serial.print(" degrees Celcius, Humidity: ");
           Serial.print(h);
           Serial.println("%. Send to Thingspeak.");                                           
      }
      client.stop();
      Serial.println("Waiting...");
      // thingspeak needs minimum 15 sec delay between updates
      delay(15000);
}
