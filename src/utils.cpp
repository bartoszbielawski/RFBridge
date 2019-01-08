#include <utils.h>
#include <types.h>
#include <globals.h>
#include <stringstream.h>

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

int hexDigitToInt(uint8_t c)
{
    if ((c >= '0') and (c <= '9'))
        return c - '0';
    
    if ((c >= 'A') and (c <= 'F'))
        return c - 'A' + 10;
    
    if ((c >= 'a') and (c <= 'f'))
        return c - 'a' + 10;

    return -1;
}