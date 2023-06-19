#ifndef NDNPH_APP_PING_SERVER_HPP
#define NDNPH_APP_PING_SERVER_HPP

#include "../face/packet-handler.hpp"
#include "../keychain/digest.hpp"

namespace ndnph
{

  /** @brief Respond to every incoming Interest with empty Data. */
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

  private:
    bool processInterest(Interest interest) final
    {
      if (!m_prefix.isPrefixOf(interest.getName()))
      {
        return false;
      }

      StaticRegion<1024> region;
      const char *PREFIX0 = "/led/on";
      const char *PREFIX1 = "/led/color/random";
      const char *PREFIX2 = "/led/off";

      if (interest.getName().compare(ndnph::Name::parse(region, PREFIX0)))
      {
      }
      else if (interest.getName().compare(ndnph::Name::parse(region, PREFIX1)))
      {
      }
      else if (interest.getName().compare(ndnph::Name::parse(region, PREFIX2)))
      {
      }

      Data data = region.create<Data>();
      assert(!!data);
      data.setName(interest.getName());
      data.setFreshnessPeriod(1);
      reply(data.sign(DigestKey::get()));
      return true;
    }

  private:
    Name m_prefix;
  };

} // namespace ndnph

#endif // NDNPH_APP_PING_SERVER_HPP
