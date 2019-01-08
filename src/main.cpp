#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <printf.h>
#include <stringstream.h>
#include <types.h>
#include <utils.h>
#include <globals.h>

#define VERSION "0.1.0"

RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO
String command;
ReceiveMode receiveMode = ReceiveMode::Manual;

void setup(void)
{
  command.reserve(72);
  Serial.begin(115200);
  printf_begin();
  Serial.println("INFO: RF Bridge " VERSION);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
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