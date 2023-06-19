#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "XXXX";
const char* WIFI_PASS = "XXXX";

const char* PREFIX0 = "/prueba0";
const char* PREFIX1 = "/prueba1";
const char* PREFIX2 = "/prueba2";

esp8266ndn::EthernetTransport transport0;
ndnph::Face face0(transport0);
esp8266ndn::UdpTransport transport1;
ndnph::Face face1(transport1);
esp8266ndn::UdpTransport transport2;
ndnph::Face face2(transport2);

ndnph::StaticRegion<1024> region;
ndnph::PingClient client0(ndnph::Name::parse(region, PREFIX0), face0);
ndnph::PingClient client1(ndnph::Name::parse(region, PREFIX1), face1);
ndnph::PingClient client2(ndnph::Name::parse(region, PREFIX2), face2);

void
setup()
{
  Serial.begin(9600);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(F("WiFi connect failed"));
    ESP.restart();
  }
  delay(1000);

#if defined(ARDUINO_ARCH_ESP8266)
  BearSSL::WiFiClientSecure fchSocketClient;
  fchSocketClient.setInsecure();
#elif defined(ARDUINO_ARCH_ESP32)
  WiFiClientSecure fchSocketClient;
  fchSocketClient.setInsecure();
#endif
  IPAddress routerIp = IPAddress(192, 168, 1, 77);
  if (routerIp == INADDR_NONE)
  {
    ESP.restart();
  }

  transport0.begin();
  transport1.beginTunnel(routerIp);
  transport2.beginMulticast(WiFi.localIP());
}

void
printCounters(const char* prefix, const ndnph::PingClient& client)
{
  auto cnt = client.readCounters();
  Serial.printf("%8dI %8dD %3.3f%% %s\n", static_cast<int>(cnt.nTxInterests),
                static_cast<int>(cnt.nRxData), 100.0 * cnt.nRxData / cnt.nTxInterests, prefix);
}

void
loop()
{
  face0.loop();
  face1.loop();
  face2.loop();
  delay(1);

  static uint16_t i = 0;
  if (++i % 1024 == 0) {
    printCounters(PREFIX0, client0);
    printCounters(PREFIX1, client1);
    printCounters(PREFIX2, client2);
  }
}