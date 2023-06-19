#include <Arduino.h>
#include <DHT.h> //Cargamos la librería DHT
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#define DHTTYPE DHT11 // se define el modelo DHT11
#define DHTPIN D2     // Se define el pin para conectar el sensor
DHT_Unified dht(DHTPIN, DHTTYPE);
float h, t;
uint32_t delayMS;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  // Mostrar por pantalla los detalles del sensor de temperatura.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Sensor de Temperatura"));
  Serial.print(F("Tipo: "));
  Serial.println(sensor.name);
  Serial.print(F("Version driver:  "));
  Serial.println(sensor.version);
  Serial.print(F("ID unico:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Valor max.:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Valor min.:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolucion:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));

  // Mostrar por pantalla los detalles del sensor de humedad.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Sensor de Humedad"));
  Serial.print(F("Tipo: "));
  Serial.println(sensor.name);
  Serial.print(F("Version driver:  "));
  Serial.println(sensor.version);
  Serial.print(F("ID unico:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Valor max.:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Valor min.:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolucion:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));

  // Se ajusta el delay de lectura del sensor según las especificaciones.
  delayMS = sensor.min_delay / 1000;
}
void loop()
{

  delay(delayMS);
  // Se lee valor de temperatura y muestra por pantalla.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("ERROR: No es posible leer temperatura."));
  }
  else
  {
    Serial.print(F("Temperatura: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  // Se lee valor de huedad y se muestra por pantalla.

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("ERROR: No es posible leer humedad."));
  }
  else
  {
    Serial.print(F("Humedad: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
}