//
// Created by magomedcoder on 07.02.2023.
//

#ifndef SIP_MESSAGE_FACTORY_HPP
#define SIP_MESSAGE_FACTORY_HPP

#include <optional>
#include <memory>
#include "SipSdpMessage.hpp"

class SipMessageFactory {
public:
    static std::optional<std::shared_ptr<SipMessage>> createMessage(std::string message, sockaddr_in src);

private:
    static constexpr auto SDP_CONTENT_TYPE = "application/sdp";

    static bool containsSdp(const std::string &message);
};

#endif
