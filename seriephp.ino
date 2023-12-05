#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "SFR_5A70";        // your network SSID (name)
char pass[] = "ra4bleteechembleyhoc";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

IPAddress server(192,168,1,24);  // numeric IP for Google (no DNS)

String txtMsg = "";//mot reçu

WiFiClient client;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  printMacAddress(mac);//MAC: 30:C6:F7:02:6C:F4
  //connexion au réseau wifi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    Serial.println("You're connected to the network");
    Serial.println();
  }


}

void loop() {
  Serial.println("commande :");
  Serial.println();
  // add any incoming characters to the String:
  while (Serial.available() == 0){
    //attendre
    //Serial.println("j'attends");
  }
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    txtMsg += inChar;
  }
  //envoie de la requete
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /index.php?data=18 HTTP/1.1");
    client.println("Host: server");
    client.println("Connection: close");
    client.println();
  }
  Serial.println(txtMsg);

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

void printWifiStatus() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
