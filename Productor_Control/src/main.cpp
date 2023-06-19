/*
  En este código se crean un objeto PingServer que escucha Interests con datanames "/led/on", "/led/blink" y "/led/off".

  Al ejecutar el código, el PingServer comenzará a escuchar Interests entrantes, y a la vez mostrará por pantalla el estado en el que se encuentra. (Por defecto, 2 = Apagado)
  Los estados que se contemplan son:
  0 = Orden de encender   ---> /led/on
  1 = Orden de parpadear  ---> /led/blink
  2 = Orden de Apagar     ---> /led/off

  Cuando se recibe un Interest con algún dataname de los indicados arriba, se procesa para saber que orden realizar, se muestra por pantalla, y se envía una respuesta al cliente.

  Para conseguir este comportamiento, también se ha modificado el archivo ping-server.hpp en donde se declara PingServer:
  .pio\libdeps\nodemcuv2\NDNph\src\ndnph\app\ping-server.hpp

  Es necesario indicar los datos de la conexión WiFi más abajo para que funcione correctamente.
*/
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

/* Introduzca los datos de la conexion WiFi */
const char* WIFI_SSID = "XXXX";
const char* WIFI_PASS = "XXXX";

// Declaraciones relacionadas con el PingServer
ndnph::StaticRegion<1024> region;
const char *PREFIX0 = "/led";
std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport2(udpBuffer);
ndnph::transport::ForceEndpointId transport2w(transport2);
ndnph::Face face2(transport2w);
ndnph::PingServer server2(ndnph::Name::parse(region, PREFIX0), face2);

void setup()
{
  Serial.begin(9600);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);
  Serial.println(WiFi.localIP());
  pinMode(LED, OUTPUT);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  delay(1000);
  Serial.println("Mi direccion IP: ");
  Serial.println(WiFi.localIP());
  IPAddress remoteIp = (192, 168, 1, 60);
  bool ok2 = transport2.beginTunnel(remoteIp); // En este apartado, se utilizan conexiones Multicast para interconectar los dispositivos

  if (!ok2)
  {
    Serial.println(F("Inicialización de la conexión fallida."));
    ESP.restart();
  }
}

// Funcion para mostrar por pantalla cuando se recibe un Interest
void printData(ndnph::PingServer &server)
{
  Serial.println(" Recogiendo interest ...");
  ndnph::Interest data = server.get_Interest();
  Serial.println(" Recogiendo dataname ... ");

  try
  {
    ndnph::Name dataName = data.getName();
    Serial.printf("Interest recibido con dataname: ");
    Serial.println(dataName);
  }
  catch (const std::exception &e)
  {
    Serial.println("Todavia no hay paquete");
  }

  Serial.flush();
}

void loop()
{
  face2.loop();

  int state = server2.getState(); // Se recoge el estado del Producer
  Serial.printf("Estado: ");
  Serial.println(state);

  switch (state)
  {
  case 0: // Estado Encendido
    digitalWrite(LED, LOW);
    break;
  case 1: // Estado Parpadeo
    digitalWrite(LED, LOW);
    delay(1000);
    digitalWrite(LED, HIGH);
    break;
  case 2: // Estado Apagado
    digitalWrite(LED, HIGH);
    break;
  default:
    break;
  }

  delay(1000);

  // Si se ha enviado un paquete Data, se muestra por pantalla
  if (server2.Data_sent)
  {
    Serial.println("Enviado paquete con dataname: ");
    Serial.println(server2.datanamePacket);
    server2.Data_sent = 0;
  }
  static uint16_t i = 0;
  if (++i % 1024 == 0)
  {
    printData(server2);
  }
}
