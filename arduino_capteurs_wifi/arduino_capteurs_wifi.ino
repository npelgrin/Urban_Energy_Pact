//bibliothèques
#include <SPI.h>
#include <WiFiNINA.h>



//variables globales
char ssid[] = "NETGEAR_11ng";        // your network SSID (name)
int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,110);
byte mac[6];
int pingResult;


void setup() {
  // put your setup code here, to run once:
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  }
    Serial.print("You're connected to the network");  
}

void loop() {
  // put your main code here, to run repeatedly:
  IPAddress ip = WiFi.localIP();//récupération adresse ip arduino
  Serial.println(ip);//affichage adresse ip
  
  WiFi.macAddress(mac);//récupération adresse mac
  //Serial.println(mac, HEX);
  printMacAddress(mac);//affichage adresse mac
  pingResult = WiFi.ping(server);
  if (pingResult >= 0) {
    Serial.print("SUCCESS! RTT = ");
    Serial.print(pingResult);
    Serial.println(" ms");
  } else {
    Serial.print("FAILED! Error code: ");
    Serial.println(pingResult);
  }
  delay(5000);
  Serial.println(WiFi.SSID());

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

//Fonctions

