#if defined(ARDUINO_ARCH_ESP8266)
#include <AddrList.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "XXXX";
const char* WIFI_PASS = "XXXX";

ndnph::StaticRegion<1024> region;

esp8266ndn::EthernetTransport transport0;
ndnph::Face face0(transport0);
const char* PREFIX0 = "/prueba0";
ndnph::PingServer server0(ndnph::Name::parse(region, PREFIX0), face0);

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport1(udpBuffer);
ndnph::Face face1(transport1);
const char* PREFIX1 = "/prueba1";
ndnph::PingServer server1(ndnph::Name::parse(region, PREFIX1), face1);

esp8266ndn::UdpTransport transport2(udpBuffer);
ndnph::transport::ForceEndpointId transport2w(transport2);
ndnph::Face face2(transport2w);
const char* PREFIX2 = "/prueba2";
ndnph::PingServer server2(ndnph::Name::parse(region, PREFIX2), face2);

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
#if defined(ARDUINO_ARCH_ESP8266) && LWIP_IPV6
  { // wait until no new address showing up in 1000ms
    size_t nAddrPrev = 0, nAddr = 0;
    do {
      delay(1000);
      nAddrPrev = nAddr;
      nAddr = 0;
      for (auto a : addrList) {
        (void)a;
        ++nAddr;
      }
    } while (nAddrPrev == nAddr);
  }
#else
  delay(1000);
#endif

  esp8266ndn::EthernetTransport::listNetifs(Serial);
  bool ok = transport0.begin(); // select any STA netif
  if (!ok) {
    Serial.println(F("Ethernet transport initialization failed"));
    ESP.restart();
  }

  ok = transport1.beginListen();
  if (!ok) {
    Serial.println(F("UDP unicast transport initialization failed"));
    ESP.restart();
  }

  ok = transport2.beginMulticast(WiFi.localIP());
  if (!ok) {
    Serial.println(F("UDP multicast transport initialization failed"));
    ESP.restart();
  }

}

void
loop()
{
  face0.loop();
  face1.loop();
  face2.loop();
  delay(1);
}