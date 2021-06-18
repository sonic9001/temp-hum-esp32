#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>

/*LCD PINOUT*/
#define CLK 18
#define DIN 23
#define DC 19
#define CE 5
#define RST 14

Adafruit_PCD8544 display = Adafruit_PCD8544 (CLK, DIN, DC, CE, RST);

/*DHT PINOUT*/
#define DHTPIN 25
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

/*WIFI CONF*/
const char* ssid = "NETWORK NAME"; /*Put your networks name here*/
const char* password = "PASSWORD"; /*put your network pass here*/

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  /*Display setup*/
  display.setContrast(50);
  display.clearDisplay();
  display.begin();
  
  /*DHT setup*/
  dht.begin();

  /*Wi-fi setup*/
  Serial.println("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connection established!");
  Serial.println("Your webserver is reachable here: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  /*DHT input*/
  float h = dht.readHumidity();     //Read humidity
  float t = dht.readTemperature();  //Read temperature

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  /*Serial Output*/
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C\n"));

  /*Display Output*/
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 2);                
  
  display.print("Hum: ");
  display.print(h, 2);
  display.print("%\n");
  display.print("Temp:");
  display.print(t, 2);
  display.print("*C\n");
  
  display.display();

  /*Webserver Output*/
  WiFiClient client = server.available();
  if(client){
    Serial.println("New Client!");
    String currentLine = "";                
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            /*Response with 200 status*/
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            /*Response with data*/
            client.print("<center>Temp: ");
            client.print(t);
            client.print("\nHum: ");
            client.print(h);
            client.print("</center>");
            /*Response ends with newline character*/
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } 
      }
    }
    /*Closing connection*/
    client.stop();
    Serial.println("Client Disconnected.");
  }
  delay(500);
}
