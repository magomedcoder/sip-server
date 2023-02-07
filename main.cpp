#include <iostream>
#include "SipServer.hpp"
#include "Lib/cxxopts.hpp"

int main(int argc, char **argv) {
    cxxopts::Options options("Sip-server", "SIP-сервер для обработки VoIP-вызовов.");
    options.add_options()
            ("ip", "IP адрес", cxxopts::value<std::string>())
            ("port", "Порт", cxxopts::value<int>()->default_value(std::to_string(5060)));
    auto result = options.parse(argc, argv);
    if (result.count("р")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    try {
        std::string ip = result["ip"].as<std::string>();
        int port = result["port"].as<int>();
        SipServer server(ip, port);
        std::cout << "Сервер запущен." << std::endl;
        getchar();
    } catch (const cxxopts::OptionException &) {
        std::cout << "Пожалуйста, введите ip и порт." << std::endl;
    }
    return 0;
}
