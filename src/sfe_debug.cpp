/*
  sfe_debug.cpp

  Definitions for SparkFun_UBLOX_GNSS::SfeDebugPrint - see sfe_debug.h for why this class exists
  and how DevUBLOXGNSS, ubxMessageVector and nmeaMessageVector each come to inherit it.

  These seven methods are unchanged from the original DevUBLOXGNSS::debugPrint()/debugPrintln()
  implementations - only their qualification (and the class they belong to) has moved.
*/

#include "sfe_debug.h"

using namespace SparkFun_UBLOX_GNSS;

// Safely print messages
void SfeDebugPrint::debugPrint(const char *message, bool important)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Don't print if _printLimitedDebug is true and this message is not important
    if (!(_printLimitedDebug && !important))
      _debugSerial.print(message);
}
// Safely print messages
void SfeDebugPrint::debugPrintln(const char *message, bool important)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Don't print if _printLimitedDebug is true and this message is not important
    if (!(_printLimitedDebug && !important))
      _debugSerial.println(message);
}
// Safely print debug values
void SfeDebugPrint::debugPrint(uint32_t value, bool important)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Don't print if _printLimitedDebug is true and this message is not important
    if (!(_printLimitedDebug && !important))
      _debugSerial.print(value);
}
// Safely print debug values in a given base (e.g. HEX)
void SfeDebugPrint::debugPrint(uint32_t value, int printBase, bool important)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Don't print if _printLimitedDebug is true and this message is not important
    if (!(_printLimitedDebug && !important))
      _debugSerial.print(value, printBase);
}
// Safely print debug values
void SfeDebugPrint::debugPrintln(uint32_t value, bool important)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Don't print if _printLimitedDebug is true and this message is not important
    if (!(_printLimitedDebug && !important))
      _debugSerial.println(value);
}
// Safely print debug values in a given base (e.g. HEX)
void SfeDebugPrint::debugPrintln(uint32_t value, int printBase, bool important)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Don't print if _printLimitedDebug is true and this message is not important
    if (!(_printLimitedDebug && !important))
      _debugSerial.println(value, printBase);
}
// Safely print a blank debug line
void SfeDebugPrint::debugPrintln(void)
{
  if (_printDebug == true) // if _printDebug is true, prepare to print
    // Not important - don't print if doing limited debugging
    if (!_printLimitedDebug)
      _debugSerial.println();
}
