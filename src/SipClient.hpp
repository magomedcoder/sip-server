//
// Created by magomedcoder on 07.02.2023.
//

#ifndef SIP_CLIENT_HPP
#define SIP_CLIENT_HPP

#include <netinet/in.h>
#include <iostream>

class SipClient {
public:
    SipClient(std::string number, sockaddr_in address);

    bool operator==(const SipClient &other);

    [[nodiscard]] std::string getNumber() const;

    [[nodiscard]] sockaddr_in getAddress() const;

private:
    std::string _number;
    sockaddr_in _address;
};

#endif
