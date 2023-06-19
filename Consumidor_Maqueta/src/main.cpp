/*
  En este código se crea un objeto PingClient, que manda constantemente Interests con "/temp" como dataname, para pedir datos del sensor DHT al Producer.
  Una vez llegan los paquetes Data, se escribe el contenido del paquete por la conexion Serial. Posteriormente, la aplicacion web recogerá esos datos para mostrarlos.

  Para ello, se ha modificado el archivo ping-client.hpp en la siguiente ruta:
  ESP_Producer\.pio\libdeps\nodemcuv2\esp8266ndn\src\ndnph\app\ping-client.hpp

  Es necesario introducir los datos de su conexion WiFi para el correcto funcionamiento.
*/
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

/* Datos WiFi necesarios para que se conecten los dispositivos */

const char *WIFI_SSID = "XXXX";
const char *WIFI_PASS = "XXXX";
/* Dataname que llevaran los Interests */
const char *PREFIX0 = "/temp";

/* Declaraciones del PingClient */
esp8266ndn::UdpTransport transport;
ndnph::Face face(transport);
ndnph::StaticRegion<1024> region;
ndnph::PingClient client(ndnph::Name::parse(region, PREFIX0), face);

/* Funcion de estadísticas de transmisiones / recepciones */
void printCounters(const char *prefix, const ndnph::PingClient &client)
{
  try
  {
    auto cnt = client.readCounters();
    Serial.printf("Stats*%8dI*%8dD*%3.3f%%*%s\n", static_cast<int>(cnt.nTxInterests),
                  static_cast<int>(cnt.nRxData), 100.0 * cnt.nRxData / cnt.nTxInterests, prefix);
  }
  catch (const std::exception &e)
  {
    Serial.println("Error printeando contadores");
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  delay(1000);
  Serial.println("Mi direccion IP: "); // 192.168.1.60
  Serial.println(WiFi.localIP());
  /*
    En la maqueta final se realiza un enlace punto a punto mediante la función beginTunnel de la siguiente manera:
    Consumer <---> Nodo NFD <---> Producer
    En este caso, en routerIp se debe poner la IP del nodo NFD.
  */
  IPAddress routerIp = IPAddress(192, 168, 1, 143);
  if (routerIp == INADDR_NONE)
  {
    ESP.restart();
  }
  transport.beginTunnel(routerIp);
}

void loop()
{
  face.loop();
  delay(1);
  /* Si se ha recibido un paquete Data, se imprime por el Serial el contenido, los valores de temperatura y humedad */
  if (client.data_received)
  {
    ndnph::Data data = client.get_Data();
    ndnph::tlv::Value valor = data.getContent();
    char *str = (char *)valor.begin();
    std::string a(str);

    Serial.println(a.c_str());
    printCounters(PREFIX0, client);
    client.data_received = 0;
  }
 

 /* static uint16_t i = 0;
  if (++i % 1024 == 0)
  {
    printCounters(PREFIX0, client);
  }*/
}