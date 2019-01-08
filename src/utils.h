#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <types.h>


class StringStream;

bool readData(uint8_t* data, uint8_t& len, uint8_t& pipe);
void readDataBinary(bool printEmpty);
void readDataHex(bool printEmpty);

bool readHexDataFromStream(StringStream& ss, uint8_t* data, uint8_t& len);
bool readRawDataFromStream(StringStream& ss, uint8_t* data, uint8_t& len);

uint8_t writeData(StringStream& ss, WriteMode mode);

int hexDigitToInt(uint8_t c);

#endif //UTILS_H