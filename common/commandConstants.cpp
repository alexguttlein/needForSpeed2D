#include "commandConstants.h"

// Metodo que inicializa el mapa tecla / bit
const std::unordered_map<CommandConstants::Key, CommandConstants::Bit> CommandConstants::keyToBitMap = {
    { W, BIT01 },
    { A, BIT02 },
    { S, BIT03 },
    { D, BIT04 },
    { KEY_1, BIT05 },
    { KEY_2, BIT06 },
    { KEY_3, BIT07 },
    { KEY_4, BIT08 },
};

// Metodo que inicializa el mapa bit / tecla
const std::unordered_map<CommandConstants::Bit, CommandConstants::Key> CommandConstants::bitToKeyMap = {
    { BIT01, W },
    { BIT02, A },
    { BIT03, S },
    { BIT04, D },
    { BIT05, KEY_1 },
    { BIT06, KEY_2 },
    { BIT07, KEY_3 },
    { BIT08, KEY_4 },
};

// Metodo que traduce un caracter de teclado a bit hexadecimal
CommandConstants::Bit CommandConstants::keyToBit(Key key) {
    auto it = keyToBitMap.find(key);
    if (it != keyToBitMap.end())
        return it->second;
    return static_cast<Bit>(0x00); // si no encuentra tiene un valor por defecto
}

// Metodo que traduce un bit hexadecimal a caracter de teclado
CommandConstants::Key CommandConstants::bitToKey(Bit bit) {
    auto it = bitToKeyMap.find(bit);
    if (it != bitToKeyMap.end())
        return it->second;
    return static_cast<Key>(0); // si no encuentra tiene un valor por defecto
}
