#include <globals.h>
#include <commands.h>


TestMode testMode = TestMode::Disabled;
uint16_t testCounter = 0;

const Command commands[] PROGMEM = 
{
  {"TPOW?", reportTransmitPower, "Get TX power level"},
  {"TPOW=", setTransmitPower,    "Set RX power level"},
  {"PSIZ?", reportPayloadSize,   "Get payload size"},
  {"PSIZ=", setPayloadSize,      "Set payload size"},
  {"DATA?", isDataAvailable,     "Query RX data presence"},
  {"PCLS=", closePipe,           "Close pipe"},
  {"CARR?", isCarrierPresent,    "Query carrier presence"},
  {"CHAN=", setChannel,          "Set channel"},
  {"CHAN?", reportChannel,       "Get channel"},
  {"RDDB?", readDataBinary,      "Read data (binary)"},
  {"RDDX?", readDataHex,         "Read data (hex)"},
  {"ARCE=", setAutoReceiveMode,  "Set autoreceive mode"},
  {"ARCE?", reportAutoReceiveMode, "Get autoreceive mode"}, 
  {"WRDB=", writeDataBinary,     "Write data (binary)"},
  {"WRDH=", writeDataHex,        "Write data (hex)"},
  {"POPN=", openPipe,            "Open pipe"},
  {"OPEN=", open,                "Open pipes 0 and 1"},
  {"HELP!", help,                "Help!"},
  {"TEST=", setTestMode,         "Set test mode"},
  {"TEST?", reportTestMode,      "Get test mode"},
  {"ERROR", nullptr,             "No help :)"}
};
