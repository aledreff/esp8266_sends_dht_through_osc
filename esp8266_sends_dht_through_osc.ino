/*---------------------------------------------------------------------------------------------

  DHT sensor sends temperature and humidity data to PC
  via Open Sound Control (OSC) for the ESP8266. 

  You need to install the DHT sensor library. 

  A Max/MSP example to receive the OSC data is also provided. 

  Antoine LE DREFF - 2021  (￢‿￢)

--------------------------------------------------------------------------------------------- */
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE    DHT22     // DHT 22 (AM2302)
#define DHTPIN 4     // Digital pin connected to the DHT sensor 
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "#####";        // network SSID (change ##### with name of your network)
char pass[] = "#####";       // network password (change ##### with password)

WiFiUDP Udp;
const IPAddress outIp(10,00,00,00);         // change this with the remote IP of your computer
const unsigned int outPort = 9997;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

// Set delay between sensor readings
uint32_t delayMS = 2000;



void setup() {
    Serial.begin(115200);

// Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
#ifdef ESP32
    Serial.println(localPort);
#else
    Serial.println(Udp.localPort());
#endif


// Initialize DHT device.
  dht.begin();

}



void loop() {

//print DHT sensor values
  // Delay between measurements.
  delay(delayMS);
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F(" °C"));
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(" %");

// send data via OSC
    OSCMessage msg("/DHT");
    msg.add(t);
    msg.add(h);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    delay(100);
}
