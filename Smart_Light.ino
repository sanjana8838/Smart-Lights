
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

int ldrIn = A0;
int led1 = 0;
int led2 = 2;
const int trigPin = 5;  
const int echoPin = 4;   
int value=0;
long duration;
int dist;
int ON=0;

String apiKey = "FYNN2LEC39RUS3PR";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "Sanjana's iPhone";     // replace with your wifi ssid and wpa2 key
const char *pass =  "sanjana";    //replace with password
const char* server = "api.thingspeak.com";
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "sanjanaramesh" //username from adafruit
#define AIO_KEY         "2c35408943444b079177ec080b"  //key from adafruit project dashboard

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/switch1");

void MQTT_connect();


void setup() {
   ON=0;
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(ldrIn, INPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);

    Serial.begin(9600); // Starts the serial communication @9600

    Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected"); 
        mqtt.subscribe(&onoffbutton);
}
uint32_t x=0;

void loop() {
    ON=0;
      MQTT_connect();
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    dist= duration*0.034/2;
    Serial.print("Distance: ");
    Serial.println(dist);

if(dist<24 && dist>12)
{
    value = analogRead(ldrIn);
    value = map(value, 100,1010, 0, 255);
    if(value<0)
    {  ON=20;
      analogWrite(led1, 712);
     analogWrite(led2, 712);
     senddata();
client.stop();
Serial.println("Waiting..");

    }
    else
    {  ON=20;
      analogWrite(led1, 255);
     analogWrite(led2, 255);
     senddata();
client.stop();
Serial.println("Waiting..");

    }
    Serial.println(value);
    delay(5000);
    
}
else if(dist<=12)
{
  digitalWrite(led1, HIGH);
  value = analogRead(ldrIn);
    value = map(value, 100,1010, 0, 255);
    if(value<0)
    { ON=20;
      analogWrite(led2, 712);
      senddata();
client.stop();
Serial.println("Waiting..");

    }
    else
    {  ON=20;
      analogWrite(led2, 255);
      senddata();
client.stop();
Serial.println("Waiting..");

    }
    Serial.println(value);
    delay(5000);
}
else
{ ON=0;
digitalWrite(led1, HIGH);
digitalWrite(led2, HIGH);
senddata();
client.stop();
Serial.println("Waiting..");

}
if (dist>24)
{
Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      uint16_t state = atoi((char*)onoffbutton.lastread);
      Serial.println(state);
      digitalWrite(led1,state);
      digitalWrite(led2,state);
    }
  }
}
}
  
void senddata()
{
    if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";    
                             postStr += String(ON);
                             postStr += "\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Condition:");
                             Serial.print(ON);
                       
                             Serial.println("%. Send to Thingspeak.");
                        }
}

       void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

