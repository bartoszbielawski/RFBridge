#ifndef GLOBALS_H
#define GLOBALS_H

#include <RF24.h>
#include <nRF24L01.h>
#include <types.h>

extern RF24 radio;
extern ReceiveMode  receiveMode;
extern TestMode     testMode;
extern uint16_t     testCounter;

extern const Command commands[];

#endif //GLOBALS_H
