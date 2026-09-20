/*
  sfe_debug.h

  v4 scaffolding: debugPrint()/debugPrintln() extracted from DevUBLOXGNSS into their own small
  base class, SfeDebugPrint, so other classes can inherit real, working debug-print support too -
  not just DevUBLOXGNSS. Added to let ubxMessageVector and nmeaMessageVector print diagnostics
  from deep inside their own methods (e.g. a full callback ring buffer dropping a message) without
  a callback or pointer back into DevUBLOXGNSS.

  ubxMessageVector and nmeaMessageVector are owned BY VALUE as public members of DevUBLOXGNSS
  (ubxMessages, nmeaMessages) - the opposite containment direction from inheritance - and
  DevUBLOXGNSS's own header (u-blox_GNSS.h) includes ubxMessageVector.h/nmeaMessageVector.h, not
  the other way around. So this class lives in its own leaf header with no dependency on
  u-blox_GNSS.h, and DevUBLOXGNSS, ubxMessageVector and nmeaMessageVector each inherit it
  independently.

  Plain (non-static) inheritance means each of the three gets its OWN copy of _debugSerial/
  _printDebug/_printLimitedDebug - so on its own, a ubxMessages.debugPrint(...) call would compile
  but never print anything, because myGNSS.enableDebugging() only ever sets DevUBLOXGNSS's own
  inherited copy of that state, not ubxMessages'/nmeaMessages' separate copies. `static` shared
  state would fix that but wrongly couple debug on/off across every SFE_UBLOX_GNSS instance in a
  sketch. Instead, copyDebugStateFrom() lets DevUBLOXGNSS::enableDebugging()/disableDebugging()
  (see u-blox_GNSS.cpp) explicitly push its own state onto ubxMessages/nmeaMessages right after
  updating it, so all three stay in sync per-instance without any shared/static state.
*/

#pragma once

#include <Arduino.h>
#include "sfe_bus.h"

namespace SparkFun_UBLOX_GNSS
{

  class SfeDebugPrint
  {
  public:
    void debugPrint(const char *message, bool important = false);             // Safely print debug statements
    void debugPrint(uint32_t value, bool important = false);                  // Safely print debug values
    void debugPrint(uint32_t value, int printBase, bool important = false);   // Safely print debug values in a given base (e.g. HEX)
    void debugPrintln(const char *message, bool important = false);           // Safely print debug statements
    void debugPrintln(uint32_t value, bool important = false);                // Safely print debug values
    void debugPrintln(uint32_t value, int printBase, bool important = false); // Safely print debug values in a given base (e.g. HEX)
    void debugPrintln(void);                                                  // Safely print a blank debug line

    // Copies debug state (which port to print to, and whether printing is enabled) from another
    // SfeDebugPrint object. Public - and taking the state rather than pulling it - because the
    // caller is normally DevUBLOXGNSS, reaching in from OUTSIDE to push its own state onto its
    // separately-inherited ubxMessages/nmeaMessages member objects. See the file comment above.
    void copyDebugStateFrom(const SfeDebugPrint &other)
    {
      _debugSerial = other._debugSerial;
      _printDebug = other._printDebug;
      _printLimitedDebug = other._printLimitedDebug;
    }

  protected:
    SfePrint _debugSerial;           // The stream to send debug messages to if enabled
    bool _printDebug = false;        // Flag to print the serial commands we are sending to the Serial port for debug
    bool _printLimitedDebug = false; // Flag to print limited debug messages. Useful for I2C debugging or high navigation rates
  };

} // namespace SparkFun_UBLOX_GNSS
