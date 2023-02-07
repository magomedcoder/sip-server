//
// Created by magomedcoder on 07.02.2023.
//

#include "SipClient.hpp"

SipClient::SipClient(std::string number, sockaddr_in address) : _number(std::move(number)),
                                                                _address(address) {
}

bool SipClient::operator==(const SipClient &other) {
    if (_number == other.getNumber()) {
        return true;
    }
    return false;
}

std::string SipClient::getNumber() const {
    return _number;
}

sockaddr_in SipClient::getAddress() const {
    return _address;
}
