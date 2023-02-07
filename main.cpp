#include <iostream>
#include "SipServer.hpp"

int main() {
    SipServer server("127.0.0.1", 5060);
    std::cout << "Сервер запущен" << std::endl;
    return 0;
}
