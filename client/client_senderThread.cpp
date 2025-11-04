#include "client_senderThread.h"

SenderThread::SenderThread(ClientProtocol &pro, Queue<SDL_KeyCode> &cmd_q) : protocol(pro), commandQueue(cmd_q){
}

void SenderThread::run() {
    while (should_keep_running()) {
        try {
            SDL_KeyCode key_code;
            commandQueue.try_pop(key_code);
            protocol.sendKey(key_code);
        } catch (std::exception &e) {
            std::cout << "Excepcion en el hilo sender" << std::endl;
        }
    }
}

