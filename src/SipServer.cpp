//
// Created by magomedcoder on 07.02.2023.
//

#include "SipServer.hpp"

SipServer::SipServer(const std::string &ip, int port) :
        _socket(ip, port, [this](auto &&PH1, auto &&PH2) {
            onNewMessage(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        }),
        _handler(ip, port, [this](auto &&PH1, auto &&PH2) {
            onHandled(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        }) {
    _socket.startReceive();
}

void SipServer::onNewMessage(std::string data, sockaddr_in src) {
    auto message = SipMessageFactory::createMessage(std::move(data), src);
    if (message.has_value()) {
        _handler.handle(std::move(message.value()));
    }
}

void SipServer::onHandled(const sockaddr_in &dest, const std::shared_ptr<SipMessage> &message) {
    _socket.send(dest, message->toString());
}