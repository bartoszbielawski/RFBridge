#ifndef COMMANDS_H
#define COMMANDS_H

class StringStream;

void reportTransmitPower(StringStream& commandStream);
void setTransmitPower(StringStream& commandStream);

void reportPayloadSize(StringStream& commandStream);
void setPayloadSize(StringStream& commandStream);

void isDataAvailable(StringStream& commandStream);
void isCarrierPresent(StringStream& commandStream);

void openPipe(StringStream& commandStream);
void closePipe(StringStream& commandStream);
  
void reportChannel(StringStream& commandStream);
void setChannel(StringStream& commandStream);

void readDataBinary(StringStream& s);
void readDataHex(StringStream& s);

void reportAutoReceiveMode(StringStream& commandStream);
void setAutoReceiveMode(StringStream& commandStream);

void writeDataBinary(StringStream& commandStream);
void writeDataHex(StringStream& commandStream);

void open(StringStream& commandStream);
void help(StringStream& commandStream);

void setTestMode(StringStream& commandStream);
void reportTestMode(StringStream& commandStream);

#endif //COMMANDS_H