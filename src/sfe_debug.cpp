/**
 * @date 2026
 * @copyright Copyright (c) 2026, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 * 
 * Please see LICENSE.md for more details
 * 
 * An Arduino Library which allows you to communicate seamlessly with u-blox GNSS modules using the Configuration Interface
 * 
 * SparkFun sells these at its website: www.sparkfun.com
 * Do you like this library? Help support SparkFun. Buy a board!
 * https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html
 *
 * Original version by Nathan Seidle @ SparkFun Electronics, September 6th, 2018
 * v2.0 rework by Paul Clark @ SparkFun Electronics, December 31st, 2020
 * v3.0 rework by Paul Clark @ SparkFun Electronics, December 8th, 2022
 * v4.0 rework by Claude, directed by Paul Clark @ SparkFun Electronics, September 2026
 *
 * https://github.com/sparkfun/SparkFun_u-blox_GNSS_v4
 *
 * 
 * @file sfe_debug.cpp
 *
 * Definitions for SparkFun_UBLOX_GNSS::SfeDebugPrint - see sfe_debug.h for why this class exists
 * and how DevUBLOXGNSS, ubxMessageVector and nmeaMessageVector each come to inherit it.
 *
 * These seven methods are unchanged from the original DevUBLOXGNSS::debugPrint()/debugPrintln()
 * implementations - only their qualification (and the class they belong to) has moved.
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
