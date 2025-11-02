#ifndef COMMANDCONSTANTS_H
#define COMMANDCONSTANTS_H

#include <unordered_map>

class CommandConstants {
public:
    // ENUMS DE COMANDOS
    enum Key : char {
        W = 'w',
        A = 'a',
        S = 's',
        D = 'd',
    };

    enum Bit : unsigned char {
        BIT01 = 0x01,
        BIT02 = 0x02,
        BIT03 = 0x03,
        BIT04 = 0x04,
    };

    // MÉTODOS DE TRADUCCIÓN
    static Bit keyToBit(Key key);
    static Key bitToKey(Bit bit);

private:
    // MAPAS DE TRADUCCIÓN
    static const std::unordered_map<Key, Bit> keyToBitMap;
    static const std::unordered_map<Bit, Key> bitToKeyMap;
};

#endif //COMMANDCONSTANTS_H
