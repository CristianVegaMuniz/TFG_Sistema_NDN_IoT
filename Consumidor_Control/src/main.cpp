/*
  En este código se crean 3 objetos PingClient que mandan Interests con datanames "/led/on", "/led/blink" y "/led/off" respectivamente.

  Al ejecutar el código, se muestra por pantalla un menú para que el usuario indique que orden quiere mandar: Encender, Parpadear, o Apagar.
  Se habilita que el cliente correspondiente mande un sólo Interest al Producer, que reconoce la orden procesando el dataname.
  Se esperará a que el cliente reciba el paquete Data con la respuesta, o a que pase el Timeout, para poder enviar otra orden.

  A mayores, se ha modificado el archivo ping-Client.hpp en donde se declara PingClient:
  .pio\libdeps\nodemcuv2\esp8266ndn\src\ndnph\app\ping-client.hpp

  Es necesario indicar los datos de la conexión WiFi más abajo para que funcione correctamente.
*/
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

/* Introduzca los datos de la conexion WiFi*/
const char* WIFI_SSID = "XXXX";
const char* WIFI_PASS = "XXXX";

/* Prefijos que se utilizan */
const char *PREFIX0 = "/led/on";
const char *PREFIX1 = "/led/blink";
const char *PREFIX2 = "/led/off";

/* Declaraciones relacionadas con los PingClients */
esp8266ndn::UdpTransport transport;
ndnph::Face face(transport);
int timeout = 0;
ndnph::StaticRegion<1024> region1;
ndnph::StaticRegion<1024> region2;
ndnph::StaticRegion<1024> region3;
ndnph::PingClient client0(ndnph::Name::parse(region1, PREFIX0), face);
ndnph::PingClient client1(ndnph::Name::parse(region2, PREFIX1), face);
ndnph::PingClient client2(ndnph::Name::parse(region3, PREFIX2), face);

/* Funcion de estadísticas de transmisiones / recepciones */
void printCounters(const char *prefix, const ndnph::PingClient &client)
{
  try
  {
    auto cnt = client.readCounters();
    Serial.printf("%8dI %8dD %3.3f%% %s\n", static_cast<int>(cnt.nTxInterests),
                  static_cast<int>(cnt.nRxData), 100.0 * cnt.nRxData / cnt.nTxInterests, prefix);
  }
  catch (const std::exception &e)
  {
    Serial.println("Error printeando contadores");
  }
}

/* Funcion printear menu */
void printMenu()
{
  Serial.println("Seleccione una opcion:");
  Serial.println("1. Encender led");
  Serial.println("2. Parpadear led");
  Serial.println("3. Apagar led");
}

/* Funcion printear datos del paquete Data recibido */
void printData(ndnph::PingClient &client)
{

  try
  {
    ndnph::Data data = client.get_Data();
    ndnph::Name dataName = data.getName();
    ndnph::tlv::Value valor = data.getContent();
    char *str = (char *)valor.begin();

    Serial.printf("Paquete recibido con dataname: ");
    Serial.println(dataName);
    Serial.printf("Datos del paquete: ");
    Serial.println(str);
    Serial.flush();
  }
  catch (const std::exception &e)
  {
    Serial.println("Todavia no hay paquete.");
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
  Serial.println("Mi direccion IP: ");
  Serial.println(WiFi.localIP());

  IPAddress remoteIp = (192, 168, 1, 77);
  bool ok2 = transport.beginTunnel(remoteIp);  // En este apartado, se utilizan conexiones Multicast para interconectar los dispositivos
  printMenu();
}

void loop()
{
  ndnph::Name dataName;

  // Se comprueba que ni el client0 "encender", client1 "parpadear" ni el client2 "apagar" están esperando contestación antes de mandar la siguiente orden.
  if (Serial.available() && !client0.sent_Interest && !client1.sent_Interest && !client2.sent_Interest)
  {
    int point;

    // Se manda la orden enviada por el usuario, y se bloquea el envío de más órdenes hasta que o se reciba respuesta, o pase el timeout.
    point = Serial.parseInt();

    switch (point)
    {
    case 1:
      Serial.println("Enviando Encender led.");
      client0.send_Interests = true; //
      client0.sent_Interest = true;
      break;
    case 2:
      Serial.println("Enviando Parpadear led.");
      client1.send_Interests = true;
      client1.sent_Interest = true;

      break;
    case 3:
      Serial.println("Enviando Apagar led.");
      client2.send_Interests = true;
      client2.sent_Interest = true;
      break;
    default:
      Serial.println("Opcion erronea.");
      break;
    }
  }

  face.loop();

  delay(1);

  static uint16_t i = 0;
  if (++i % 1024 == 0)
  {
    if (client0.Data_processed > 0) // Si se procesó el paquete Data, se deja de esperar paquetes y se reinicia el timeout
    {
      printCounters(PREFIX0, client0);
      printData(client0);
      client0.sent_Interest = false;
      client0.Data_processed = 0;
      timeout = 0;
      printMenu();
    }
    else if (client1.Data_processed > 0)
    {
      printCounters(PREFIX1, client1);
      printData(client1);
      client1.sent_Interest = false;
      client1.Data_processed = 0;
      timeout = 0;
      printMenu();
    }
    else if (client2.Data_processed > 0)
    {
      printCounters(PREFIX2, client2);
      printData(client2);
      client2.sent_Interest = false;
      timeout = 0;
      client2.Data_processed = 0;
      printMenu();
    }
    else if (client0.sent_Interest || client1.sent_Interest || client2.sent_Interest) // Si no llegó ningún Data y algún cliente espera uno, se suma al timeout
    {

      timeout++;
      Serial.printf("Timeout:");
      Serial.println(timeout);
      if (timeout == 10) // Si se llega a 60, dejamos de esperar el paquete.
      {

        Serial.println("Timeout alcanzado.");
        client0.sent_Interest = false;
        client1.sent_Interest = false;
        client2.sent_Interest = false;
        timeout = 0;
        printMenu();
      }
    }
  }
}
