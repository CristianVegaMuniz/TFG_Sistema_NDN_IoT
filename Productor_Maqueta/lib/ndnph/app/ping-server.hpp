/*
  Declaracion del objeto PingServer

  Originalmente, PingServer respondía a Interests con un determinado dataname, con un paquete Data vacío.
  PingServer procesa y responde a los Interests a través de "processInterest()".

  Bajo esta modificacion, PingServer:
  1. Comprueba si el dataname del Interest coincide con los datanames que escucha.
  2. Cambia el estado del LED al correspondiente, y responde al Interest con un paquete Data.
  Los paquetes Data ahora también contienen un mensaje de respuesta para mostrar en el PingClient.

  Los estados se relacionan con los datanames de la siguiente manera:
    0 = Encendido ---> "/led/on"
    1 = Parpadeo  ---> "/led/blink"
    2 = Apagado   ---> "/led/off"

  Se ha modificado lo siguiente:
  - Añadidas variables "PREFIX0", "PREFIX1", Y "PREFIX2" en "processInterest()", datanames a los que respondera el PingServer.
  - Añadida variable "content" que incluirá el mensaje de respuesta para el PingClient.
  - Añadida variable "Data_sent" para saber que se ha enviado un paquete Data y poder imprimirlo por pantalla en el main.
  - Añadida variable "m_Interest" para guardar el paquete entrante y mostrarlo por pantalla en el main.
  - Añadida variable "state" para conocer el estado actual del LED.

*/
#ifndef NDNPH_APP_PING_SERVER_HPP
#define NDNPH_APP_PING_SERVER_HPP
#include "../face/packet-handler.hpp"
#include "../keychain/digest.hpp"
#define LED D0 // Led del NodeMCU que se utiliza para mostrar los estados.

namespace ndnph
{

  class PingServer : public PacketHandler
  {
  public:
    /**
     * @brief Constructor.
     * @param prefix name prefix to serve. It should have 'ping' suffix.
     * @param face face for communication.
     */
    explicit PingServer(Name prefix, Face &face)
        : PacketHandler(face), m_prefix(std::move(prefix))
    {
    }
    int state = 2; // Estado 0 = Encendido, Estado 1 = Parpadear, Estado 2 = Apagado
    int getState()
    {
      return state;
    }

    Interest m_interest; // Se guarda el Interest para mostrarse en el main
    void set_Interest(Interest interest)
    {
      m_interest = interest;
    }
    Interest get_Interest()
    {
      return m_interest;
    }
    int Data_sent = 0;
    ndnph::Name datanamePacket;
    StaticRegion<1024> region0;

  private:
    bool processInterest(Interest interest) final
    {
      const char *PREFIX0 = "/led/on"; // Datanames que escuchará el PingServer
      const char *PREFIX1 = "/led/blink";
      const char *PREFIX2 = "/led/off";

      ndnph::tlv::Value content; // Contenido del paquete Data donde se incluirá la respuesta

      datanamePacket = interest.getName();

      set_Interest(interest);
      if (!m_prefix.isPrefixOf(interest.getName()))
      {
        return false;
      }

      // Se interpreta el prefijo del Interest para saber que orden realizar, y en el contenido del paquete se envia una contestacion a la peticion
      if (ndnph::Name::parse(region0, PREFIX0).isPrefixOf(interest.getName())) // Si el prefijo es /led/on
      {
        if (state != 0) // Encendido
        {
          state = 0;
          content = ndnph::tlv::Value::fromString("Led encendido con éxito. \n");
        }
        else
        {
          content = ndnph::tlv::Value::fromString("led ya encendido. \n");
        }
      }
      else if (ndnph::Name::parse(region0, PREFIX1).isPrefixOf(interest.getName())) // Si el prefijo es /led/blink
      {
        if (state != 1) // Parpadeo
        {
          state = 1;
          content = ndnph::tlv::Value::fromString("Led parpadeando con éxito. \n");
        }
        else
        {
          content = ndnph::tlv::Value::fromString("led ya parpadeando. \n");
        }
      }
      else if (ndnph::Name::parse(region0, PREFIX2).isPrefixOf(interest.getName())) // Si el prefijo es /led/off
      {
        if (state != 2) // Apagado
        {
          state = 2;
          content = ndnph::tlv::Value::fromString("Led apagado con éxito. \n");
        }
        else
        {
          content = ndnph::tlv::Value::fromString("Led ya apagado. \n");
        }
      }

      // Se responde con un paquete Data
      StaticRegion<1024> region;
      Data data = region.create<Data>();
      assert(!!data);
      data.setName(interest.getName());
      data.setFreshnessPeriod(1);
      data.setContent(content);           // Se incluye el contenido en el paquete Data
      reply(data.sign(DigestKey::get())); // Se envía
      Data_sent++;                        // Para avisar en el main de que se ha respondido
      return true;
    }

  private:
    Name m_prefix;
    String color;
  };

} // namespace ndnph

#endif // NDNPH_APP_PING_SERVER_HPP
