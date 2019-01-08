
#include <Arduino.h>
#include <types.h>
#include <globals.h>
#include <utils.h>
#include <commands.h>
#include <stringstream.h>

void reportTransmitPower(StringStream& commandStream)
{
  uint8_t paLevel = radio.getPALevel();
  Serial.print("TPOW:");
  Serial.println(paLevel);
}

void setTransmitPower(StringStream& commandStream)
{
  uint8_t paLevel = commandStream.parseInt();
  radio.setPALevel(paLevel);
  reportTransmitPower(commandStream);
}

void reportPayloadSize(StringStream& commandStream)
{
  uint8_t payloadSize = radio.getPayloadSize();
  Serial.print("PSIZ:");
  Serial.println(payloadSize);
}

void setPayloadSize(StringStream& commandStream)
{
  uint8_t paLevel = commandStream.parseInt();
  radio.setPayloadSize(paLevel);
  reportPayloadSize(commandStream);
}

void isDataAvailable(StringStream& commandStream)
{
  uint8_t pipe = 0;
  Serial.print("DATA:");
  bool available = radio.available(&pipe);
  char x = available ? pipe + '0': '-';
  Serial.println(x);
}

void closePipe(StringStream& commandStream)
{
  uint8_t pipe = commandStream.parseInt();
  if ((pipe == 0) or (pipe >= 6))
  {
     Serial.println(F("PCLS: Invalid pipe number!"));
     return;
  }
 
  radio.closeReadingPipe(pipe);
  Serial.println(F("PCLS:OK"));   
}


void isCarrierPresent(StringStream& commandStream)
{
  Serial.print(F("CARR:"));
  Serial.println(radio.testCarrier());
}
  
void reportChannel(StringStream& commandStream)
{
  Serial.print(F("CHAN:"));
  Serial.println(radio.getChannel());
}

void setChannel(StringStream& commandStream)
{
  uint8_t channel = commandStream.parseInt();
  radio.setChannel(channel);
  reportChannel(commandStream);
}


void readDataBinary(StringStream& s)
{
  readDataBinary(true);
}

void readDataHex(StringStream& s)
{
  readDataHex(true);
}    

void reportAutoReceiveMode(StringStream& commandStream)
{
  Serial.print(F("ARCE:"));
  switch (receiveMode)
  {
    case ReceiveMode::Manual: Serial.println(F("MANUAL")); break;
    case ReceiveMode::Binary: Serial.println(F("BINARY")); break;
    case ReceiveMode::Hex: Serial.println(F("HEX")); break;
    case ReceiveMode::Invalid: Serial.println(F("Invalid")); break;
    default: break;
  }
}

void setAutoReceiveMode(StringStream& commandStream)
{
  String v = commandStream.readString();
  receiveMode = ReceiveMode::Invalid;
  if (v == F("MANUAL")) receiveMode = ReceiveMode::Manual;
  if (v == F("BINARY")) receiveMode = ReceiveMode::Binary;
  if (v == F("HEX")) receiveMode = ReceiveMode::Hex;

  reportAutoReceiveMode(commandStream);

  if (receiveMode == ReceiveMode::Invalid)
    receiveMode = ReceiveMode::Manual;
}
        

void openPipe(StringStream& commandStream)
{
  uint8_t address[5] = {0};
  uint8_t pipe = commandStream.parseInt();
  commandStream.readStringUntil(',');
  commandStream.readBytes(address, 5);

  switch (pipe)
  {
    case 0:
      radio.openWritingPipe(address);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      radio.openReadingPipe(pipe, address);
      break;
    default:
      Serial.println(F("POPN: Invalid address!"));
      break;
  }
  printf("POPN:%d\n",(int)pipe);
}

void writeDataBinary(StringStream& commandStream)
{
  uint8_t len = writeData(commandStream, WriteMode::Bin);
  Serial.print(F("WRTB:"));
  Serial.println(len);
}

void writeDataHex(StringStream& commandStream)
{
  uint8_t len = writeData(commandStream, WriteMode::Bin);
  Serial.print(F("WRTH:"));
  Serial.println(len);
}

void open(StringStream& commandStream)
{
  uint8_t address[5] = {0};
  commandStream.readBytes(address, 5);
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  Serial.println(F("OPEN:OK"));
}

void test(StringStream& commandStream)
{
  int16_t counter = 0;
  Serial.println(F("TEST:START"));
  while (!Serial.available())
  {
    char data[5] = {0};
    snprintf(data, 5, "%04X", counter++);
    Serial.print(F("TEST:Sending "));
    Serial.println(data);
    radio.write(data, 4);
    delay(1000);
  }
  Serial.println(F("TEST:DONE"));
}

void help(StringStream& commandStream)
{
  for (int i = 0; ; ++i)
  {
    Command commandStruct = {"", nullptr};
    memcpy_P(&commandStruct, &commands[i], sizeof(Command));
    if (!commandStruct.function)
      break;
    Serial.print("HELP:");
    Serial.print(commandStruct.command);
    Serial.print(" - ");
    Serial.println(commandStruct.help);
  }
}