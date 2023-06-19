# Envío controlado de Interests y Datas con contenido
Aquí se encuentra el desarrollo de Datas con contenido del Productor. 

## 1. Modificaciones principales en ndnph::PingServer
En la clase ndnph::PingServer, se declara fuera de processInterest una variable int “state” para definir el estado actual en on, off o blink.
En el método processInterest, se añaden nombres que servirán para interpretar la comando del Consumidor según el Interest entrante y se define el Content que contendrá los datos.
<pre>
<code>const char* PREFIX0 = "/led/on";
const char* PREFIX1 = "/led/blink";
const char* PREFIX2 = "/led/off";
ndnph::tlv::Value content;</code></pre>
Se añaden los siguientes tres apartados en los que se comprueba si el dataname es el del estado on, off o blink. Dentro sólo se cambia el valor de “state” y se añade la respuesta al content. En el main se comprueba el estado y se escribe el valor adecuado al LED.
<pre>
<code>if (ndnph::Name::parse(region0, PREFIX0).isPrefixOf(interest.getName())) {
      if (state != 0) {
        state = 0;
        content = ndnph::tlv::Value::fromString("Led encendido con éxito. \n");
      } else {
        content = ndnph::tlv::Value::fromString("led ya encendido. \n");
      }
    } else if (ndnph::Name::parse(region0, PREFIX1).isPrefixOf(interest.getName())) {
      state = 1;
      content = ndnph::tlv::Value::fromString("Led parpadeando con éxito. \n");
    } else if (ndnph::Name::parse(region0, PREFIX2).isPrefixOf(interest.getName())) {
      if (state != 2) {
        state = 2;
        content = ndnph::tlv::Value::fromString("Led apagado con éxito. \n");
      } else {
        content = ndnph::tlv::Value::fromString("Led ya apagado. \n");
      }
    }</code></pre>
    
A la hora de enviar el paquete Data, se añade una línea en la que se incluye el Content en su interior:
<pre>
<code>data.setContent(content);</code></pre>

## 2. Modificaciones principales en PingServer: Main
En cuanto al main, se declara el LED, y se usa un switch sobre la variable “state” para escribir en el LED el valor adecuado:
<pre>
<code>#define LED D0 // Declaración del LED

int state = server2.getState();

  switch (state)
  {
  case 0:
    Serial.println("Encendiendo led ... ");
    digitalWrite(LED, LOW);
    break;
  case 1:
    Serial.println("Parpadeando led ... ");
    digitalWrite(LED, LOW);
    delay(1000);
    digitalWrite(LED, HIGH);
    break;
  case 2:
    Serial.println("Apagando led ... ");
    digitalWrite(LED, HIGH);
    break;
  default:
    Serial.println("Default");
    digitalWrite(LED, HIGH);
    break;
  }</code></pre>
  