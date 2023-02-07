//
// Created by magomedcoder on 07.02.2023.
//

#include "Session.hpp"
#include <utility>

Session::Session(std::string callID, std::shared_ptr<SipClient> src, uint32_t srcRtpPort) :
        _callID(std::move(callID)),
        _src(std::move(src)),
        _state(State::Invited),
        _srcRtpPort(srcRtpPort),
        _destRtpPort(0) {
}

void Session::setState(State state) {
    if (state == _state)
        return;
    _state = state;
    if (state == State::Connected) {
        std::cout << "Сеанс создан между " << _src->getNumber() << " и " << _dest->getNumber() << std::endl;
    }
}

void Session::setDest(std::shared_ptr<SipClient> dest, uint32_t destRtpPort) {
    _dest = std::move(dest);
    _destRtpPort = destRtpPort;
}

std::string Session::getCallID() const {
    return _callID;
}

std::shared_ptr<SipClient> Session::getSrc() const {
    return _src;
}

std::shared_ptr<SipClient> Session::getDest() const {
    return _dest;
}

Session::State Session::getState() const {
    return _state;
}
