<p float="middle">
<img src="https://upload.wikimedia.org/wikipedia/commons/2/25/Electron_Wordmark.svg" width="45%" height="50"/>
<img src="https://named-data.net/doc/NFD/current/_static/ndn-logo.svg" width="45%" height="70" />
</p>



# Creación de una red NDN con dispositivos ESP8266

## Introducción

En este repositorio se aloja el código del trabajo de fin de grado *"Creación de una red NDN con dispositivos ESP8266"*.<br/>

Este proyecto trata de comprobar la viabilidad del protocolo NDN como base para construir aplicaciones utilizando dispositivos IOT.<br/>
Para ello, a partir de la librería [esp8266ndn](https://github.com/yoursunny/esp8266ndn), se construye una comunicación NDN entre 2 ESPs por la que se comparten datos de un sensor de temperatura.<br/>

Estos datos son luego mostrados en una aplicación web desarrollada con ElectronJS.
Adicionalmente, se configura un nodo NFD entre la comunicación de los ESPs para comprobar escalabilidad e interoperabilidad.

##  Funcionamiento

Siguiendo nomenclatura NDN, los ESPs realizarán uno el rol de Consumidor y otro el de Productor, siendo el Productor el que produce o proporciona la información y el Consumidor el que la consume.

El Consumidor lanzará paquetes Interest con un nombre específico al que contesta el Productor, que se encargará de trasmitir de vuelta un paquete Data con los datos de un sensor DHT previamente conectado.

El nodo NFD actuará de intermediario entre los ESPs, se encargará de enrutar cada paquete según el dataname que lleve y el tipo de paquete que sea, Interest o Data.

Cuando el paquete Data llegue al Consumidor, éste lo reenviará a la aplicación web, que estará leyendo el puerto serie para obtener los datos de temperatura y humedad para mostrarlos por pantalla.
 
##  Estructura de ficheros

A continuación se explicará como está organizado el repositorio.
El código de la maqueta final del proyecto está formado por las siguientes carpetas:
- **App_Web:** Código de la aplicación web.
- **Consumidor_Control:** Consumidor de la prueba de envío controlado de Interests y Datas con contenido.
- **Consumidor_Maqueta:** Consumidor de la maqueta final.
- **Consumidor_Simple:** Consumidor de la prueba de conexión simple.
- **Consumidor_Testbed:** Consumidor de la prueba de conexión al testbed NDN.
- **KY-015 Temperatura:** Código del cual se adaptó la recogida de datos del sensor de la maqueta final. 
- **Productor_Control:** Productor de la prueba de envío controlado de Interests y Datas con contenido.
- **Productor_Maqueta:** Productor de la maqueta final.
- **Productor_Simple:** Productor de la prueba de conexión simple.

## Software utilizado

Durante el desarrollo del proyecto, se han utilizado las siguientes herramientas:
- **Visual Studio Code con la extensión PlatformIO:** Para gestionar el desarrollo y despliegue del código de la aplicación web y los proyectos del Consumer y Producer.
- **ElectronJS:** Como framework de desarrollo de la aplicación web.

Para probar la aplicación web, es necesario instalar Node y Electron. 
Una vez instalados, se abre un terminal en App_Web y se usan los comandos:
- npm install
- npm start

La forma más rápida de probar el código de la maqueta es instalar VSCode y la extensión PlatformIO. La propia extensión permite importar proyectos y subirlos a los dispositivos fácilmente.
