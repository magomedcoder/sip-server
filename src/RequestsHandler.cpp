//
// Created by magomedcoder on 07.02.2023.
//

#include "RequestsHandler.hpp"
#include <sstream>
#include <utility>
#include "SipMessageTypes.h"
#include "SipSdpMessage.hpp"
#include "IDGen.hpp"

RequestsHandler::RequestsHandler(std::string serverIp, int serverPort, OnHandledEvent onHandledEvent) :
        _serverIp(std::move(serverIp)),
        _serverPort(serverPort),
        _onHandled(std::move(onHandledEvent)) {
    initHandlers();
}

void RequestsHandler::initHandlers() {
    _handlers.emplace(SipMessageTypes::REGISTER, [this](auto &&PH1) {
        OnRegister(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::CANCEL, [this](auto &&PH1) {
        OnCancel(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::INVITE, [this](auto &&PH1) {
        OnInvite(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::TRYING, [this](auto &&PH1) {
        OnTrying(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::RINGING, [this](auto &&PH1) {
        OnRinging(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::BUSY, [this](auto &&PH1) {
        OnBusy(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::UNAVAIALBLE, [this](auto &&PH1) {
        OnUnavailable(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::OK, [this](auto &&PH1) {
        OnOk(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::ACK, [this](auto &&PH1) {
        OnAck(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::BYE, [this](auto &&PH1) {
        OnBye(std::forward<decltype(PH1)>(PH1));
    });
    _handlers.emplace(SipMessageTypes::REQUEST_TERMINATED, [this](auto &&PH1) {
        onReqTerminated(std::forward<decltype(PH1)>(PH1));
    });
}

void RequestsHandler::handle(std::shared_ptr<SipMessage> request) {
    if (_handlers.find(request->getType()) != _handlers.end()) {
        _handlers[request->getType()](std::move(request));
    }
}

std::optional<std::shared_ptr<Session>> RequestsHandler::getSession(const std::string &callID) {
    auto sessionIt = _sessions.find(callID);
    if (sessionIt != _sessions.end()) {
        return sessionIt->second;
    }
    return {};
}

void RequestsHandler::OnRegister(const std::shared_ptr<SipMessage> &data) {
    bool isUnregisterReq = data->getContact().find("expires=0") != -1;
    if (!isUnregisterReq) {
        auto newClient = std::make_shared<SipClient>(
                data->getFromNumber(),
                data->getSource()
        );
        registerClient(newClient);
    }
    const auto &response = data;
    response->setHeader(SipMessageTypes::OK);
    response->setVia(data->getVia() + ";received=" + _serverIp);
    response->setTo(data->getTo() + ";tag=" + IDGen::GenerateID(9));
    response->setContact(
            "Contact: <sip:" + data->getFromNumber()
            + "@" + _serverIp
            + ":" + std::to_string(_serverPort)
            + ";transport=UDP>"
    );
    endHandle(response->getFromNumber(), response);
    if (isUnregisterReq) {
        auto newClient = std::make_shared<SipClient>(
                data->getFromNumber(),
                data->getSource()
        );
        unregisterClient(newClient);
    }
}

void RequestsHandler::OnCancel(const std::shared_ptr<SipMessage> &data) {
    setCallState(data->getCallID(), Session::State::Cancel);
    endHandle(data->getToNumber(), data);
}

void RequestsHandler::onReqTerminated(const std::shared_ptr<SipMessage> &data) {
    endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnInvite(const std::shared_ptr<SipMessage> &data) {
    // Зарегистрирован ли вызывающий абонент
    auto caller = findClient(data->getFromNumber());
    if (!caller.has_value()) {
        return;
    }
    // Зарегистрирован ли вызываемый
    auto called = findClient(data->getToNumber());
    if (!called.has_value()) {
        // Отправка "SIP/2.0 404 Not Found"
        data->setHeader(SipMessageTypes::NOT_FOUND);
        data->setContact(
                "Contact: <sip:" + caller.value()->getNumber()
                + "@" + _serverIp
                + ":" + std::to_string(_serverPort)
                + ";transport=UDP>"
        );
        endHandle(data->getFromNumber(), data);
        return;
    }
    auto messge = dynamic_cast<SipSdpMessage *>(data.get());
    auto newSession = std::make_shared<Session>(
            data->getCallID(),
            caller.value(),
            messge->getRtpPort()
    );
    _sessions.emplace(data->getCallID(), newSession);
    const auto &response = data;
    response->setContact(
            "Contact: <sip:" + caller.value()->getNumber()
            + "@" + _serverIp
            + ":" + std::to_string(_serverPort)
            + ";transport=UDP>"
    );
    endHandle(data->getToNumber(), response);
}

void RequestsHandler::OnTrying(const std::shared_ptr<SipMessage> &data) {
    endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnRinging(const std::shared_ptr<SipMessage> &data) {
    endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnBusy(const std::shared_ptr<SipMessage> &data) {
    setCallState(data->getCallID(), Session::State::Busy);
    endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnUnavailable(const std::shared_ptr<SipMessage> &data) {
    setCallState(data->getCallID(), Session::State::Unavailable);
    endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnBye(const std::shared_ptr<SipMessage> &data) {
    setCallState(data->getCallID(), Session::State::Bye);
    endHandle(data->getToNumber(), data);
}

void RequestsHandler::OnOk(const std::shared_ptr<SipMessage> &data) {
    auto session = getSession(data->getCallID());
    if (session.has_value()) {
        if (session.value()->getState() == Session::State::Cancel) {
            endHandle(data->getFromNumber(), data);
            return;
        }
        if (data->getCSeq().find(SipMessageTypes::INVITE) != std::string::npos) {
            auto client = findClient(data->getToNumber());
            if (!client.has_value()) {
                return;
            }
            auto sdpMessage = dynamic_cast<SipSdpMessage *>(data.get());
            session->get()->setDest(client.value(), sdpMessage->getRtpPort());
            session->get()->setState(Session::State::Connected);
            auto response = data;
            response->setContact(
                    "Contact: <sip:" + data->getToNumber()
                    + "@" + _serverIp
                    + ":" + std::to_string(_serverPort)
                    + ";transport=UDP>"
            );
            endHandle(data->getFromNumber(), std::move(response));
            return;
        }
        if (session.value()->getState() == Session::State::Bye) {
            endHandle(data->getFromNumber(), data);
            endCall(data->getCallID(), data->getToNumber(), data->getFromNumber());
        }
    }
}

void RequestsHandler::OnAck(const std::shared_ptr<SipMessage> &data) {
    auto session = getSession(data->getCallID());
    if (!session.has_value()) {
        return;
    }
    endHandle(data->getToNumber(), data);
    auto sessionState = session.value()->getState();
    std::string endReason;
    if (sessionState == Session::State::Busy) {
        endReason = data->getToNumber() + " занят.";
        endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), endReason);
        return;
    }
    if (sessionState == Session::State::Unavailable) {
        endReason = data->getToNumber() + " недоступен.";
        endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), endReason);
        return;
    }
    if (sessionState == Session::State::Cancel) {
        endReason = data->getFromNumber() + " отменил сеанс.";
        endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), endReason);
        return;
    }
}

bool RequestsHandler::setCallState(const std::string &callID, Session::State state) {
    auto session = getSession(callID);
    if (session) {
        session->get()->setState(state);
        return true;
    }
    return false;
}

void RequestsHandler::endCall(
        const std::string &callID,
        const std::string &srcNumber,
        const std::string &destNumber,
        const std::string &reason
) {
    if (_sessions.erase(callID) > 0) {
        std::ostringstream message;
        message << "Сессия была разорвана между " << srcNumber << " и " << destNumber;
        if (!reason.empty()) {
            message << " потому что " << reason;
        }
        std::cout << message.str() << std::endl;
    }
}

bool RequestsHandler::registerClient(const std::shared_ptr<SipClient> &client) {
    if (_clients.find(client->getNumber()) == _clients.end()) {
        std::cout << "Клиент подключен: " << client->getNumber() << std::endl;
        _clients.emplace(client->getNumber(), client);
        return true;
    }
    return false;
}

void RequestsHandler::unregisterClient(const std::shared_ptr<SipClient> &client) {
    std::cout << "Клиент отключен: " << client->getNumber() << std::endl;
    _clients.erase(client->getNumber());
}

std::optional<std::shared_ptr<SipClient>> RequestsHandler::findClient(const std::string &number) {
    auto it = _clients.find(number);
    if (it != _clients.end()) {
        return it->second;
    }
    return {};
}

void RequestsHandler::endHandle(const std::string &destNumber, std::shared_ptr<SipMessage> message) {
    auto destClient = findClient(destNumber);
    if (destClient.has_value()) {
        _onHandled(destClient.value()->getAddress(), std::move(message));
    } else {
        message->setHeader(SipMessageTypes::NOT_FOUND);
        auto src = message->getSource();
        _onHandled(src, std::move(message));
    }
}
