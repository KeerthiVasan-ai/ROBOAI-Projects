#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <DHT.h>

// USE YOUR SSID PASSWORD
const char* ssid = "";
const char* password = "";

WiFiServer server(80);
WiFiClient client;

// USE YOUR THINKSPEAK CHANNEL ID and API
unsigned long channelId = 000000;
const char* writeApi = "XXXXXXXXXXXXXX";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  dht.begin();
  ThingSpeak.begin(client);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    delay(2500);

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.print("Temperature (ºC): ");
    Serial.print(t);
    Serial.println("ºC");
    
    Serial.print("Humidity (%): ");
    Serial.println(h);

    ThingSpeak.setField(1, h);
    ThingSpeak.setField(2, t);

    int x = ThingSpeak.writeFields(channelId, writeApi);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    lastTime = millis();
  }
}