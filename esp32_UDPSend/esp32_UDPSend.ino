#include <WiFi.h>
#include <WiFiUdp.h>

// Datos WiFi
const char* WIFI_SSID = "Mucho Flow";
const char* WIFI_PASS = "00439568963";

// Destino UDP
const char* UDP_IP = "192.168.1.24";
const uint16_t UDP_PORT = 5005;

WiFiUDP udp;

String bufferSerial = "";

void conectarWiFi() {
  Serial.print("Conectando a WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

void enviarUDP(const String& mensaje) {
  udp.beginPacket(UDP_IP, UDP_PORT);
  udp.print(mensaje);
  udp.endPacket();

  Serial.print("Enviado por UDP: ");
  Serial.println(mensaje);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  conectarWiFi();

  // No es estrictamente necesario para enviar,
  // pero deja inicializado el socket UDP local.
  udp.begin(0);
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      bufferSerial.trim();

      if (bufferSerial.length() > 0) {
        enviarUDP(bufferSerial);
      }

      bufferSerial = "";
    } else {
      bufferSerial += c;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
}
