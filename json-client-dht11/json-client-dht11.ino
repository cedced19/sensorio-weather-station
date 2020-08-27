#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

const char *ssid = "wifi_name";
const char *password = "wifi_pwd";

void setup()
{
  WiFi.begin(ssid, password);
  WiFi.hostname("esp-cave-temp"); 
  Serial.begin(9600);
  dht.begin();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    
  Serial.println("Connecting..");

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

      snprintf(params, 100, "humidity=%s&temperature=%s&heat_index=%s&temperature2=false", hB, tB, hicB);

      HTTPClient http;
      http.begin("http://192.168.0.40:8888/api/weather-data/"); // specifie the address
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.POST(params);    
      http.end();
    }
  }
  delay(3600000); 
}
