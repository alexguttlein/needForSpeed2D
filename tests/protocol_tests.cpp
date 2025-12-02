#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <chrono>

#include "../common/commandConstants.h"
#include "../common/message.h"
#include "../common/snapshot.h"
#include "../common/carStateDTO.h"
#include "../common/raceStateDTO.h"
#include "../common/gameInfo.h"
#include "../common/constants.h"
#include "../common/eventType.h"
#include "../server/leaderBoard.h"


class ProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        
    }

    void TearDown() override {
        
    }

    uint32_t hostToNetworkUint32(uint32_t hostValue) {
        return htonl(hostValue);
    }

    uint32_t networkToHostUint32(uint32_t networkValue) {
        return ntohl(networkValue);
    }

    uint16_t hostToNetworkUint16(uint16_t hostValue) {
        return htons(hostValue);
    }

    uint16_t networkToHostUint16(uint16_t networkValue) {
        return ntohs(networkValue);
    }

    enum TestKeyCode {
        TEST_KEY_w = 'w',
        TEST_KEY_a = 'a',
        TEST_KEY_s = 's', 
        TEST_KEY_d = 'd',
        TEST_KEY_1 = '1',
        TEST_KEY_2 = '2',
        TEST_KEY_3 = '3',
        TEST_KEY_4 = '4',
        TEST_KEY_z = 'z',
        TEST_KEY_SPACE = ' '
    };

    CommandConstants::Key testKeyToInternalKey(TestKeyCode input) {
        switch (input) {
            case TEST_KEY_w: return CommandConstants::W;
            case TEST_KEY_a: return CommandConstants::A;
            case TEST_KEY_s: return CommandConstants::S;
            case TEST_KEY_d: return CommandConstants::D;
            case TEST_KEY_1: return CommandConstants::KEY_1;
            case TEST_KEY_2: return CommandConstants::KEY_2;
            case TEST_KEY_3: return CommandConstants::KEY_3;
            case TEST_KEY_4: return CommandConstants::KEY_4;
            default: throw std::invalid_argument("Tecla no válida");
        }
    }

    uint8_t encodeKeyMessage(TestKeyCode key, bool isPressed) {
        auto internalKey = testKeyToInternalKey(key);
        uint8_t msg = CommandConstants::keyToBit(internalKey);
        if (isPressed) {
            msg |= 0x80; 
        }
        return msg;
    }

    char decodeKeyMessage(uint8_t command) {
        const uint8_t KEY_BITS = command & 0x7F; 
        try {
            CommandConstants::Key key = CommandConstants::bitToKey(
                static_cast<CommandConstants::Bit>(KEY_BITS)
            );
            return static_cast<char>(key);
        } catch (const std::invalid_argument& e) {
            return '\0';
        }
    }


    std::vector<uint8_t> serializeString(const std::string& str) {
        std::vector<uint8_t> buffer;
        uint16_t size = htons(static_cast<uint16_t>(str.length()));
        
        const uint8_t* sizeBytes = reinterpret_cast<const uint8_t*>(&size);
        buffer.insert(buffer.end(), sizeBytes, sizeBytes + sizeof(size));
        
        buffer.insert(buffer.end(), str.begin(), str.end());
        
        return buffer;
    }


    std::string deserializeString(const std::vector<uint8_t>& buffer, size_t& offset) {
        if (offset + 2 > buffer.size()) {
            throw std::runtime_error("Buffer too small for string size");
        }
        
        uint16_t sizeNetwork;
        std::memcpy(&sizeNetwork, &buffer[offset], sizeof(sizeNetwork));
        offset += sizeof(sizeNetwork);
        
        uint16_t size = ntohs(sizeNetwork);
        
        if (offset + size > buffer.size()) {
            throw std::runtime_error("Buffer too small for string data");
        }
        
        std::string result(buffer.begin() + offset, buffer.begin() + offset + size);
        offset += size;
        
        return result;
    }
};

// Prueba conversión de tecla a bit de CommandConstants
TEST_F(ProtocolTest, CommandConstants_KeyToBit_ValidKeys) {
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::W), CommandConstants::BIT01);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::A), CommandConstants::BIT02);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::S), CommandConstants::BIT03);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::D), CommandConstants::BIT04);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::KEY_1), CommandConstants::BIT05);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::KEY_2), CommandConstants::BIT06);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::KEY_3), CommandConstants::BIT07);
    EXPECT_EQ(CommandConstants::keyToBit(CommandConstants::KEY_4), CommandConstants::BIT08);
}

// Prueba conversión de bit a tecla de CommandConstants
TEST_F(ProtocolTest, CommandConstants_BitToKey_ValidBits) {
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT01), CommandConstants::W);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT02), CommandConstants::A);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT03), CommandConstants::S);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT04), CommandConstants::D);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT05), CommandConstants::KEY_1);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT06), CommandConstants::KEY_2);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT07), CommandConstants::KEY_3);
    EXPECT_EQ(CommandConstants::bitToKey(CommandConstants::BIT08), CommandConstants::KEY_4);
}

// Prueba conversión de tecla desde código de prueba a código interno
TEST_F(ProtocolTest, ClientProtocol_TestKeyToInternal_ValidKeys) {
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_w), CommandConstants::W);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_a), CommandConstants::A);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_s), CommandConstants::S);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_d), CommandConstants::D);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_1), CommandConstants::KEY_1);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_2), CommandConstants::KEY_2);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_3), CommandConstants::KEY_3);
    EXPECT_EQ(testKeyToInternalKey(TEST_KEY_4), CommandConstants::KEY_4);
}

// Prueba conversión de teclas con teclas inválidas
TEST_F(ProtocolTest, ClientProtocol_TestKeyToInternal_InvalidKey) {
    EXPECT_THROW(testKeyToInternalKey(TEST_KEY_z), std::invalid_argument);
    EXPECT_THROW(testKeyToInternalKey(TEST_KEY_SPACE), std::invalid_argument);
}

// Prueba codificación de mensaje de tecla (lado cliente)
TEST_F(ProtocolTest, ClientProtocol_KeyEncoding_KeyPressed) {
    uint8_t encodedW = encodeKeyMessage(TEST_KEY_w, true);
    EXPECT_EQ(encodedW & 0x80, 0x80); 
    EXPECT_EQ(encodedW & 0x7F, CommandConstants::BIT01); 

    uint8_t encodedA = encodeKeyMessage(TEST_KEY_a, false);
    EXPECT_EQ(encodedA & 0x80, 0x00); 
    EXPECT_EQ(encodedA & 0x7F, CommandConstants::BIT02); 
}

// Prueba decodificación de mensaje de tecla (lado servidor)
TEST_F(ProtocolTest, ServerProtocol_KeyDecoding) {
    uint8_t wPressed = CommandConstants::BIT01 | 0x80;
    char decodedW = decodeKeyMessage(wPressed);
    EXPECT_EQ(decodedW, 'w');

    uint8_t aReleased = CommandConstants::BIT02;
    char decodedA = decodeKeyMessage(aReleased);
    EXPECT_EQ(decodedA, 'a');

    // Prueba comando inválido
    uint8_t invalid = 0xFF;
    char decodedInvalid = decodeKeyMessage(invalid);
    EXPECT_EQ(decodedInvalid, '\0');
}

// Prueba conversión de orden de bytes de red para uint32
TEST_F(ProtocolTest, Protocol_NetworkByteOrder_Uint32) {
    uint32_t hostValue = 0x12345678;
    uint32_t networkValue = hostToNetworkUint32(hostValue);
    uint32_t backToHost = networkToHostUint32(networkValue);
    
    EXPECT_EQ(hostValue, backToHost);
}

// Prueba conversión de orden de bytes de red para uint16
TEST_F(ProtocolTest, Protocol_NetworkByteOrder_Uint16) {
    uint16_t hostValue = 0x1234;
    uint16_t networkValue = hostToNetworkUint16(hostValue);
    uint16_t backToHost = networkToHostUint16(networkValue);
    
    EXPECT_EQ(hostValue, backToHost);
}

// Prueba serialización/deserialización de strings
TEST_F(ProtocolTest, Protocol_StringSerialization) {
    std::string originalString = "TestPlayer123";
    
    std::vector<uint8_t> serialized = serializeString(originalString);
    
    EXPECT_EQ(serialized.size(), 2 + originalString.length());
    
    size_t offset = 0;
    std::string deserializedString = deserializeString(serialized, offset);
    
    EXPECT_EQ(originalString, deserializedString);
    EXPECT_EQ(offset, serialized.size());
}

// Prueba serialización de string vacío
TEST_F(ProtocolTest, Protocol_EmptyStringSerialization) {
    std::string emptyString = "";
    
    std::vector<uint8_t> serialized = serializeString(emptyString);
    EXPECT_EQ(serialized.size(), 2);
    
    size_t offset = 0;
    std::string deserialized = deserializeString(serialized, offset);
    
    EXPECT_EQ(emptyString, deserialized);
    EXPECT_TRUE(deserialized.empty());
}

// Prueba inicialización de estructura Message
TEST_F(ProtocolTest, Protocol_MessageStructure) {
    Message defaultMsg;
    EXPECT_EQ(defaultMsg.code, 0);
    EXPECT_EQ(defaultMsg.key, '\0');
    EXPECT_EQ(defaultMsg.intValue, 0);
    EXPECT_TRUE(defaultMsg.stringValue.empty());
    EXPECT_EQ(defaultMsg.senderId, -1);
    EXPECT_EQ(defaultMsg.carId, -1);

    Message paramMsg(Constants::CREATE_GAME, 'w', 42);
    EXPECT_EQ(paramMsg.code, Constants::CREATE_GAME);
    EXPECT_EQ(paramMsg.key, 'w');
    EXPECT_EQ(paramMsg.senderId, 42);
}

// Prueba inicialización de estructura Snapshot
TEST_F(ProtocolTest, Protocol_SnapshotStructure) {
    Snapshot snapshot;
    
    // Prueba valores por defecto
    EXPECT_EQ(snapshot.raceFinished, false);
    EXPECT_EQ(snapshot.gameFinished, false);
    EXPECT_TRUE(snapshot.cars.empty());
    EXPECT_TRUE(snapshot.gameList.empty());
    EXPECT_TRUE(snapshot.raceStates.empty());
    EXPECT_TRUE(snapshot.leaderboards.empty());
    EXPECT_TRUE(snapshot.collisions.empty());
}

// Prueba estructura CollisionEvent
TEST_F(ProtocolTest, Protocol_CollisionEvent) {
    CollisionEvent carCollision(1, EventType::COLLISION_CAR);
    EXPECT_EQ(carCollision.playerId, 1);
    EXPECT_EQ(carCollision.collisionType, EventType::COLLISION_CAR);

    CollisionEvent buildingCollision(2, EventType::COLLISION_BUILDING);
    EXPECT_EQ(buildingCollision.playerId, 2);
    EXPECT_EQ(buildingCollision.collisionType, EventType::COLLISION_BUILDING);
}

// Prueba simulación de comando de lobby
TEST_F(ProtocolTest, Protocol_LobbyCommands_CreateGame) {

    std::vector<uint8_t> buffer;
    
    buffer.push_back(Constants::CREATE_GAME);
    
    std::string playerName = "TestPlayer";
    auto nameData = serializeString(playerName);
    buffer.insert(buffer.end(), nameData.begin(), nameData.end());
    
    uint32_t carId = 3;
    uint32_t carIdBE = htonl(carId);
    const uint8_t* carIdBytes = reinterpret_cast<const uint8_t*>(&carIdBE);
    buffer.insert(buffer.end(), carIdBytes, carIdBytes + sizeof(carIdBE));
    
    EXPECT_EQ(buffer[0], Constants::CREATE_GAME);
    
    size_t offset = 1; 
    std::string parsedName = deserializeString(buffer, offset);
    EXPECT_EQ(parsedName, playerName);
    
    uint32_t parsedCarIdBE;
    std::memcpy(&parsedCarIdBE, &buffer[offset], sizeof(parsedCarIdBE));
    uint32_t parsedCarId = ntohl(parsedCarIdBE);
    EXPECT_EQ(parsedCarId, carId);
}

// Prueba simulación de comando de lobby para JOIN_GAME
TEST_F(ProtocolTest, Protocol_LobbyCommands_JoinGame) {
    std::vector<uint8_t> buffer;
    
    buffer.push_back(Constants::JOIN_GAME);
    
    uint16_t matchId = 1234;
    uint16_t matchIdBE = htons(matchId);
    const uint8_t* matchIdBytes = reinterpret_cast<const uint8_t*>(&matchIdBE);
    buffer.insert(buffer.end(), matchIdBytes, matchIdBytes + sizeof(matchIdBE));
    
    std::string playerName = "JoiningPlayer";
    auto nameData = serializeString(playerName);
    buffer.insert(buffer.end(), nameData.begin(), nameData.end());
    
    uint32_t carId = 2;
    uint32_t carIdBE = htonl(carId);
    const uint8_t* carIdBytes = reinterpret_cast<const uint8_t*>(&carIdBE);
    buffer.insert(buffer.end(), carIdBytes, carIdBytes + sizeof(carIdBE));
    
    size_t offset = 1; 
    
    uint16_t parsedMatchIdBE;
    std::memcpy(&parsedMatchIdBE, &buffer[offset], sizeof(parsedMatchIdBE));
    offset += sizeof(parsedMatchIdBE);
    uint16_t parsedMatchId = ntohs(parsedMatchIdBE);
    EXPECT_EQ(parsedMatchId, matchId);
    
    std::string parsedName = deserializeString(buffer, offset);
    EXPECT_EQ(parsedName, playerName);
    
    uint32_t parsedCarIdBE;
    std::memcpy(&parsedCarIdBE, &buffer[offset], sizeof(parsedCarIdBE));
    uint32_t parsedCarId = ntohl(parsedCarIdBE);
    EXPECT_EQ(parsedCarId, carId);
}

// Prueba estructura de serialización de datos de snapshot
TEST_F(ProtocolTest, Protocol_SnapshotSerialization_Structure) {
    CarStateDTO carState;
    carState.car_id = 1;
    carState.car_type_id = 2;
    carState.currentUpgradeId = 3;
    carState.health = 100.0f;
    carState.speed = 25.5f;
    carState.position.x = 10.0f;
    carState.position.y = 20.0f;
    
    std::vector<uint8_t> buffer;
    
    buffer.push_back(Constants::TYPE_SNAPSHOT);
    
    buffer.push_back(static_cast<uint8_t>(EventType::GAME_START));
    
    uint32_t playerId = 42;
    uint32_t playerIdBE = htonl(playerId);
    const uint8_t* playerIdBytes = reinterpret_cast<const uint8_t*>(&playerIdBE);
    buffer.insert(buffer.end(), playerIdBytes, playerIdBytes + sizeof(playerIdBE));
 
    uint32_t playersSize = 1;
    uint32_t playersSizeBE = htonl(playersSize);
    const uint8_t* playersSizeBytes = reinterpret_cast<const uint8_t*>(&playersSizeBE);
    buffer.insert(buffer.end(), playersSizeBytes, playersSizeBytes + sizeof(playersSizeBE));
    
    auto addInt = [&buffer](int value) {
        uint32_t valueBE = htonl(static_cast<uint32_t>(value));
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&valueBE);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(valueBE));
    };
    
    auto addFloat = [&buffer](float value) {
        uint32_t valueBE = htonl(*reinterpret_cast<const uint32_t*>(&value));
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&valueBE);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(valueBE));
    };
    
    addInt(carState.car_id);
    addInt(carState.car_type_id);
    addInt(carState.currentUpgradeId);
    addFloat(carState.health);
    addFloat(carState.speed);
    addFloat(carState.position.x);
    addFloat(carState.position.y);
    
    EXPECT_EQ(buffer[0], Constants::TYPE_SNAPSHOT);
    EXPECT_EQ(buffer[1], static_cast<uint8_t>(EventType::GAME_START));
    
    size_t offset = 2; 
    
    uint32_t parsedPlayerIdBE;
    std::memcpy(&parsedPlayerIdBE, &buffer[offset], sizeof(parsedPlayerIdBE));
    offset += sizeof(parsedPlayerIdBE);
    uint32_t parsedPlayerId = ntohl(parsedPlayerIdBE);
    EXPECT_EQ(parsedPlayerId, playerId);
    
    uint32_t parsedPlayersSizeBE;
    std::memcpy(&parsedPlayersSizeBE, &buffer[offset], sizeof(parsedPlayersSizeBE));
    offset += sizeof(parsedPlayersSizeBE);
    uint32_t parsedPlayersSize = ntohl(parsedPlayersSizeBE);
    EXPECT_EQ(parsedPlayersSize, playersSize);
}

// Prueba manejo de errores para comandos inválidos
TEST_F(ProtocolTest, Protocol_InvalidCommandHandling) {
    
    uint8_t invalidCommand = 0xFF;
    char result = decodeKeyMessage(invalidCommand);
    EXPECT_EQ(result, '\0'); 
    
}

// Prueba de rendimiento para operaciones de protocolo
TEST_F(ProtocolTest, Protocol_Performance_KeyConversions) {
    const int iterations = 1000000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        TestKeyCode keys[] = {TEST_KEY_w, TEST_KEY_a, TEST_KEY_s, TEST_KEY_d};
        for (auto key : keys) {
            auto internalKey = testKeyToInternalKey(key);
            auto bit = CommandConstants::keyToBit(internalKey);
            auto backToKey = CommandConstants::bitToKey(bit);
            (void)backToKey; 
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_LT(duration.count(), 2000000); 
}