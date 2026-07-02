//
// Created by os on 6/26/26.
//

#include "../h/KConsole.h"
#include "../h/Buffer.hpp"

Buffer* KConsole::inputBuffer = nullptr;
Buffer* KConsole::outputBuffer = nullptr;

void KConsole::init() {
    KConsole::inputBuffer = new Buffer();
    KConsole::outputBuffer = new Buffer();
}

void KConsole::putc(char c) {
    outputBuffer->putb(c);

    if ((*(volatile char*)CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT) && !outputBuffer->empty()) {
        char out = outputBuffer->getb();
        *(volatile char*)CONSOLE_TX_DATA = out;
    }
}

char KConsole::getc() {
    return inputBuffer->getb();
}

void KConsole::putDirect(char c) {
    while (!(*(volatile char*)CONSOLE_STATUS & CONSOLE_TX_STATUS_BIT)) {}
    *(volatile char*)CONSOLE_TX_DATA = c;
}