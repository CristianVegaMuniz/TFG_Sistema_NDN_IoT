/*
  Declaracion del objeto PingClient

  Originalmente, PingClient mandaba Interest de forma continua a través de "sendInterest()" y se recibían los paquetes Data vacíos a través de "processData()".
  Bajo esta modificacion, solo manda un interest, y se espera al paquete Data de respuesta o al timeout para volver a poder mandar otro.
  Se controla el comportamiento desde el main. Los paquetes Data ahora también contienen datos.

  Se ha modificado lo siguiente:
  - Añadida variable "send_Interests" para controlar ejecucion de "sendInterest()" y solo ejecutarla cuando el usuario lo indique desde el menu del main.
  - Añadida variable "sent_Interest" para saber que el cliente ha enviado un Interest y por lo tanto, espera un paquete Data de respuesta.
  - Añadida variable "m_Data" para guardar el paquete entrante y mostrar por pantalla su contenido en el main.
  - Añadida variable "Data_processed" para saber cuando un paquete Data ha sido recibido. Una vez se activa, en el main "sent_interest" se desactiva y se reinicia el timeout, para volver a poder mandar un Interest.

*/

#ifndef NDNPH_APP_PING_CLIENT_HPP
#define NDNPH_APP_PING_CLIENT_HPP
#include "../face/packet-handler.hpp"
#include "../port/clock/port.hpp"

namespace ndnph
{

  class PingClient : public PacketHandler
  {
  public:
    /**
     * @brief Constructor.
     * @param prefix name prefix to request. It should have 'ping' suffix.
     * @param face face for communication.
     * @param interval Interest interval in milliseconds.
     */
    explicit PingClient(Name prefix, Face &face, int interval = 1000)
        : PacketHandler(face), m_prefix(std::move(prefix)), m_interval(interval), m_next(port::Clock::add(port::Clock::now(), interval))
    {
      port::RandomSource::generate(reinterpret_cast<uint8_t *>(&m_seqNum), sizeof(m_seqNum));
    }

    struct Counters
    {
      uint32_t nTxInterests = 0;
      uint32_t nRxData = 0;
    };

    Counters readCounters() const
    {
      return m_cnt;
    }

    Data m_Data;                // Paquete Data entrante para printear en el main
    bool sent_Interest = false; // hay algun Interest enviado que no fue satisfecho aún

    int Data_processed = 0; // Para saber en el main que se procesó el Data de respuesta

    void set_Data(Data data) // getter y setter para data
    {
      m_Data = data;
    }
    Data get_Data()
    {
      return m_Data;
    }

    bool send_Interests = false; // Si true, pueden enviarse Interests

  private:
    void loop() final
    {
      auto now = port::Clock::now();
      if (port::Clock::isBefore(now, m_next))
      {
        return;
      }
      sendInterest();
      m_next = port::Clock::add(now, m_interval);
    }

    bool sendInterest()
    {
      if (send_Interests)
      {                         // se envían Interests cuando no hay ninguno enviado
        send_Interests = false; // Para no poder mandar más de uno
        StaticRegion<1024> region;
        Component seqNumComp =
            Component::from(region, TT::GenericNameComponent, tlv::NNI8(++m_seqNum));
        assert(!!seqNumComp);
        Name name = m_prefix.append(region, {seqNumComp});
        assert(!!name);

        Interest interest = region.create<Interest>();
        assert(!!interest);
        interest.setName(name);
        interest.setMustBeFresh(true);

        if (!send(interest))
        {
          return false;
        }
        ++m_cnt.nTxInterests;
        return true;
      }
      return false;
    }

    bool processData(Data data) final
    {

      set_Data(data); // se guarda el Data para imprimir en el main el contenido

      const Name &dataName = data.getName();
      if (!m_prefix.isPrefixOf(dataName) || m_prefix.size() + 1 != dataName.size())
      {
        return false;
      }
      Component lastComp = dataName[-1];
      Decoder::Tlv d;
      Decoder::readTlv(d, lastComp.tlv(), lastComp.tlv() + lastComp.size());
      uint64_t seqNum = 0;
      if (!tlv::NNI8::decode(d, seqNum))
      {
        return false;
      }

      if (m_seqNum == seqNum)
      {
        ++m_cnt.nRxData;
      }

      Data_processed++; // Data procesado
      return true;
    }

  private:
    Name m_prefix;
    uint64_t m_seqNum = 0;
    int m_interval = 1000;
    port::Clock::Time m_next;
    Counters m_cnt;
  };

} // namespace ndnph

#endif // NDNPH_APP_PING_CLIENT_HPP
