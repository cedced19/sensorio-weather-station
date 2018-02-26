#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 12 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "claire";
const char *password = "26afaf2004";

void setup()
{
  WiFi.begin(ssid, password);
  dht.begin();
  sensors.begin(); 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    char params[100];

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (!isnan(h) || !isnan(t) || !isnan(f)) {
      char hB[6];
      dtostrf(h, 4, 2, hB);
      char tB[6];
      dtostrf(t, 4, 2, tB);

      float hic = dht.computeHeatIndex(t, h, false);
      char hicB[6];
      dtostrf(hic, 4, 2, hicB);

      sensors.requestTemperatures();
      char t2[6];
      dtostrf(sensors.getTempCByIndex(0), 4, 2, t2);

      snprintf(params, 100, "humidity=%s&temperature=%s&heat_index=%s&temperature2=%s", hB, tB, hicB, t2);

      HTTPClient http;
      http.begin("http://192.168.0.47:8000/"); // specifie the address
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.POST(params);    
      http.end();
    }
  }
  ESP.deepSleep(120000000); 
}
