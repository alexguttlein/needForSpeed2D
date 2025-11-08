// #include "server_gameloop.h"
// #include "../common/queue.h"

// #include <thread>
// #include <chrono>
// #include <iostream>
// #include <string>

// int main() {
//     // cola de comandos para el GameLoop
//     Queue<std::string> commandQueue;

//     // precargar comandos (formato usado en tu GameLogic: "id:action" o solo "action")
//     // en tu GameLoop actual usás processCommand(1, command) — dejamos comandos simples
//     commandQueue.push("w");
//     commandQueue.push("w");
//     commandQueue.push("d");
//     commandQueue.push("w");
//     commandQueue.push("a");
//     commandQueue.push("s");

//     // crear y arrancar GameLoop en hilo separado
//     GameLoop gameLoop(commandQueue);
//     std::thread loopThread(&GameLoop::run, &gameLoop);

//     // dejar correr un tiempo para que procese algunos pasos y genere snapshots
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     // agregar más comandos mientras corre
//     commandQueue.push("w");
//     commandQueue.push("d");
//     commandQueue.push("w");

//     // esperar un poco más
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     // parar el loop y unir el hilo
//     gameLoop.stop();
//     if (loopThread.joinable()) loopThread.join();

//     std::cout << "Test GameLoop finalizado.\n";
//     return 0;
// }