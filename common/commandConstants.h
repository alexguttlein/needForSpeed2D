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
        KEY_1 = '1',
        KEY_2 = '2',
        KEY_3 = '3',
        KEY_4 = '4',
    };

    enum Bit : unsigned char {
        BIT01 = 0x01,
        BIT02 = 0x02,
        BIT03 = 0x03,
        BIT04 = 0x04,
       
        BIT05 = 0x05,
        BIT06 = 0x06,
        BIT07 = 0x07,
        BIT08 = 0x08,
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
