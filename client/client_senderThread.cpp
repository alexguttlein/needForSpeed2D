#include "client_senderThread.h"

SenderThread::SenderThread(ClientProtocol &pro, Queue<commandMessage> &cmd_q) : protocol(pro), commandQueue(cmd_q){
}

void SenderThread::run() {
    while (should_keep_running()) {
        try {
            commandMessage cmd_msg = commandQueue.pop();
            protocol.sendKey(cmd_msg.key, cmd_msg.isPressed);
        } catch (std::exception &e) {
            std::cout << "Excepcion en el hilo sender" << std::endl;
            break;
        }
    }
}

