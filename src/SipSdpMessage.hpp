//
// Created by magomedcoder on 07.02.2023.
//

#ifndef SIP_SDP_MESSAGE_HPP
#define SIP_SDP_MESSAGE_HPP

#include "SipMessage.hpp"

class SipSdpMessage : public SipMessage {
public:
    SipSdpMessage(std::string message, sockaddr_in src);

    void setMedia(const std::string &value);

    void setRtpPort(int port);

    [[nodiscard]] std::string getVersion() const;

    [[nodiscard]] std::string getOriginator() const;

    [[nodiscard]] std::string getSessionName() const;

    [[nodiscard]] std::string getConnectionInformation() const;

    [[nodiscard]] std::string getTime() const;

    [[nodiscard]] std::string getMedia() const;

    [[nodiscard]] int getRtpPort() const;

private:
    void parse() override;

    static int extractRtpPort(std::string data);

    std::string _version;
    std::string _originator;
    std::string _sessionName;
    std::string _connectionInformation;
    std::string _time;
    std::string _media;
    int _rtpPort{};
};

#endif
