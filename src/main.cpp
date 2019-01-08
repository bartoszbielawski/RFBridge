#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <printf.h>
#include <stringstream.h>

#define VERSION "0.1.0"

RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

String command;

enum class ReceiveMode
{
  Manual,
  Binary,
  Hex,
  Invalid
};

ReceiveMode receiveMode = ReceiveMode::Manual;

void setup(void)
{
  command.reserve(64);
  Serial.begin(115200);
  printf_begin();
  Serial.println("INFO: RF Bridge " VERSION);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
}

bool readData(uint8_t* data, uint8_t& len, uint8_t& pipe)
{
  bool available = radio.available(&pipe);
  if (!available) 
    return false; 

  len = radio.getPayloadSize();
  radio.read(data, len);
  return true;
}

void readDataBinary(bool printEmpty)
{
  uint8_t pipe = 0;
  uint8_t len = 0;
  uint8_t data[32] = {0};

  bool available = readData(data, len, pipe);
  if (!available)
  {
    if (printEmpty) Serial.println(F("RDBX:-"));
    return;
  }

  printf("RDB%d:", pipe);
  Serial.write(data, len);
  Serial.println();
}

void readDataHex(bool printEmpty)
{
  uint8_t pipe = 0;
  uint8_t len = 0;
  uint8_t data[32] = {0};

  bool available = readData(data, len, pipe);
  if (!available)
  {
    if (printEmpty) Serial.println(F("RDHX:-"));
    return;
  }

  printf("RDH%d:", pipe);

  for (uint8_t i = 0; i < len; ++i)
  {
    Serial.print(data[i], 16);
  }
  Serial.println();
}



enum class WriteMode: uint8_t
{
  Bin,
  Hex
};





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



bool readHexDataFromStream(StringStream& ss, uint8_t* data, uint8_t& len);
bool readRawDataFromStream(StringStream& ss, uint8_t* data, uint8_t& len);

uint8_t writeData(StringStream& ss, WriteMode mode)
{
  uint8_t data[32];
  uint8_t len = radio.getPayloadSize();

  bool dataValid = false;

  switch (mode)
  {
    case WriteMode::Bin:
      dataValid = readRawDataFromStream(ss, data, len);
      break;

    case WriteMode::Hex:
      dataValid = readHexDataFromStream(ss, data, len);
      break;
  }

  if (!dataValid)
    return 0;

  radio.stopListening();
  radio.write(data, len);
  radio.startListening();

  return len;
}

bool readRawDataFromStream(StringStream& ss, uint8_t* data, uint8_t& len)
{
  uint8_t out = 0;
  bool isEscape = false;

  while (ss.available() && (out <= len))
  {
    uint8_t b = ss.read();
    if (b == '\\')
    {
      isEscape = true;
      continue;
    }

    if (isEscape)
    {
      switch (b)
      {
        case 'r': b = '\r'; break;
        case 'n': b = '\n'; break;
        case 't': b = '\t'; break;
        default: break;
      }
      isEscape = false;
    }
    *data++ = b;
    out++;
  }

  len = out;
  return true;
}

bool readHexDataFromStream(StringStream& ss, uint8_t* data, uint8_t& len)
{
  int savedPos = ss.getPositon();
  if (ss.available() & 1)
    return false;

  while (ss.available())
  {
    if (!isHexadecimalDigit(ss.read()))
      return false;
  }
  
  ss.setPositon(savedPos);

  len = min(ss.available()/2, len);

  for(int i = 0; i < len; i++)
  {
    uint8_t d = hexDigitToInt(ss.read()) << 4;
    d |= hexDigitToInt(ss.read());
    *data++ = d;
  }

  return true;
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

using CommandFunction = void (*)(StringStream& ss);

struct Command
{
  const char command[6];
  CommandFunction function;
};

void help(StringStream& commandStream);

const Command commands[] PROGMEM = 
{
  {"TPOW?", reportTransmitPower},
  {"TPOW=", setTransmitPower},
  {"PSIZ?", reportPayloadSize},
  {"PSIZ=", setPayloadSize},
  {"DATA?", isDataAvailable},
  {"PCLS=", closePipe},
  {"CARR?", isCarrierPresent},
  {"CHAN=", setChannel},
  {"CHAN?", reportChannel},
  {"RDDB?", readDataBinary},
  {"RDDX?", readDataHex},
  {"ARCE=", setAutoReceiveMode},
  {"ARCE?", reportAutoReceiveMode},
  {"WRDB=", writeDataBinary},
  {"WRDH=", writeDataHex},
  {"POPN=", openPipe},
  {"OPEN=", open},
  {"HELP!", help},
  {"TEST!", test},
  {"ERROR", nullptr}
};

void help(StringStream& commandStream)
{
  for (int i = 0; ; ++i)
  {
    Command commandStruct = {"", nullptr};
    memcpy_P(&commandStruct, &commands[i], sizeof(Command));
    if (!commandStruct.function)
      break;
    Serial.print("HELP:");
    Serial.println(commandStruct.command);
  }
}

void loop(void)
{
  switch (receiveMode)
  {
    case ReceiveMode::Binary: readDataBinary(false); break;
    case ReceiveMode::Hex:readDataHex(false); break;
    default:;
  }
  
  if (!Serial.available())
    return;

  while (Serial.available() > 0)
  {
    char c = Serial.read();
    
    if ((c != '\n') and (c != '\r'))
    {
      command += c;
      continue;
    }

    if (c == '\r')
      continue;

    if (command.length() == 0)
      continue;

    StringStream commandStream(command);
    commandStream.setTimeout(0);
    commandStream.discard(5);
    String cmd = command.substring(0, 5);
    cmd.toUpperCase();
    

    for (int i = 0;; ++i)
    {
        Command commandStruct = {"", nullptr};
        memcpy_P(&commandStruct, &commands[i], sizeof(Command));
        if (commandStruct.function == nullptr) 
        {
          Serial.print(F("CMD: Error, unknown command: "));        
          Serial.println(command);
          break;
        }

        if (cmd == commandStruct.command)
        {
          commandStruct.function(commandStream);
          break;
        }
    } 

    command = "";
  } //while available
}