//
// Created by magomedcoder on 07.02.2023.
//

#ifndef REQUESTS_HANDLER_HPP
#define REQUESTS_HANDLER_HPP

#include <functional>
#include <unordered_map>
#include <optional>
#include "SipMessage.hpp"
#include "SipClient.hpp"
#include "Session.hpp"

class RequestsHandler {
public:

    using OnHandledEvent = std::function<void(const sockaddr_in &, std::shared_ptr<SipMessage>)>;

    RequestsHandler(std::string serverIp, int serverPort, OnHandledEvent onHandledEvent);

    void handle(std::shared_ptr<SipMessage> request);

    std::optional<std::shared_ptr<Session>> getSession(const std::string &callID);

private:
    void initHandlers();

    void OnRegister(const std::shared_ptr<SipMessage> &data);

    void OnCancel(const std::shared_ptr<SipMessage> &data);

    void onReqTerminated(const std::shared_ptr<SipMessage> &data);

    void OnInvite(const std::shared_ptr<SipMessage> &data);

    void OnTrying(const std::shared_ptr<SipMessage> &data);

    void OnRinging(const std::shared_ptr<SipMessage> &data);

    void OnBusy(const std::shared_ptr<SipMessage> &data);

    void OnUnavailable(const std::shared_ptr<SipMessage> &data);

    void OnBye(const std::shared_ptr<SipMessage> &data);

    void OnOk(const std::shared_ptr<SipMessage> &data);

    void OnAck(const std::shared_ptr<SipMessage> &data);

    bool setCallState(const std::string &callID, Session::State state);

    void endCall(
            const std::string &callID,
            const std::string &srcNumber,
            const std::string &destNumber,
            const std::string &reason = ""
    );

    bool registerClient(const std::shared_ptr<SipClient> &client);

    void unregisterClient(const std::shared_ptr<SipClient> &client);

    std::optional<std::shared_ptr<SipClient>> findClient(const std::string &number);

    void endHandle(const std::string &destNumber, std::shared_ptr<SipMessage> message);

    std::unordered_map<std::string, std::function<void(std::shared_ptr<SipMessage> request)>> _handlers;
    std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;
    std::unordered_map<std::string, std::shared_ptr<SipClient>> _clients;

    std::function<void(std::shared_ptr<SipClient>)> _onNewClient;
    std::function<void(std::shared_ptr<SipClient>)> _onUnregister;
    OnHandledEvent _onHandled;

    std::string _serverIp;
    int _serverPort;
};

#endif
