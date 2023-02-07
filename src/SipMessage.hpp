//
// Created by magomedcoder on 07.02.2023.
//

#ifndef SIP_MESSAGE_HPP
#define SIP_MESSAGE_HPP

#include <netinet/in.h>
#include <iostream>

class SipMessage {
public:

    SipMessage(std::string message, sockaddr_in src);

    void setType(std::string value);

    void setHeader(std::string value);

    void setVia(std::string value);

    void setFrom(const std::string &value);

    void setTo(const std::string &value);

    void setCallID(std::string value);

    void setCSeq(std::string value);

    void setContact(std::string value);

    void setContentLength(std::string value);

    [[nodiscard]] std::string getType() const;

    [[nodiscard]] std::string getHeader() const;

    [[nodiscard]] std::string getVia() const;

    [[nodiscard]] std::string getFrom() const;

    [[nodiscard]] std::string getFromNumber() const;

    [[nodiscard]] std::string getTo() const;

    [[nodiscard]] std::string getToNumber() const;

    [[nodiscard]] std::string getCallID() const;

    [[nodiscard]] std::string getCSeq() const;

    [[nodiscard]] std::string getContact() const;

    [[nodiscard]] std::string getContactNumber() const;

    [[nodiscard]] std::string getContentLength() const;

    [[nodiscard]] sockaddr_in getSource() const;

    [[nodiscard]] std::string toString() const;

protected:
    virtual void parse();

    [[nodiscard]] bool isValidMessage() const;

    [[nodiscard]] static std::string extractNumber(const std::string &header) ;

    std::string _type;
    std::string _header;
    std::string _via;
    std::string _from;
    std::string _fromNumber;
    std::string _to;
    std::string _toNumber;
    std::string _callID;
    std::string _cSeq;
    std::string _contact;
    std::string _contactNumber;
    std::string _contentLength;
    std::string _messageStr;

    sockaddr_in _src;
};

#endif
