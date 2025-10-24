#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "../common/constants.h"
#include "client_protocol.h"

class Client {
public:
    Client(const char* host, const char* port);
    void run();
private:
    ClientProtocol protocol;
};

#endif //CLIENT_H
