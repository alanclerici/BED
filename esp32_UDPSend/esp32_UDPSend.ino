#include <WiFi.h>
#include <WiFiUdp.h>

// Datos WiFi
const char* WIFI_SSID = "Mucho Flow";
const char* WIFI_PASS = "00439568963";

// Destino UDP
const char* UDP_IP = "192.168.1.24";
const uint16_t UDP_PORT = 5005;

// Pulsador conectado entre GPIO13 y GND
const uint8_t PIN_PULSADOR = 13;

// Tiempos
const unsigned long DEBOUNCE_MS = 80;
const unsigned long TIEMPO_FIN_MS = 3000;

WiFiUDP udp;

String bufferSerial = "";

// Variables para antirrebote
bool lecturaAnterior = HIGH;
bool estadoEstable = HIGH;
bool estadoEstableAnterior = HIGH;

unsigned long ultimoCambioLectura = 0;
unsigned long tiempoInicioPulsacion = 0;

bool botonPresionado = false;
bool finEnviado = false;

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
  udp.print('\n');
  udp.endPacket();

  Serial.print("Enviado por UDP: ");
  Serial.println(mensaje);
}

void procesarSerial() {
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
}

void procesarPulsador() {
  bool lecturaActual = digitalRead(PIN_PULSADOR);

  // Detectar cambios para antirrebote
  if (lecturaActual != lecturaAnterior) {
    ultimoCambioLectura = millis();
    lecturaAnterior = lecturaActual;
  }

  // Actualizar estado estable luego del tiempo de debounce
  if ((millis() - ultimoCambioLectura) >= DEBOUNCE_MS) {
    estadoEstable = lecturaActual;
  }

  // Flanco de bajada: pulsador presionado
  if (estadoEstableAnterior == HIGH && estadoEstable == LOW) {
    tiempoInicioPulsacion = millis();
    botonPresionado = true;
    finEnviado = false;
  }

  // Si sigue presionado y pasaron 3 segundos, enviar "fin"
  if (botonPresionado && estadoEstable == LOW && !finEnviado) {
    if ((millis() - tiempoInicioPulsacion) >= TIEMPO_FIN_MS) {
      enviarUDP("fin");
      finEnviado = true;
    }
  }

  // Flanco de subida: pulsador liberado
  if (estadoEstableAnterior == LOW && estadoEstable == HIGH) {
    botonPresionado = false;

    // Si no se llegó a enviar "fin", entonces fue una pulsación corta
    if (!finEnviado) {
      enviarUDP("emergencia");
    }
  }

  estadoEstableAnterior = estadoEstable;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_PULSADOR, INPUT_PULLUP);

  conectarWiFi();

  // Inicializa UDP
  udp.begin(0);

  Serial.println("Sistema listo");
}

void loop() {
  procesarSerial();
  procesarPulsador();

  // Reconexión automática WiFi
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
}