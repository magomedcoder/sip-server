//
// Created by magomedcoder on 07.02.2023.
//

#include "SipMessageFactory.hpp"

std::optional<std::shared_ptr<SipMessage>> SipMessageFactory::createMessage(std::string message, sockaddr_in src) {
    try {
        if (containsSdp(message)) {
            return std::make_shared<SipSdpMessage>(std::move(message), src);
        }
        return std::make_shared<SipMessage>(std::move(message), src);
    } catch (const std::exception &) {
        return {};
    }
}

bool SipMessageFactory::containsSdp(const std::string &message) {
    return message.find(SDP_CONTENT_TYPE) != std::string::npos;
}
