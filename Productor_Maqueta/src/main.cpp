/*
  En este código se crea un objeto PingServer, que al recibir Interests con "/temp" como dataname, activa su LED para avisar,
  y responde con un paquete Data que contiene valores de temperatura y humedad extraídos del sensor DHT11.

  Para ello, se ha modificado el archivo ping-server.hpp en la siguiente ruta:
  ESP_Producer\.pio\libdeps\nodemcuv2\esp8266ndn\src\ndnph\app\ping-server.hpp

  Es necesario introducir los datos de su conexion WiFi para el correcto funcionamiento.
*/

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#define LED D0        // Led del NodeMCU utilizado para avisar de la llegada de Interests.
#define DHTTYPE DHT11 // Se define el modelo DHT11.
#define DHTPIN D2     // Se define el pin para conectar el sensor.
DHT_Unified dht(DHTPIN, DHTTYPE);
float h, t;
uint32_t delayMS;

const char *WIFI_SSID = "XXXX";
const char *WIFI_PASS = "XXXX";

/* Declaraciones del PingServer */
ndnph::StaticRegion<1024> region;
const char *PREFIX0 = "/temp";
std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport(udpBuffer);
ndnph::transport::ForceEndpointId transportw(transport);
ndnph::Face face(transportw);
ndnph::PingServer server(ndnph::Name::parse(region, PREFIX0), face);

void setup()
{
  Serial.begin(9600);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);
  Serial.println(WiFi.localIP());
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  delay(1000);

  Serial.println("Mi dirección IP: "); // 192.168.1.77
  Serial.println(WiFi.localIP());
  esp8266ndn::EthernetTransport::listNetifs(Serial);

  /*
    En la maqueta final se realiza un enlace punto a punto mediante la función beginTunnel de la siguiente manera:
    Consumer <---> Nodo NFD <---> Producer
    En este caso, en routerIp se debe poner la IP del nodo NFD.
  */
  IPAddress routerIp = IPAddress(192, 168, 1, 143);
  bool ok2 = transport.beginTunnel(routerIp);
  if (!ok2)
  {
    Serial.println(F("Inicio de comunicacion fallido."));
    ESP.restart();
  }

  /* Sensor DHT11 setup */
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

/* Funcion para recoger temperatura y humedad del sensor */
void get_Temp()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event); // Se recoge el dato de temperatura, si no es valido se escribe un error por pantalla
  if (isnan(event.temperature))
  {
    Serial.println(F("ERROR: No es posible leer temperatura."));
  }
  else
  {
    Serial.print(F("Temperatura: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    server.set_Temperature(event.temperature);
  }

  dht.humidity().getEvent(&event); // Se recoge el dato de humedad, si no es valido se escribe un error por pantalla
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("ERROR: No es posible leer humedad."));
  }
  else
  {
    Serial.print(F("Humedad: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
    server.set_Humidity(event.relative_humidity);
  }
}

void loop()
{
  face.loop();

  /* Se espera el delay estipulado por el sensor, y se recogen los datos */
  delay(delayMS);
  get_Temp();

  /* Se activa el LED un tiempo para avisar si se ha recibido un Interest */
  if (server.Led)
  {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    server.Led = 0;
  }
}