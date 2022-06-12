#include <WiFi.h>

// Set your Network Name and Password
const char* ssid     = "ESP32-WIFI-SCANNER";
const char* password = "123456789";

WiFiServer server(80);

char linebuf[80];
int charcount = 0;

void setup() {

//Initialize serial and wait for port to open:
Serial.begin(115200);
Serial.println();
Serial.print("Setting AP (Access Point)…");
WiFi.softAP(ssid, password);

IPAddress IP = WiFi.softAPIP();
Serial.print("AP IP address: ");
Serial.println(IP);

server.begin();
}

void loop() {
// listen for incoming clients
WiFiClient client = server.available();
if (client) {
Serial.println("New client");
long rssi = WiFi.RSSI();
memset(linebuf, 0, sizeof(linebuf));
charcount = 0;
boolean currentLineIsBlank = true;
while (client.connected()) {
if (client.available()) {
char c = client.read();
Serial.write(c);
linebuf[charcount] = c;
if (charcount < sizeof(linebuf) - 1) charcount++;
if (c == '\n' && currentLineIsBlank) {
// send a standard http response
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");
client.println();
client.println("<!DOCTYPE HTML><html><head>");
client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
client.println("<h1>ESP32 WiFi HTTP Server</h1>");
client.print("<p>RSSI: ");
client.print(rssi);
client.println(" dBm</p>");
client.print("<p>Local MAC: ");
getMacAddress(client);
client.println("</p>");
client.print("<p>Neighborhood networks:</p>");
getNetworks(client);
client.println("");
client.println("</html>");
break;
}
if (c == '\n') {
currentLineIsBlank = true;
memset(linebuf, 0, sizeof(linebuf));
charcount = 0;
} else if (c != '\r') {
// you've gotten a character on the current line
currentLineIsBlank = false;
}
}
}
delay(1);
client.stop();
Serial.println("client disconnected");
}
}

void getMacAddress(WiFiClient client) {
byte mac[6];
WiFi.macAddress(mac);
client.print(mac[5], HEX);
client.print(":");
client.print(mac[4], HEX);
client.print(":");
client.print(mac[3], HEX);
client.print(":");
client.print(mac[2], HEX);
client.print(":");
client.print(mac[1], HEX);
client.print(":");
client.println(mac[0], HEX);
}

void getNetworks(WiFiClient client) {

int numSsid = WiFi.scanNetworks(); client.println("<table border='1' style='width: 300px'>");
client.println("<tr>");
client.println("<th style='width: 15px'>NR</th>");
client.println("<th style='width: 255px'>Name</th>");
client.println("<th style='width: 20px'>dBm</th>");
client.println("</tr>");
for (int thisNet = 0; thisNet < numSsid; thisNet++) {
client.println("<tr>");
client.println("<td>");
client.print(thisNet);
client.println("</td>");
client.println("<td>");
client.print(WiFi.SSID(thisNet));
client.println("</td>");
client.println("<td>");
client.print(WiFi.RSSI(thisNet));
client.println("</td>");
client.println("</tr>");
}
client.println("</table>");
}
