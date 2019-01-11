#ifndef TYPES_H
#define TYPES_H

class StringStream;

enum class ReceiveMode: uint8_t
{
  Manual,
  Binary,
  Hex,
  Invalid
};

enum class WriteMode: uint8_t
{
  Bin,
  Hex
};

enum class TestMode: uint8_t
{
  Disabled,
  Counter,
  Invalid
};

using CommandFunction = void (*)(StringStream& ss);

struct Command
{
  const char command[6];
  CommandFunction function;
  const char help[32];
};

#endif //TYPES_H