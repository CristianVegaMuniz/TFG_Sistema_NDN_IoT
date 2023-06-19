# Envío controlado de Interests y Datas con contenido
Aquí se encuentra el desarrollo de envío controlado de Interest del Consumidor. 


## 1. Modificaciones principales en  PingClient: ndnph::PingClient
Se añade una variable m_Data para guardar los paquetes Data, y una variable prefix0 para guardar aparte el nombre del consumidor. Estas variables son exclusivamente para imprimir por pantalla. Adquieren valor mediante set_Data(data) y prefix0 = get_Prefix() en la función processData() una vez recibido un paquete Data.
La variable Data_processed indica si se ha procesado un paquete Data para poder imprimir por pantalla, y avisar de que se puede volver a mandar otro Interest.
send_Interests es para controlar el envío de Interests. En caso de haber enviado un Interest y todavía no haberse satisfecho, se espera un tiempo antes de poder enviar más. 
<pre>
<code>Data m_Data; // Paquete Data entrante para printear en el main
  bool sent_Interest = false; // Interest enviado que no fue satisfecho aún
 
  int Data_processed = 0; // Para saber en el main que se procesó un Data
  Name prefix0; // Dataname auxiliar del Consumer para printear en main

  void set_Data(Data data) // getter y setter para data
  {
    m_Data = data;
  }
  Data get_Data()
  {
    return m_Data;
  }
 bool send_Interests = false; // Si true, pueden enviarse Interests</code></pre>
 
 Se rodea todo el interior de la función sendInterest() con un if(send_interests) para ejecutarla cuando se requiera desde el main poniendo este bool a true. Una vez dentro send_Interests pasa a false.
<pre>
<code>bool sendInterest()
  {
    if (send_Interests) {// se envían Interests cuando no hay ninguno enviado
      send_Interests = false; // Para no poder mandar más de uno
      StaticRegion<1024> region; 
	...
    }
  }</code></pre>

## 2. Modificaciones principales en  PingClient: Main

Se cambian los nombres utilizados por los consumidores.
timeout se utiliza para establecer un límite de tiempo en el que un Interest puede estar sin satisfacer antes de que se deseche. Esto es debido al problema encontrado de que en un punto de acceso hecho con un móvil, entre 1 o 2 paquetes de cada 3 no eran respondidos correctamente, a pesar de que a través de Wireshark era posible ver que salían del productor.
<pre>
<code>const char *PREFIX0 = "/led/on";
const char *PREFIX1 = "/led/blink";
const char *PREFIX2 = "/led/off";

int timeout = 0; // Por si algún Interest no es satisfecho</code></pre>

Se añade dos funciones printMenu() y printData() para imprimir los comandos disponibles, siendo encender, parpadear o apagar led, y para imprimir el paquete Data entrante al procesarse.
A continuación se comprueba si no hay ningún Interest sin satisfacer de ninguno de los consumidores. Si se cumple, se recoge la opción escogida por el usuario y se le permite al client asociado a la opción mandar un Interest. 

<pre>
<code>if (Serial.available() && !client0.sent_Interest && !client1.sent_Interest && !client2.sent_Interest){
    int point;

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
}</code></pre>
  
Se comprueba si algún paquete Data fue procesado por algún consumidor. Si es así, se resetean los flags para poder permitir el envío de más Interests. Como sólo se permite que haya un paquete Interest lanzado entre los 3 consumidores, se espera a que llegue un paquete Data. Si ninguno de los consumidores ha procesado ningún Data, entonces se aumenta la variable timeout hasta que llega al límite y se desecha ese Interest reseteando todo.

 
<pre>
<code>
if (client0.Data_processed > 0) 
    {
      printCounters(PREFIX0, client0);
      printData(client0);
      client0.sent_Interest = false;
      client0.Data_processed = 0;
      timeout = 0;
      printMenu();
    }
...
else if (client0.sent_Interest || client1.sent_Interest || client2.sent_Interest){
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
    }</code></pre>
