/*
  An Arduino Library which allows you to communicate seamlessly with u-blox GNSS modules using the Configuration Interface

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html
  https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html
  https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html
  https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html
  https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html

  Original version by Nathan Seidle @ SparkFun Electronics, September 6th, 2018
  v2.0 rework by Paul Clark @ SparkFun Electronics, December 31st, 2020
  v3.0 rework by Paul Clark @ SparkFun Electronics, December 8th, 2022
  v4.0 rework by Claude, directed by Paul Clark @ SparkFun Electronics, September 2026

  https://github.com/sparkfun/SparkFun_u-blox_GNSS_v4

  SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
  The MIT License (MIT)
  Copyright (c) 2018 SparkFun Electronics
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
  associated documentation files (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to
  do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial
  portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include "u-blox_config_keys.h"
#include "u-blox_structs.h"
#include "u-blox_external_typedefs.h"
#include "u-blox_Class_and_ID.h"
#include "sfe_bus.h"
#include "sfe_debug.h" // v4 scaffolding - shared base for debugPrint()/debugPrintln(), see AGENTS.md
#include "ubxMessageVector.h" // v4 scaffolding - see AGENTS.md "Reference Scaffolding"
#include "nmeaMessageVector.h"

// Define a digital pin to aid debugging
// Leave set to -1 if not needed
const int debugPin = -1;

class DevUBLOXGNSS : public SparkFun_UBLOX_GNSS::SfeDebugPrint
{
public:
  DevUBLOXGNSS(void);
  ~DevUBLOXGNSS(void);

  // New in v3.0: hardware interface is abstracted
  bool isConnected(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

protected:
  enum commTypes
  {
    COMM_TYPE_I2C = 0,
    COMM_TYPE_SERIAL,
    COMM_TYPE_SPI
  } _commType = COMM_TYPE_I2C; // Controls which port we look to for incoming bytes
  bool init(uint16_t maxWait, bool assumeSuccess);
  void setCommunicationBus(SparkFun_UBLOX_GNSS::GNSSDeviceBus &theBus);
  // For I2C, ping the _address
  // Not Applicable for SPI and Serial
  bool ping();
  // For Serial, return Serial.available()
  // For I2C, read registers 0xFD and 0xFE. Return bytes available as uint16_t
  // Not Applicable for SPI
  uint16_t available();
  // For Serial, do Serial.write
  // For I2C, push data to register 0xFF. Chunkify if necessary. Prevent single byte writes as these are illegal
  // For SPI, writing bytes will also read bytes simultaneously. Read data is _ignored_ here. Use writeReadBytes
  uint8_t writeBytes(uint8_t *data, uint8_t length);
  // For SPI, writing bytes will also read bytes simultaneously. Read data is returned in readData
  uint8_t writeReadBytes(const uint8_t *data, uint8_t *readData, uint8_t length);
  void startWriteReadByte();
  void writeReadByte(const uint8_t *data, uint8_t *readData);
  void writeReadByte(const uint8_t data, uint8_t *readData);
  void endWriteReadByte();
  // For Serial, attempt Serial.read
  // For I2C, read from register 0xFF
  // For SPI, read the byte while writing 0xFF
  uint8_t readBytes(uint8_t *data, uint8_t length);
  // Flag to indicate if we are connected to UART1 or UART2
  // Needed to select the correct config items when enabling a periodic message
  bool _UART2 = false; // Default to UART1

  // These lock / unlock functions can be used if you have multiple tasks writing to the bus.
  // The idea is that in a RTOS you override this class and the functions in which you take and give a mutex.
  virtual bool createLock(void) { return true; }
  virtual bool lock(void) { return true; }
  virtual void unlock(void) {}
  virtual void deleteLock(void) {}

public:
  void connectedToUART2(bool connected = true) { _UART2 = connected; }

  // Depending on the sentence type the processor will load characters into different arrays
  enum sfe_ublox_sentence_types_e
  {
    SFE_UBLOX_SENTENCE_TYPE_NONE = 0,
    SFE_UBLOX_SENTENCE_TYPE_NMEA,
    SFE_UBLOX_SENTENCE_TYPE_UBX,
    SFE_UBLOX_SENTENCE_TYPE_RTCM
  } currentSentence = SFE_UBLOX_SENTENCE_TYPE_NONE;

  // New in v2.0: allow the payload size for packetCfg to be changed
  bool setPacketCfgPayloadSize(size_t payloadSize); // Set packetCfgPayloadSize
  size_t getPacketCfgSpaceRemaining();              // Returns the number of free bytes remaining in packetCfgPayload

  void end(void); // Stop all automatic message processing. Free all used RAM

  void setI2CpollingWait(uint8_t newPollingWait_ms); // Allow the user to change the I2C polling wait if required
  void setSPIpollingWait(uint8_t newPollingWait_ms); // Allow the user to change the SPI polling wait if required

  // Set the max number of bytes set in a given I2C transaction
  uint8_t i2cTransactionSize = 32; // Default to ATmega328 limit

  // Control the size of the internal I2C transaction amount
  void setI2CTransactionSize(uint8_t transactionSize);
  uint8_t getI2CTransactionSize(void);

  // Control the size of the internal I2C transaction amount
  void setSpiTransactionSize(uint8_t transactionSize);
  uint8_t getSpiTransactionSize(void);

  // Control the size of the SPI transfer buffer. If the buffer isn't big enough, we'll start to lose bytes
  void setSpiBufferSize(size_t bufferSize);
  size_t getSpiBufferSize(void);

  // A dedicated buffer for RTCM data - separate to the logging buffer
  // RTCM data can be stored here and then extracted - avoiding processRTCM
  // This is useful on SPI systems, avoiding bus collisions between checkUblox/processRTCM
  // and pushing the RTCM data to (e.g.) Ethernet
  void setRTCMBufferSize(uint16_t bufferSize);                             // Set the size of the RTCM buffer. This must be called _before_ .begin.
  uint16_t getRTCMBufferSize(void);                                        // Return the size of the RTCM buffer
  uint16_t extractRTCMBufferData(uint8_t *destination, uint16_t numBytes); // Extract numBytes of data from the RTCM buffer. Copy it to destination. It is the user's responsibility to ensure destination is large enough.
  uint16_t rtcmBufferAvailable(void);                                      // Returns the number of bytes available in the RTCM buffer which are waiting to be read
  void clearRTCMBuffer(void);                                              // Empty the RTCM buffer - discard all contents

  // Control the size of maxNMEAByteCount
  void setMaxNMEAByteCount(int8_t newMax);
  int8_t getMaxNMEAByteCount(void);

// Enable debug messages using the chosen Serial port (Stream)
// Boards like the RedBoard Turbo use SerialUSB (not Serial).
// But other boards like the SAMD51 Thing Plus use Serial (not SerialUSB).
// These lines let the code compile cleanly on as many SAMD boards as possible.
#if defined(ARDUINO_ARCH_SAMD)                                                        // Is this a SAMD board?
#if defined(USB_VID)                                                                  // Is the USB Vendor ID defined?
#if (USB_VID == 0x1B4F)                                                               // Is this a SparkFun board?
#if !defined(ARDUINO_SAMD51_THING_PLUS) & !defined(ARDUINO_SAMD51_MICROMOD)           // If it is not a SAMD51 Thing Plus or SAMD51 MicroMod
  void enableDebugging(Print &debugPort = SerialUSB, bool printLimitedDebug = false); // Given a port to print to, enable debug messages. Default to all, not important-only.
#else
  void enableDebugging(Print &debugPort = Serial, bool printLimitedDebug = false); // Given a port to print to, enable debug messages. Default to all, not important-only.
#endif
#else
  void enableDebugging(Print &debugPort = Serial, bool printLimitedDebug = false); // Given a port to print to, enable debug messages. Default to all, not important-only.
#endif
#else
  void enableDebugging(Print &debugPort = Serial, bool printLimitedDebug = false); // Given a port to print to, enable debug messages. Default to all, not important-only.
#endif
#else
  void enableDebugging(Print &debugPort = Serial, bool printLimitedDebug = false); // Given a port to print to, enable debug messages. Default to all, not important-only.
#endif

  void disableDebugging(void);                                    // Turn off debug statements
  // debugPrint()/debugPrintln() are inherited from SfeDebugPrint (see sfe_debug.h) - also
  // inherited by ubxMessageVector/nmeaMessageVector, so diagnostics deep inside those classes'
  // own methods can use them too, kept in sync by enableDebugging()/disableDebugging() below.
  const char *statusString(sfe_ublox_status_e stat);              // Pretty print the return value

  // Check for the arrival of new I2C/Serial data
  // Changed in V1.8.1: provides backward compatibility for the examples that call checkUblox directly
  // Will default to using packetCfg to look for explicit autoPVT packets so they get processed correctly by processUBX
  bool checkUblox(uint8_t requestedClass = 0, uint8_t requestedID = 0); // Checks module with user selected commType

  bool checkUbloxI2C(ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID);    // Method for I2C polling of data, passing any new bytes to process()
  bool checkUbloxSerial(ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID); // Method for serial polling of data, passing any new bytes to process()
  bool checkUbloxSpi(ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID);    // Method for spi polling of data, passing any new bytes to process()
  bool processSpiBuffer(ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID); // Called by checkUbloxSpi to process any backlog data in the spiBuffer

  // Process the incoming data

  void process(uint8_t incoming, ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID);             // Processes NMEA and UBX binary sentences one byte at a time
  void processNMEA(char incoming) __attribute__((weak));                                                           // Given a NMEA character, do something with it. User can overwrite if desired to use something like tinyGPS or MicroNMEA libraries
  sfe_ublox_sentence_types_e processRTCMframe(uint8_t incoming, uint16_t *rtcmFrameCounter) __attribute__((weak)); // Monitor the incoming bytes for start and length bytes
  void processRTCM(uint8_t incoming) __attribute__((weak));                                                        // Given rtcm byte, do something with it. User can overwrite if desired to pipe bytes to radio, internet, etc.
  void processUBX(uint8_t incoming, ubxPacket *incomingUBX, uint8_t requestedClass, uint8_t requestedID);          // Given a character, file it away into the uxb packet structure
  void processUBXpacket(ubxPacket *msg);                                                                           // Once a packet has been received and validated, identify this packet's class/id and update internal flags
  virtual void processLoggedUBX(ubxPacket *incomingUBX) {}                                                         // Process any UBX message with enableUBXlogging processMe set true

  // Send I2C/Serial/SPI commands to the module

  void calcChecksum(ubxPacket *msg);                                                                                               // Sets the checksumA and checksumB of a given messages
  sfe_ublox_status_e sendCommand(ubxPacket *outgoingUBX, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait, bool expectACKonly = false); // Given a packet and payload, send everything including CRC bytes, return true if we got a response
  sfe_ublox_status_e sendI2cCommand(ubxPacket *outgoingUBX);
  void sendSerialCommand(ubxPacket *outgoingUBX);
  sfe_ublox_status_e sendSpiCommand(ubxPacket *outgoingUBX);
  void spiTransfer(const uint8_t byteToTransfer);

  sfe_ublox_status_e pollNMEA(const char *msgId, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // The equivalent of sendCommand but for NMEA. Poll a single NMEA message using the GN Talker ID

  void printPacket(ubxPacket *packet, bool alwaysPrintPayload = false); // Useful for debugging

  // After sending a message to the module, wait for the expected response (data+ACK or just data)

  sfe_ublox_status_e waitForACKResponse(ubxPacket *outgoingUBX, uint8_t requestedClass, uint8_t requestedID, uint16_t maxTime = kUBLOXGNSSDefaultMaxWait);   // Poll the module until a config packet and an ACK is received, or just an ACK
  sfe_ublox_status_e waitForNoACKResponse(ubxPacket *outgoingUBX, uint8_t requestedClass, uint8_t requestedID, uint16_t maxTime = kUBLOXGNSSDefaultMaxWait); // Poll the module until a config packet is received

  // Check if any callbacks need to be called
  void checkCallbacks(void);

  // Push (e.g.) RTCM or Assist Now data directly to the module
  // Warning: this function does not check that the data is valid. It is the user's responsibility to ensure the data is valid before pushing.
  //
  // For SPI: callProcessBuffer defaults to true and forces pushRawData to call processSpiBuffer in between push transactions.
  // This is to try and prevent incoming data being 'lost' during large (bi-directional) pushes.
  // If you are only pushing limited amounts of data and/or will be calling checkUblox manually, it might be advantageous to set callProcessBuffer to false.
  //
  // Likewise for Serial: callProcessBuffer defaults to true and forces pushRawData to call checkUbloxSerial in between pushing data.
  // This is to try and prevent incoming data being 'lost' (overflowing the serial RX buffer) during a large push.
  // If you are only pushing limited amounts of data and/or will be calling checkUblox manually, it might be advantageous to set callProcessBuffer to false.
  bool pushRawData(uint8_t *dataBytes, size_t numDataBytes, bool callProcessBuffer = true);
  // RTCM parsing - used inside pushRawData
protected:
  void parseRTCM1005(uint8_t *dataBytes, size_t numDataBytes);
  void parseRTCM1006(uint8_t *dataBytes, size_t numDataBytes);

public:

// Push MGA AssistNow data to the module.
// Check for UBX-MGA-ACK responses if required (if mgaAck is YES or ENQUIRE).
// Wait for maxWait millis after sending each packet (if mgaAck is NO).
// Return how many bytes were pushed successfully.
// If skipTime is true, any UBX-MGA-INI-TIME_UTC or UBX-MGA-INI-TIME_GNSS packets found in the data will be skipped,
// allowing the user to override with their own time data with setUTCTimeAssistance.
// offset allows a sub-set of the data to be sent - starting from offset.
#define defaultMGAdelay 7 // Default to waiting for 7ms between each MGA message
  size_t pushAssistNowData(const String &dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);
  size_t pushAssistNowData(const uint8_t *dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);
  size_t pushAssistNowData(bool skipTime, const String &dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);
  size_t pushAssistNowData(bool skipTime, const uint8_t *dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);
  size_t pushAssistNowData(size_t offset, bool skipTime, const String &dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);
  size_t pushAssistNowData(size_t offset, bool skipTime, const uint8_t *dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);

// Provide initial time assistance
#define defaultMGAINITIMEtAccS 2  // Default to setting the seconds time accuracy to 2 seconds
#define defaultMGAINITIMEtAccNs 0 // Default to setting the nanoseconds time accuracy to zero
#define defaultMGAINITIMEsource 0 // Set default source to none, i.e. on receipt of message (will be inaccurate!)
  bool setUTCTimeAssistance(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanos = 0,
                            uint16_t tAccS = defaultMGAINITIMEtAccS, uint32_t tAccNs = defaultMGAINITIMEtAccNs, uint8_t source = defaultMGAINITIMEsource,
                            sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);

  // Provide initial position assistance
  // The units for ecefX/Y/Z and posAcc (stddev) are cm.
  bool setPositionAssistanceXYZ(int32_t ecefX, int32_t ecefY, int32_t ecefZ, uint32_t posAcc, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);
  // The units for lat and lon are degrees * 1e-7 (WGS84)
  // The units for alt (WGS84) and posAcc (stddev) are cm.
  bool setPositionAssistanceLLH(int32_t lat, int32_t lon, int32_t alt, uint32_t posAcc, sfe_ublox_mga_assist_ack_e mgaAck = SFE_UBLOX_MGA_ASSIST_ACK_NO, uint16_t maxWait = defaultMGAdelay);

  // Find the start of the AssistNow Offline (UBX_MGA_ANO) data for the chosen day
  // The daysIntoFture parameter makes it easy to get the data for (e.g.) tomorrow based on today's date
  // Returns numDataBytes if unsuccessful
  // TO DO: enhance this so it will find the nearest data for the chosen day - instead of an exact match
  size_t findMGAANOForDate(const String &dataBytes, size_t numDataBytes, uint16_t year, uint8_t month, uint8_t day, uint8_t daysIntoFuture = 0);
  size_t findMGAANOForDate(const uint8_t *dataBytes, size_t numDataBytes, uint16_t year, uint8_t month, uint8_t day, uint8_t daysIntoFuture = 0);

// Read the whole navigation data base. The receiver will send all available data from its internal database.
// Data is written to dataBytes. Set maxNumDataBytes to the (maximum) size of dataBytes.
// If the database exceeds maxNumDataBytes, the excess bytes will be lost.
// The function returns the number of database bytes written to dataBytes.
// The return value will be equal to maxNumDataBytes if excess data was received.
// The function will timeout after maxWait milliseconds - in case the final UBX-MGA-ACK was missed.
#define defaultNavDBDMaxWait 3100
  size_t readNavigationDatabase(uint8_t *dataBytes, size_t maxNumDataBytes, uint16_t maxWait = defaultNavDBDMaxWait);

  // Support for data logging
  void setFileBufferSize(uint16_t bufferSize);                             // Set the size of the file buffer. This must be called _before_ .begin.
  uint16_t getFileBufferSize(void);                                        // Return the size of the file buffer
  uint16_t extractFileBufferData(uint8_t *destination, uint16_t numBytes); // Extract numBytes of data from the file buffer. Copy it to destination. It is the user's responsibility to ensure destination is large enough.
  uint16_t fileBufferAvailable(void);                                      // Returns the number of bytes available in file buffer which are waiting to be read
  uint16_t getMaxFileBufferAvail(void);                                    // Returns the maximum number of bytes which the file buffer has contained. Handy for checking the buffer is large enough to handle all the incoming data.
  void clearFileBuffer(void);                                              // Empty the file buffer - discard all contents
  void clearMaxFileBufferAvail(void);                                      // Reset fileBufferMaxAvail

  // Specific commands

  // Port configurations
  bool setI2CAddress(uint8_t deviceAddress, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxTime = kUBLOXGNSSDefaultMaxWait);                                // Changes the I2C address of the u-blox module
  bool setSerialRate(uint32_t baudrate, uint8_t uartPort = COM_PORT_UART1, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxTime = kUBLOXGNSSDefaultMaxWait); // Changes the serial baud rate of the u-blox module, uartPort should be COM_PORT_UART1/2

  bool setI2COutput(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure I2C port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setUART1Output(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Configure UART1 port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setUART2Output(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Configure UART2 port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setUSBOutput(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure USB port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setSPIOutput(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure SPI port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof

  bool setI2CInput(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure I2C port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setUART1Input(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Configure UART1 port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setUART2Input(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Configure UART2 port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setUSBInput(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure USB port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof
  bool setSPIInput(uint8_t comSettings, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure SPI port to output UBX, NMEA, RTCM3, SPARTN or a combination thereof

  void setNMEAOutputPort(Print &outputPort); // Sets the internal variable for the port to direct only NMEA characters to
  void setRTCMOutputPort(Print &outputPort); // Sets the internal variable for the port to direct only RTCM characters to
  void setUBXOutputPort(Print &outputPort);  // Sets the internal variable for the port to direct only UBX characters to
  void setOutputPort(Print &outputPort);     // Sets the internal variable for the port to direct ALL characters to

  // Reset to defaults

  void factoryReset();                                              // Send factory reset sequence (i.e. load "default" configuration and perform hardReset)
  bool factoryDefault(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Reset module to factory defaults
  void hardReset();                                                 // Perform a reset leading to a cold start (zero info start-up)
  void softwareResetGNSSOnly();                                     // Controlled Software Reset (GNSS only) only restarts the GNSS tasks, without reinitializing the full system or reloading any stored configuration.
  void softwareEnableGNSS(bool enable);                             // Controlled Software Start / Stop (GNSS only)
  void cfgRst(uint8_t *data, uint8_t len);                          // Common method for CFG RST

  // Save configuration to BBR / Flash

  bool saveConfiguration(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                        // Save current configuration to flash and BBR (battery backed RAM)
  bool saveConfigSelective(uint32_t configMask, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Save the selected configuration sub-sections to flash and BBR (battery backed RAM)
  bool cfgCfg(uint8_t *data, uint8_t len, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);       // Common method for CFG CFG

  // Functions used for RTK and base station setup
  bool setSurveyMode(uint8_t mode, uint16_t observationTime, float requiredAccuracy, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // Control survey in mode
  bool setSurveyModeFull(uint8_t mode, uint32_t observationTime, float requiredAccuracy, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Control survey in mode
  bool enableSurveyMode(uint16_t observationTime, float requiredAccuracy, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                // Begin Survey-In for NEO-M8P / ZED-F9x
  bool enableSurveyModeFull(uint32_t observationTime, float requiredAccuracy, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);            // Begin Survey-In for NEO-M8P / ZED-F9x
  bool disableSurveyMode(uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                 // Stop Survey-In mode
  // Given coordinates, put receiver into static position. Set latlong to true to pass in lat/long values instead of ecef.
  // For ECEF the units are: cm, 0.1mm, cm, 0.1mm, cm, 0.1mm
  // For Lat/Lon/Alt the units are: degrees^-7, degrees^-9, degrees^-7, degrees^-9, cm, 0.1mm
  bool setStaticPosition(int32_t ecefXOrLat, int8_t ecefXOrLatHP, int32_t ecefYOrLon, int8_t ecefYOrLonHP, int32_t ecefZOrAlt, int8_t ecefZOrAltHP, bool latLong, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setStaticPosition(int32_t ecefXOrLat, int32_t ecefYOrLon, int32_t ecefZOrAlt, bool latLong, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setDGNSSConfiguration(sfe_ublox_dgnss_mode_e dgnssMode = SFE_UBLOX_DGNSS_MODE_FIXED, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Set the DGNSS differential mode

  // Read the module's protocol version
  // For safety, call getProtocolVersion etc. inside an if(getModuleInfo())
  uint8_t getProtocolVersionHigh(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Returns the PROTVER XX.00 from UBX-MON-VER register
  uint8_t getProtocolVersionLow(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);  // Returns the PROTVER 00.XX from UBX-MON-VER register
  uint8_t getFirmwareVersionHigh(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Returns the FWVER XX.00 from UBX-MON-VER register
  uint8_t getFirmwareVersionLow(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);  // Returns the FWVER 00.XX from UBX-MON-VER register
  const char *getFirmwareType(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);    // Returns the firmware type (SPG, HPG, ADR, etc.) from UBX-MON-VER register
  const char *getModuleName(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Returns the module name (ZED-F9P, ZED-F9R, etc.) from UBX-MON-VER register
  bool getProtocolVersion(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);        // Deprecated. Use getModuleInfo.
  bool getModuleInfo(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);             // Queries module, extracts info. Returns true if MON-VER was read successfully
protected:
  bool prepareModuleInfo(uint16_t maxWait);

public:
  moduleSWVersion_t *moduleSWVersion = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary

  // Support for geofences
  bool addGeofence(int32_t latitude, int32_t longitude, uint32_t radius, uint8_t confidence = 0, bool pinPolarity = 0, uint8_t pin = 0, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Add a new geofence
  bool clearGeofences(uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                                                                   // Clears all geofences
  bool getGeofenceState(geofenceState &currentGeofenceState, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                                                               // Returns the combined geofence state
  // Storage for the geofence parameters. RAM is allocated for this if/when required.
  geofenceParams_t *currentGeofenceParams = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary

  // Power save / off
  bool powerOff(uint32_t durationInMs, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool powerOffWithInterrupt(uint32_t durationInMs, uint32_t wakeupSources = VAL_RXM_PMREQ_WAKEUPSOURCE_EXTINT0, bool forceWhileUsb = true, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  // Change the dynamic platform model using UBX-CFG-NAV5
  bool setDynamicModel(dynModel newDynamicModel = DYN_MODEL_PORTABLE, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  uint8_t getDynamicModel(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the dynamic model - returns 255 if the sendCommand fails

  // Reset / enable / configure the odometer
  bool resetOdometer(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                                                                                                          // Reset the odometer
  bool enableOdometer(bool enable = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                                                  // Enable / disable the odometer
  bool getOdometerConfig(uint8_t *flags, uint8_t *odoCfg, uint8_t *cogMaxSpeed, uint8_t *cogMaxPosAcc, uint8_t *velLpGain, uint8_t *cogLpGain, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Read the odometer configuration
  bool setOdometerConfig(uint8_t flags, uint8_t odoCfg, uint8_t cogMaxSpeed, uint8_t cogMaxPosAcc, uint8_t velLpGain, uint8_t cogLpGain, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Configure the odometer

  // Enable/Disable individual GNSS systems using UBX-CFG-GNSS
  // Note: you must leave at least one major GNSS enabled! If in doubt, enable GPS before disabling the others
  bool enableGNSS(bool enable, sfe_ublox_gnss_ids_e id, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool isGNSSenabled(sfe_ublox_gnss_ids_e id, bool *enabled, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool isGNSSenabled(sfe_ublox_gnss_ids_e id, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Unsafe overload
  uint32_t getEnableGNSSConfigKey(sfe_ublox_gnss_ids_e id);

  // Reset ESF automatic IMU-mount alignment
  bool resetIMUalignment(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  // Enable/disable esfAutoAlignment
  bool getESFAutoAlignment(bool *enabled, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool getESFAutoAlignment(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Unsafe overload
  bool setESFAutoAlignment(bool enable, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  // RF Information (including jamming) - ZED-F9 only
  bool getRFinformation(UBX_MON_RF_data_t *data = nullptr, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the RF information using UBX_MON_RF

  // Extended hardware status
  bool getHW2status(UBX_MON_HW2_data_t *data = nullptr, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the extended hardware status using UBX_MON_HW2

  // UBX-CFG-NAVX5 - get/set the ackAiding byte. If ackAiding is 1, UBX-MGA-ACK messages will be sent by the module to acknowledge the MGA data
  uint8_t getAckAiding(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                     // Get the ackAiding byte - returns 255 if the sendCommand fails
  bool setAckAiding(uint8_t ackAiding, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Set the ackAiding byte

  // AssistNow Autonomous support
  // UBX-CFG-NAVX5 - get/set the aopCfg byte and set the aopOrdMaxErr word. If aopOrbMaxErr is 0 (default), the max orbit error is reset to the firmware default.
  uint8_t getAopCfg(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                             // Get the AssistNow Autonomous configuration (aopCfg) - returns 255 if the sendCommand fails
  bool setAopCfg(uint8_t aopCfg, uint16_t aopOrbMaxErr = 0, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Set the aopCfg byte and the aopOrdMaxErr word

  // SPARTN dynamic keys
  //"When the receiver boots, the host should send 'current' and 'next' keys in one message." - Use setDynamicSPARTNKeys for this.
  //"Every time the 'current' key is expired, 'next' takes its place."
  //"Therefore the host should then retrieve the new 'next' key and send only that." - Use setDynamicSPARTNKey for this.
  // The key can be provided in binary (uint8_t) format or in ASCII Hex (char) format, but in both cases keyLengthBytes _must_ represent the binary key length in bytes.
  bool setDynamicSPARTNKey(uint8_t keyLengthBytes, uint16_t validFromWno, uint32_t validFromTow, const char *key);
  bool setDynamicSPARTNKey(uint8_t keyLengthBytes, uint16_t validFromWno, uint32_t validFromTow, const uint8_t *key);
  bool setDynamicSPARTNKeys(uint8_t keyLengthBytes1, uint16_t validFromWno1, uint32_t validFromTow1, const char *key1,
                            uint8_t keyLengthBytes2, uint16_t validFromWno2, uint32_t validFromTow2, const char *key2);
  bool setDynamicSPARTNKeys(uint8_t keyLengthBytes1, uint16_t validFromWno1, uint32_t validFromTow1, const uint8_t *key1,
                            uint8_t keyLengthBytes2, uint16_t validFromWno2, uint32_t validFromTow2, const uint8_t *key2);

  // Support for SPARTN parsing
  uint8_t uSpartnCrc4(const uint8_t *pU8Msg, size_t size);
  uint8_t uSpartnCrc8(const uint8_t *pU8Msg, size_t size);
  uint16_t uSpartnCrc16(const uint8_t *pU8Msg, size_t size);
  uint32_t uSpartnCrc24(const uint8_t *pU8Msg, size_t size);
  uint32_t uSpartnCrc32(const uint8_t *pU8Msg, size_t size);
  uint8_t * parseSPARTN(uint8_t incoming, bool &valid, uint16_t &len, sfe_ublox_spartn_header_t *header = nullptr);

  // Get unique chip ID - UBX-SEC-UNIQID
  bool getUniqueChipId(UBX_SEC_UNIQID_data_t *data = nullptr, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the unique chip ID using UBX_SEC_UNIQID
  const char *getUniqueChipIdStr(UBX_SEC_UNIQID_data_t *data = nullptr, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the unique chip ID using UBX_SEC_UNIQID

  // General configuration (used only on protocol v27 and higher - ie, ZED-F9P)

  // VALGET

protected:                                                         // These use packetCfg - which is protected from the user
  bool newCfgValget(uint8_t layer = VAL_LAYER_RAM);                // Create a new, empty UBX-CFG-VALGET. Add entries with addCfgValget
  bool addCfgValget(uint32_t key);                                 // Add a new key to an existing UBX-CFG-VALGET ubxPacket - deduce the value size automatically
  bool sendCfgValget(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Send the CfgValget (UBX-CFG-VALGET) construct

public:
  bool newCfgValget(ubxPacket *pkt, uint16_t maxPayload, uint8_t layer = VAL_LAYER_RAM); // Create a new, empty UBX-CFG-VALGET. Add entries with addCfgValget8/16/32/64
  bool addCfgValget(ubxPacket *pkt, uint32_t key);                                       // Add a new key to an existing UBX-CFG-VALGET ubxPacket - deduce the value size automatically
  bool sendCfgValget(ubxPacket *pkt, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);       // Send the CfgValget (UBX-CFG-VALGET) construct
  uint8_t getNumGetCfgKeys() { return _numGetCfgKeys; }                                  // Return the number of keys in the VALGET packet
  uint16_t getLenCfgValGetResponse() { return _lenCfgValGetResponse; }                   // Return the expected length of the VALGET response
  uint8_t getCfgValueSizeBytes(const uint32_t key);                                      // Returns the value size in bytes for the given key

  template <typename T>
  bool extractConfigValueByKey(ubxPacket *pkt, const uint32_t key, T value, size_t maxWidth) // Extract the config value by its key. maxWidth prevents writing beyond the end of value
  {
    if (cfgValgetValueSizes == nullptr) // Check the size list exists
      return false;
    uint8_t sizePtr = 0;

    if (pkt->len < 4)
      return false;

    uint32_t k1 = key & ~UBX_CFG_SIZE_MASK; // Convert key back into an actual key

    uint16_t ptr = 4;
    while (ptr < pkt->len)
    {
      uint32_t k2 = extractLong(pkt, ptr);
      if (k1 == k2)
      {
        ptr += 4; // Point to the value
        switch (key & UBX_CFG_SIZE_MASK)
        {
        case UBX_CFG_L:
          if (maxWidth < sizeof(bool))
            return false;
          *value = (bool)extractByte(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_U1:
        case UBX_CFG_E1:
        case UBX_CFG_X1:
          if (maxWidth < sizeof(uint8_t))
            return false;
          *value = (uint8_t)extractByte(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_I1:
          if (maxWidth < sizeof(int8_t))
            return false;
          *value = (int8_t)extractSignedChar(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_U2:
        case UBX_CFG_E2:
        case UBX_CFG_X2:
          if (maxWidth < sizeof(uint16_t))
            return false;
          *value = (uint16_t)extractInt(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_I2:
          if (maxWidth < sizeof(int16_t))
            return false;
          *value = (int16_t)extractSignedInt(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_U4:
        case UBX_CFG_E4:
        case UBX_CFG_X4:
          if (maxWidth < sizeof(uint32_t))
            return false;
          *value = (uint32_t)extractLong(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_I4:
          if (maxWidth < sizeof(int32_t))
            return false;
          *value = (int32_t)extractSignedLong(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_R4:
          if (maxWidth < sizeof(float))
            return false;
          *value = (float)extractFloat(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_U8:
        case UBX_CFG_X8:
          if (maxWidth < sizeof(uint64_t))
            return false;
          *value = (uint64_t)extractLongLong(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_I8:
          if (maxWidth < sizeof(int64_t))
            return false;
          *value = (int64_t)extractSignedLongLong(pkt, ptr);
          return (true);
          break;
        case UBX_CFG_R8:
          if (maxWidth < sizeof(double))
            return false;
          *value = (double)extractDouble(pkt, ptr);
          return (true);
          break;
        default:
          return false;
          break;
        }
      }
      ptr += 4; // Update ptr
      ptr += cfgValgetValueSizes[sizePtr++];
    }
    return false;
  }

protected:
  sfe_ublox_status_e getVal(uint32_t key, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Load payload with response
public:
  bool getVal8(uint32_t key, uint8_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);        // Returns the value at a given key location
  uint8_t getVal8(uint32_t key, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                   // Unsafe overload - for backward compatibility only
  bool getVal16(uint32_t key, uint16_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Returns the value at a given key location
  uint16_t getVal16(uint32_t key, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                 // Unsafe overload - for backward compatibility only
  bool getVal32(uint32_t key, uint32_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Returns the value at a given key location
  uint32_t getVal32(uint32_t key, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                 // Unsafe overload - for backward compatibility only
  bool getVal64(uint32_t key, uint64_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Returns the value at a given key location
  uint64_t getVal64(uint32_t key, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                 // Unsafe overload - for backward compatibility only
  bool getValSigned8(uint32_t key, int8_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Returns the value at a given key location
  bool getValSigned16(uint32_t key, int16_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Returns the value at a given key location
  bool getValSigned32(uint32_t key, int32_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Returns the value at a given key location
  bool getValSigned64(uint32_t key, int64_t *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Returns the value at a given key location
  bool getValFloat(uint32_t key, float *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Returns the value at a given key location
  bool getValDouble(uint32_t key, double *val, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);    // Returns the value at a given key location

  // VALSET

  bool setValN(uint32_t key, uint8_t *value, uint8_t N, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Sets the N-byte value at a given group/id/size location
  bool setVal8(uint32_t key, uint8_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);             // Sets the 8-bit value at a given group/id/size location
  bool setVal16(uint32_t key, uint16_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);           // Sets the 16-bit value at a given group/id/size location
  bool setVal32(uint32_t key, uint32_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);           // Sets the 32-bit value at a given group/id/size location
  bool setVal64(uint32_t key, uint64_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);           // Sets the 64-bit value at a given group/id/size location
  bool setValSigned8(uint32_t key, int8_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);        // Sets the 8-bit value at a given group/id/size location
  bool setValSigned16(uint32_t key, int16_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Sets the 16-bit value at a given group/id/size location
  bool setValSigned32(uint32_t key, int32_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Sets the 32-bit value at a given group/id/size location
  bool setValSigned64(uint32_t key, int64_t value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Sets the 64-bit value at a given group/id/size location
  bool setValFloat(uint32_t key, float value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);           // Sets the 32-bit value at a given group/id/size location
  bool setValDouble(uint32_t key, double value, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);         // Sets the 64-bit value at a given group/id/size location

  bool newCfgValset(uint8_t layer = VAL_LAYER_RAM_BBR);                                                         // Create a new, empty UBX-CFG-VALSET. Add entries with addCfgValset8/16/32/64
  bool addCfgValsetN(uint32_t key, uint8_t *value, uint8_t N);                                                  // Add a new key and N-byte value to an existing UBX-CFG-VALSET ubxPacket
  bool sendCfgValset(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                              // Send the CfgValset (UBX-CFG-VALSET) construct
  uint8_t getCfgValsetLen();                                                                                    // Returns the length of the current CfgValset construct as number-of-keys
  size_t getCfgValsetSpaceRemaining();                                                                          // Returns the number of free bytes remaining in packetCfg
  void autoSendCfgValsetAtSpaceRemaining(size_t spaceRemaining) { _autoSendAtSpaceRemaining = spaceRemaining; } // Cause CFG_VALSET packets to be sent automatically when packetCfg has less than this many bytes available

  template <typename T>
  bool addCfgValset(uint32_t key, T value) // Add a new key and value to an existing UBX-CFG-VALSET ubxPacket
  {
    uint8_t val[8];

    uint32_t k1 = key & ~UBX_CFG_SIZE_MASK; // Convert key back into an actual key

    switch (key & UBX_CFG_SIZE_MASK)
    {
    case UBX_CFG_L:
      val[0] = (bool)value;
      return (addCfgValsetN(k1, val, 1));
      break;
    case UBX_CFG_U1:
    case UBX_CFG_E1:
    case UBX_CFG_X1:
      val[0] = (uint8_t)value;
      return (addCfgValsetN(k1, val, 1));
      break;
    case UBX_CFG_I1:
      unsignedSigned8 usVal8;
      usVal8.signed8 = (int8_t)value;
      return (addCfgValsetN(k1, &usVal8.unsigned8, 1));
      break;
    case UBX_CFG_U2:
    case UBX_CFG_E2:
    case UBX_CFG_X2:
      for (uint8_t i = 0; i < 2; i++)
        val[i] = (uint8_t)(((uint16_t)value) >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 2));
      break;
    case UBX_CFG_I2:
      unsignedSigned16 usVal16;
      usVal16.signed16 = (int16_t)value;
      for (uint8_t i = 0; i < 2; i++)
        val[i] = (uint8_t)(usVal16.unsigned16 >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 2));
      break;
    case UBX_CFG_U4:
    case UBX_CFG_E4:
    case UBX_CFG_X4:
      for (uint8_t i = 0; i < 4; i++)
        val[i] = (uint8_t)(((uint32_t)value) >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 4));
      break;
    case UBX_CFG_I4:
      unsignedSigned32 usVal32;
      usVal32.signed32 = (int32_t)value;
      for (uint8_t i = 0; i < 4; i++)
        val[i] = (uint8_t)(usVal32.unsigned32 >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 4));
      break;
    case UBX_CFG_R4:
      unsigned32float us32flt;
      us32flt.flt = (float)value;
      for (uint8_t i = 0; i < 4; i++)
        val[i] = (uint8_t)(us32flt.unsigned32 >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 4));
      break;
    case UBX_CFG_U8:
    case UBX_CFG_X8:
      for (uint8_t i = 0; i < 8; i++)
        val[i] = (uint8_t)(((uint64_t)value) >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 8));
      break;
    case UBX_CFG_I8:
      unsignedSigned64 usVal64;
      usVal64.signed64 = (int64_t)value;
      for (uint8_t i = 0; i < 8; i++)
        val[i] = (uint8_t)(usVal64.unsigned64 >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 8));
      break;
    case UBX_CFG_R8:
      unsigned64double us64dbl;
      us64dbl.dbl = (float)value;
      for (uint8_t i = 0; i < 8; i++)
        val[i] = (uint8_t)(us64dbl.unsigned64 >> (8 * i)); // Value
      return (addCfgValsetN(k1, val, 8));
      break;
    default:
      return false;
      break;
    }
    return false;
  }

  // Convenience wrapper: set a single CFG key/value pair in one call, instead of the usual
  // newCfgValset() / addCfgValset() / sendCfgValset() three-step dance - see AGENTS.md
  // "setCfgValset" and CallbackExample1_NAVHPPOSLLH.ino (`myGNSS.setCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_HPPOSLLH_I2C, 1);`).
  template <typename T>
  bool setCfgValset(uint32_t key, T value, uint8_t layer = VAL_LAYER_RAM_BBR) // Set the single key to the given value using CFG-VALSET
  {
    newCfgValset(layer);
    addCfgValset(key, value);
    return sendCfgValset();
  }

  // get and set functions for all of the "automatic" message processing

  // getUBX will only return data once in each navigation cycle. By default, that is once per second.
  // Therefore we should set kUBLOXGNSSDefaultMaxWait to slightly longer than that.
  // If you change the navigation frequency to (e.g.) 4Hz using setNavigationFrequency(4)
  // then you should use a shorter maxWait. 300msec would be about right: getUBX(300)

  // ***** v4 scaffolding - generic (Class, ID)-keyed message access. See AGENTS.md "Reference Scaffolding" *****
  ubxMessage *getUbxMessagePtr(ubxCallbackDataCommon_t *theData); // Factory: hands back the opaque per-message object a callback's ubxCallbackDataCommon_t* points at
  ubxAnyType getUbxMessageFieldCallback(ubxMessage *theMessage, const char *fieldName); // Factory: extracts a named field from the message a callback just fired for, reading from its _callbackStorage
  ubxAnyType getUbxMessageField(ubxMessage *theMessage, const char *fieldName); // Factory: extracts a named field from the message, reading from its _storage
  ubxAnyType getUbxMessageBlockFieldCallback(ubxMessage *theMessage, uint16_t blockIndex, const char *fieldName); // Factory: extracts a named field from repeated block 'blockIndex' of a variable-length message (e.g. NAV-SAT), reading from its _callbackStorage
  ubxAnyType getUbxMessageBlockField(ubxMessage *theMessage, uint16_t blockIndex, const char *fieldName); // Factory: extracts a named field from repeated block 'blockIndex' of a variable-length message (e.g. NAV-SAT), reading from its _storage
  bool getUBX(const char *Class, const char *ID, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Generic poll-or-check-automatic, by Class/ID
  bool getUBX(uint8_t Class, uint8_t ID, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Generic poll-or-check-automatic, by Class/ID
  bool getUBXfield(const char *Class, const char *ID, const char *field, ubxAnyType *value); // Generic field read, by Class/ID/name
  bool getUBXfield(uint8_t Class, uint8_t ID, const char *field, ubxAnyType *value); // Generic field read, by Class/ID/name

  bool setAutoUBX(const char *Class, const char *ID, bool enabled = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoUBX(uint8_t Class, uint8_t ID, bool enabled = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoUBX(const char *Class, const char *ID, bool enabled, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoUBX(uint8_t Class, uint8_t ID, bool enabled, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoUBXrate(const char *Class, const char *ID, uint8_t rate, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoUBXrate(uint8_t Class, uint8_t ID, uint8_t rate, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool assumeAutoUBX(const char *Class, const char *ID, bool enabled, bool implicitUpdate = true);  // In case no config access to the GPS is possible and UBX is send cyclically already
  bool assumeAutoUBX(uint8_t Class, uint8_t ID, bool enabled, bool implicitUpdate = true);  // In case no config access to the GPS is possible and UBX is send cyclically already
  void flushUBX(const char *Class, const char *ID); // Mark the UBX data as read/stale
  void flushUBX(uint8_t Class, uint8_t ID); // Mark the UBX data as read/stale
  void logUBX(const char *Class, const char *ID, bool enabled = true); // Log data to file buffer
  void logUBX(uint8_t Class, uint8_t ID, bool enabled = true); // Log data to file buffer
  // Generic replacement for the old per-message setAuto<MSG>callbackPtr() functions
  bool setAutoCallbackPtr(const char *classStr, const char *idStr, void (*callbackPointerPtr)(ubxCallbackDataCommon_t *));

  // UBX-NAV-SAT is now a registered v4 message (ubxNAVSAT) - see AGENTS.md "Adding the
  // variable-length UBX messages". setAutoNAVSAT/setAutoNAVSATrate/assumeAutoNAVSAT/
  // flushNAVSAT/logNAVSAT/setAutoNAVSATcallbackPtr are retired; use the generic
  // setAutoUBX/setAutoUBXrate/assumeAutoUBX/flushUBX/logUBX/setAutoCallbackPtr above instead
  // (by Class/ID = UBX_CLASS_NAV/UBX_NAV_SAT, or by name "NAV"/"SAT"). getNAVSAT() remains, as a
  // thin wrapper, since it is called directly rather than by name.
  bool getNAVSAT(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Query module for latest NAVSAT data. If autoNAVSAT is disabled, performs an explicit poll and waits, if enabled does not block. Returns true if new NAVSAT is available.

  // UBX-NAV-SIG is now a registered v4 message (ubxNAVSIG) - see AGENTS.md "Adding the
  // variable-length UBX messages". setAutoNAVSIG/setAutoNAVSIGrate/assumeAutoNAVSIG/
  // flushNAVSIG/logNAVSIG/setAutoNAVSIGcallbackPtr are retired; use the generic
  // setAutoUBX/setAutoUBXrate/assumeAutoUBX/flushUBX/logUBX/setAutoCallbackPtr above instead
  // (by Class/ID = UBX_CLASS_NAV/UBX_NAV_SIG, or by name "NAV"/"SIG"). getNAVSIG() remains, as a
  // thin wrapper, since it is called directly rather than by name.
  bool getNAVSIG(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Query module for latest NAVSIG data. If autoNAVSIG is disabled, performs an explicit poll and waits, if enabled does not block. Returns true if new NAVSIG is available.

  // Receiver Manager Messages (RXM)

  // Configure a callback for the UBX-RXM-PMP messages produced by the NEO-D9S
  // Note: on the NEO-D9S, the UBX-RXM-PMP messages are enabled by default on all ports.
  //       You can disable them by calling (e.g.) setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_I2C, 0)
  //       The NEO-D9S does not support UBX-CFG-MSG
  bool setRXMPMPcallbackPtr(void (*callbackPointerPtr)(UBX_RXM_PMP_data_t *));                // Callback receives a pointer to the data, instead of _all_ the data. Much kinder on the stack!
  bool setRXMPMPmessageCallbackPtr(void (*callbackPointerPtr)(UBX_RXM_PMP_message_data_t *)); // Use this if you want all of the PMP message (including sync chars, checksum, etc.) to push to a GNSS

  // Configure a callback for the UBX-RXM-QZSSL6 messages produced by the NEO-D9C
  // Note: on the NEO-D9C, the UBX-RXM-QZSSL6 messages are enabled by default on all ports.
  //       You can disable them by calling (e.g.) setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_I2C, 0)
  //       The NEO-D9C does not support UBX-CFG-MSG
  bool setRXMQZSSL6messageCallbackPtr(void (*callbackPointerPtr)(UBX_RXM_QZSSL6_message_data_t *)); // Use this if you want all of the QZSSL6 message (including sync chars, checksum, etc.) to push to a GNSS


  // UBX-RXM-SFRBX is now a registered v4 message (ubxRXMSFRBX) - see AGENTS.md "Adding support
  // for RXM-SFRBX". setAutoRXMSFRBX/setAutoRXMSFRBXrate/setAutoRXMSFRBXcallbackPtr/
  // setAutoRXMSFRBXmessageCallbackPtr/assumeAutoRXMSFRBX/flushRXMSFRBX/logRXMSFRBX are retired -
  // see the comment above getRXMSFRBX()'s definition in u-blox_GNSS.cpp. getRXMSFRBX is kept as a
  // thin wrapper for backward compatibility, though it should strictly be deprecated (SFRBX is
  // output-only and cannot be polled - see issue #167).
  bool getRXMSFRBX(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // RXM SFRBX

  // UBX-RXM-RAWX and UBX-RXM-MEASX are now registered v4 messages (ubxRXMRAWX/ubxRXMMEASX) -
  // see AGENTS.md "Adding the variable-length UBX messages". setAutoRXMRAWX/setAutoRXMRAWXrate/
  // assumeAutoRXMRAWX/flushRXMRAWX/logRXMRAWX/setAutoRXMRAWXcallbackPtr and their RXM-MEASX
  // equivalents are retired; use the generic setAutoUBX/setAutoUBXrate/assumeAutoUBX/flushUBX/
  // logUBX/setAutoCallbackPtr above instead (by Class/ID = UBX_CLASS_RXM/UBX_RXM_RAWX or
  // UBX_CLASS_RXM/UBX_RXM_MEASX, or by name "RXM"/"RAWX" or "RXM"/"MEASX"). getRXMRAWX()/
  // getRXMMEASX() remain, as thin wrappers, since they are called directly rather than by name.
  bool getRXMRAWX(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // Query module for latest RXM RAWX data. If autoRXMRAWX is disabled, performs an explicit poll and waits, if enabled does not block. Returns true if new RXM RAWX is available.
  bool getRXMMEASX(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Query module for latest RXM MEASX data. If autoRXMMEASX is disabled, performs an explicit poll and waits, if enabled does not block. Returns true if new RXM MEASX is available.

  // Receiver status (MON)

  // ubxMONCOMMS is now self-registered - see AGENTS.md "Adding the variable-length UBX messages".
  // getMONCOMMS() remains, as a thin wrapper, since it is called directly rather than by name.
  bool getMONCOMMS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // MON COMMS

  // Sensor fusion (dead reckoning) (ESF)

  bool getEsfInfo(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                                                          // ESF STATUS Helper
  bool getESFSTATUS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                                                                        // ESF STATUS
  bool setAutoESFSTATUS(bool enabled, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                   // Enable/disable automatic ESF STATUS reports
  bool setAutoESFSTATUS(bool enabled, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                              // Enable/disable automatic ESF STATUS reports, with implicitUpdate == false accessing stale data will not issue parsing of data in the rxbuffer of your interface, instead you have to call checkUblox when you want to perform an update
  bool setAutoESFSTATUSrate(uint8_t rate, bool implicitUpdate = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                   // Set the rate for automatic STATUS reports
  bool setAutoESFSTATUScallbackPtr(void (*callbackPointerPtr)(UBX_ESF_STATUS_data_t *), uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Enable automatic STATUS reports at the navigation frequency. Data is accessed from the callback.
  bool assumeAutoESFSTATUS(bool enabled, bool implicitUpdate = true);                                                                                                    // In case no config access to the GPS is possible and ESF STATUS is send cyclically already
  void flushESFSTATUS();                                                                                                                                                 // Mark all the data as read/stale
  void logESFSTATUS(bool enabled = true);                                                                                                                                // Log data to file buffer

  bool setAutoESFMEAS(bool enabled, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                 // Enable/disable automatic ESF MEAS reports
  bool setAutoESFMEAS(bool enabled, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                            // Enable/disable automatic ESF MEAS reports, with implicitUpdate == false accessing stale data will not issue parsing of data in the rxbuffer of your interface, instead you have to call checkUblox when you want to perform an update
  bool setAutoESFMEASrate(uint8_t rate, bool implicitUpdate = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                 // Set the rate for automatic MEAS reports
  bool setAutoESFMEAScallbackPtr(void (*callbackPointerPtr)(UBX_ESF_MEAS_data_t *), uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Enable automatic MEAS reports at the navigation frequency. Data is accessed from the callback.
  bool assumeAutoESFMEAS(bool enabled, bool implicitUpdate = true);                                                                                                  // In case no config access to the GPS is possible and ESF MEAS is send cyclically already
  void logESFMEAS(bool enabled = true);                                                                                                                              // Log data to file buffer

  bool setAutoESFRAW(bool enabled, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                                                // Enable/disable automatic ESF RAW reports
  bool setAutoESFRAW(bool enabled, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                           // Enable/disable automatic ESF RAW reports, with implicitUpdate == false accessing stale data will not issue parsing of data in the rxbuffer of your interface, instead you have to call checkUblox when you want to perform an update
  bool setAutoESFRAWrate(uint8_t rate, bool implicitUpdate = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                // Set the rate for automatic RAW reports
  bool setAutoESFRAWcallbackPtr(void (*callbackPointerPtr)(UBX_ESF_RAW_data_t *), uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Enable automatic RAW reports at the navigation frequency. Data is accessed from the callback.
  bool assumeAutoESFRAW(bool enabled, bool implicitUpdate = true);                                                                                                 // In case no config access to the GPS is possible and ESF RAW is send cyclically already
  void logESFRAW(bool enabled = true);                                                                                                                             // Log data to file buffer

  // ubxSECSIG (Version 2 only) is now self-registered - see AGENTS.md "Adding the variable-length
  // UBX messages". getSECSIG() remains, as a thin wrapper, since it is called directly rather
  // than by name. The UBX_SEC_SIG_data_t* overload is redacted.
  bool getSECSIG(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Query module for latest data

// Helper functions for CFG RATE

  bool setNavigationFrequency(uint8_t navFreq, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Set the number of nav solutions sent per second
  bool getNavigationFrequency(uint8_t *navFreq, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);    // Get the number of nav solutions sent per second currently being output by module
  uint8_t getNavigationFrequency(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                   // Unsafe overload
  bool setMeasurementRate(uint16_t rate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);       // Set the elapsed time between GNSS measurements in milliseconds, which defines the rate
  bool getMeasurementRate(uint16_t *measRate, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);      // Return the elapsed time between GNSS measurements in milliseconds
  uint16_t getMeasurementRate(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                      // Unsafe overload
  bool setNavigationRate(uint16_t rate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);        // Set the ratio between the number of measurements and the number of navigation solutions. Unit is cycles. Max is 127
  bool getNavigationRate(uint16_t *navRate, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);        // Return the ratio between the number of measurements and the number of navigation solutions. Unit is cycles
  uint16_t getNavigationRate(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                       // Unsafe overload

  // Helper functions for DOP
  // For safety, call these inside an if(getNAVDOP()) or if(getUBX("NAV","DOP"))

  bool getNAVDOP(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint16_t getGeometricDOP();
  uint16_t getPositionDOP();
  uint16_t getTimeDOP();
  uint16_t getVerticalDOP();
  uint16_t getHorizontalDOP();
  uint16_t getNorthingDOP();
  uint16_t getEastingDOP();

  // Helper functions for ATT
  // For safety, call these inside an if(getNAVATT()) or if(getUBX("NAV","ATT"))

  bool getNAVATT(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  float getATTroll();    // Returned as degrees
  float getATTpitch();   // Returned as degrees
  float getATTheading(); // Returned as degrees

  // Helper functions for PVT
  // For safety, call these inside an if (getNAVPVT()) or if(getUBX("NAV","PVT"))

  bool getNAVPVT(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint32_t getTimeOfWeek();
  uint16_t getYear();
  uint8_t getMonth();
  uint8_t getDay();
  uint8_t getHour();
  uint8_t getMinute();
  uint8_t getSecond();
  uint16_t getMillisecond();
  int32_t getNanosecond();
  uint32_t getUnixEpoch();
  uint32_t getUnixEpoch(uint32_t &microsecond);

  bool getDateValid();
  bool getTimeValid();
  bool getTimeFullyResolved();
  bool getConfirmedDate();
  bool getConfirmedTime();

  uint8_t getFixType(); // Returns the type of fix: 0=no, 3=3D, 4=GNSS+Deadreckoning

  bool getGnssFixOk(); // Get whether we have a valid fix (i.e within DOP & accuracy masks)
  bool getDiffSoln();  // Get whether differential corrections were applied
  bool getHeadVehValid();
  uint8_t getCarrierSolutionType(); // Returns RTK solution: 0=no, 1=float solution, 2=fixed solution

  uint8_t getSIV();         // Returns number of sats used in fix
  int32_t getLongitude();   // Returns the current longitude in degrees * 10-7. Auto selects between HighPrecision and Regular depending on ability of module.
  int32_t getLatitude();    // Returns the current latitude in degrees * 10^-7. Auto selects between HighPrecision and Regular depending on ability of module.
  int32_t getAltitude();    // Returns the current altitude in mm above ellipsoid
  int32_t getAltitudeMSL(); // Returns the current altitude in mm above mean sea level
  uint32_t getHorizontalAccEst();
  uint32_t getVerticalAccEst();
  int32_t getNedNorthVel();
  int32_t getNedEastVel();
  int32_t getNedDownVel();
  int32_t getGroundSpeed(); // Returns speed in mm/s
  int32_t getHeading();     // Returns heading in degrees * 10^-5
  uint32_t getSpeedAccEst();
  uint32_t getHeadingAccEst();
  uint16_t getPDOP(); // Returns positional dillution of precision * 10^-2 (dimensionless)

  bool getInvalidLlh();

  int32_t getHeadVeh();
  int16_t getMagDec();
  uint16_t getMagAcc();

  int32_t getGeoidSeparation();

  // Helper functions for POSECEF
  // For safety, call these inside an if(getNAVPOSECEF()) or if(getUBX("NAV","POSECEF"))

  bool getNAVPOSECEF(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint32_t getPositionAccuracyPOSECEF(); // Returns the position accuracy estimate of the current POSECEF solution, in mm (not cm)

  // Helper functions for HPPOSECEF
  // For safety, call these inside an if(getNAVHPPOSECEF()) or if(getUBX("NAV","HPPOSECEF"))

  bool getNAVHPPOSECEF(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint32_t getPositionAccuracy(); // Returns the 3D accuracy of the current high-precision fix, in mm. Supported on NEO-M8P, ZED-F9P,
  int32_t getHighResECEFX();      // Returns the ECEF X coordinate (cm)
  int32_t getHighResECEFY();      // Returns the ECEF Y coordinate (cm)
  int32_t getHighResECEFZ();      // Returns the ECEF Z coordinate (cm)
  int8_t getHighResECEFXHp();     // Returns the ECEF X coordinate High Precision Component (0.1 mm)
  int8_t getHighResECEFYHp();     // Returns the ECEF Y coordinate High Precision Component (0.1 mm)
  int8_t getHighResECEFZHp();     // Returns the ECEF Z coordinate High Precision Component (0.1 mm)

  // Helper functions for HPPOSLLH
  // For safety, call these inside an if(getNAVHPPOSLLH()) or if(getUBX("NAV","HPPOSLLH"))

  bool getNAVHPPOSLLH(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint32_t getTimeOfWeekFromHPPOSLLH();
  int32_t getHighResLongitude();
  int32_t getHighResLatitude();
  int32_t getElipsoid();
  int32_t getMeanSeaLevel();
  int8_t getHighResLongitudeHp();
  int8_t getHighResLatitudeHp();
  int8_t getElipsoidHp();
  int8_t getMeanSeaLevelHp();
  uint32_t getHorizontalAccuracy();
  uint32_t getVerticalAccuracy();

  // Helper functions for PVAT
  // For safety, call these inside an if(getNAVPVAT()) or if(getUBX("NAV","PVAT"))

  bool getNAVPVAT(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  int32_t getVehicleRoll();    // Returns vehicle roll in degrees * 10^-5
  int32_t getVehiclePitch();   // Returns vehicle pitch in degrees * 10^-5
  int32_t getVehicleHeading(); // Returns vehicle heading in degrees * 10^-5
  int32_t getMotionHeading();  // Returns the motion heading in degrees * 10^-5

  // Helper functions for SVIN
  // For safety, call these inside an if(getNAVSVIN()) or if(getUBX("NAV","SVIN"))

  bool getNAVSVIN(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  bool getSurveyInActive();
  bool getSurveyInValid();
  uint16_t getSurveyInObservationTime();     // Truncated to 65535 seconds
  uint32_t getSurveyInObservationTimeFull(); // Return the full uint32_t
  float getSurveyInMeanAccuracy();           // Returned as m

  // Helper functions for TIMELS
  // For safety, call these inside an if(getNAVTIMELS()) or if(getUBX("NAV","TIMELS"))

  bool getNAVTIMELS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  int32_t getTimeToLsEvent();
  int8_t getCurrentLeapSeconds();

  // Helper functions for RELPOSNED
  // For safety, call these inside an if(getNAVRELPOSNED()) or if(getUBX("NAV","RELPOSNED"))

  bool getNAVRELPOSNED(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  float getRelPosN();    // Returned as m
  float getRelPosE();    // Returned as m
  float getRelPosD();    // Returned as m
  float getRelPosAccN(); // Returned as m
  float getRelPosAccE(); // Returned as m
  float getRelPosAccD(); // Returned as m

  // Helper functions for DAHEADING
  // For safety, call these inside an if(getNAVDAHEADING()) or if(getUBX("NAV","DAHEADING"))

  bool getNAVDAHEADING(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  float getDAHeadingRelPosN();    // Returned as m
  float getDAHeadingRelPosE();    // Returned as m
  float getDAHeadingRelPosD();    // Returned as m
  float getDAHeadingRelPosAccN(); // Returned as m
  float getDAHeadingRelPosAccE(); // Returned as m
  float getDAHeadingRelPosAccD(); // Returned as m

  // Helper functions for AOPSTATUS
  // For safety, call these inside an if(getNAVAOPSTATUS()) or if(getUBX("NAV","AOPSTATUS"))

  bool getNAVAOPSTATUS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint8_t getAOPSTATUSuseAOP(); // Returns the UBX-NAV-AOPSTATUS useAOP flag. Don't confuse this with getAopCfg - which returns the aopCfg byte from UBX-CFG-NAVX5
  uint8_t getAOPSTATUSstatus(); // Returns the UBX-NAV-AOPSTATUS status field. A host application can determine the optimal time to shut down the receiver by monitoring the status field for a steady 0.

  // Helper functions for TIM TP
  // For safety, call these inside an if(getTIMTP()) or if(getUBX("TIM","TP"))

  bool getTIMTP(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  uint32_t getTIMTPtowMS();                          // Returns the UBX-TIM-TP towMS time pulse of week (ms)
  uint32_t getTIMTPtowSubMS();                       // Returns the UBX-TIM-TP submillisecond part of towMS (ms * 2^-32)
  uint16_t getTIMTPweek();                           // Returns the UBX-TIM-TP time pulse week according to time base
  uint32_t getTIMTPAsEpoch(uint32_t &microsecond); // Convert TIM TP to Unix Epoch - CAUTION! Assumes the time base is UTC and the week number is GPS

  // Helper function for hardware status (including jamming)
  // For safety, call getAntennaStatus inside an if(getMONHW()) or if(getUBX("MON","HW"))

  bool getMONHW(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  bool getHWstatus(UBX_MON_HW_data_t *data = nullptr, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the hardware status using UBX_MON_HW
  sfe_ublox_antenna_status_e getAntennaStatus();         // Get the antenna status (aStatus) using UBX_MON_HW

  // Helper functions for ESF
  // For safety, call getESFroll/pitch/yaw inside an if(getESFALG()) or if(getUBX("ESF","ALG"))

  bool getESFALG(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  float getESFroll();  // Returned as degrees
  float getESFpitch(); // Returned as degrees
  float getESFyaw();   // Returned as degrees
  bool getSensorFusionMeasurement(UBX_ESF_MEAS_sensorData_t *sensorData, UBX_ESF_MEAS_data_t ubxDataStruct, uint8_t sensor);
  bool getRawSensorMeasurement(UBX_ESF_RAW_sensorData_t *sensorData, UBX_ESF_RAW_data_t ubxDataStruct, uint8_t sensor);
  bool getSensorFusionStatus(UBX_ESF_STATUS_sensorStatus_t *sensorStatus, uint8_t sensor, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool getSensorFusionStatus(UBX_ESF_STATUS_sensorStatus_t *sensorStatus, UBX_ESF_STATUS_data_t ubxDataStruct, uint8_t sensor);

  // Helper functions for HNR
  // For safety, call getHNRroll/pitch/yaw inside an if(getHNRATT()) or if(getUBX("HNR","ATT"))

  bool getHNRATT(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  bool setHNRNavigationRate(uint8_t rate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Returns true if the setHNRNavigationRate is successful
  uint8_t getHNRNavigationRate(uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);                // Returns 0 if the getHNRNavigationRate fails
  float getHNRroll();                                                           // Returned as degrees
  float getHNRpitch();                                                          // Returned as degrees
  float getHNRheading();                                                        // Returned as degrees

  // Helper functions for the remaining registered ubxMessages (thin wrappers only - see
  // AGENTS.md "getUBX()"). None of these has per-field convenience getters yet; call
  // getUBXfield(Class, ID, "fieldName", &value) directly, or use getUBX("Class","ID") /
  // findByName(), to read individual fields.

  bool getNAVPOSLLH(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);  // NAV-POSLLH: Geodetic position solution
  bool getNAVSTATUS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);  // NAV-STATUS: Receiver navigation status
  bool getNAVODO(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // NAV-ODO: Odometer solution
  bool getNAVVELECEF(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // NAV-VELECEF: Velocity solution in ECEF
  bool getNAVVELNED(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);  // NAV-VELNED: Velocity solution in NED frame
  bool getNAVTIMEUTC(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // NAV-TIMEUTC: UTC time solution
  bool getNAVCLOCK(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);   // NAV-CLOCK: Clock solution
  bool getNAVEOE(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // NAV-EOE: End of epoch marker
  bool getRXMCOR(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // RXM-COR: Differential correction input status
  bool getMONHW2(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // MON-HW2: Extended hardware status
  bool getTIMTM2(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // TIM-TM2: Time mark data
  bool getESFINS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // ESF-INS: Vehicle dynamics information
  bool getHNRPVT(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // HNR-PVT: High rate output of PVT solution
  bool getHNRINS(uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);     // HNR-INS: High rate inertial solution

  // Helper functions for the NEO-F10N
  bool getLNAMode(sfe_ublox_lna_mode_e *mode, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the LNA mode
  bool setLNAMode(sfe_ublox_lna_mode_e mode, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Set the LNA mode
  bool getGPSL5HealthOverride(bool *override, uint8_t layer = VAL_LAYER_RAM, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Get the GPS L5 health override status
  bool setGPSL5HealthOverride(bool override, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Set the GPS L5 health override status

  // Set the mainTalkerId used by NMEA messages - allows all NMEA messages except GSV to be prefixed with GP instead of GN
  bool setMainTalkerID(sfe_ublox_talker_ids_e id = SFE_UBLOX_MAIN_TALKER_ID_DEFAULT, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  // Enable/Disable NMEA High Precision Mode - include extra decimal places in the Lat and Lon
  bool setHighPrecisionMode(bool enable = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);

  // NMEA

  // Helper functions for NMEA logging
  void setNMEALoggingMask(uint32_t messages = SFE_UBLOX_FILTER_NMEA_ALL); // Add selected NMEA messages to file buffer - if enabled. Default to adding ALL messages to the file buffer
  uint32_t getNMEALoggingMask();                                          // Return which NMEA messages are selected for logging to the file buffer - if enabled

  // Helper functions to control which NMEA messages are passed to processNMEA
  void setProcessNMEAMask(uint32_t messages = SFE_UBLOX_FILTER_NMEA_ALL); // Control which NMEA messages are passed to processNMEA. Default to passing ALL messages
  uint32_t getProcessNMEAMask();                                          // Return which NMEA messages are passed to processNMEA

  // ***** v4 scaffolding - generic (Class, ID)-keyed message access. See AGENTS.md "Reference Scaffolding" *****
  nmeaMessage *getNmeaMessagePtr(nmeaCallbackDataCommon_t *theData); // Factory: hands back the opaque per-message object a callback's nmeaCallbackDataCommon_t* points at
  String getNmeaMessageFieldCallback(nmeaMessage *theMessage, const char *fieldName); // Factory: extracts a named field from the message a callback just fired for, reading from its _callbackStorage
  String getNmeaMessageField(nmeaMessage *theMessage, const char *fieldName); // Factory: extracts a named field from the message, reading from its _storage
  String getNmeaMessageBlockFieldCallback(nmeaMessage *theMessage, uint16_t blockIndex, const char *fieldName); // Factory: extracts a named field from repeated block 'blockIndex' of a variable-length message (e.g. GSV), reading from its _callbackStorage
  String getNmeaMessageBlockField(nmeaMessage *theMessage, uint16_t blockIndex, const char *fieldName); // Factory: extracts a named field from repeated block 'blockIndex' of a variable-length message (e.g. GSV), reading from its _storage

  nmeaMessageVector nmeaMessages; // v4 scaffolding - the registry of per-message objects

  bool getNMEA(const char *msgId, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait); // Generic poll-or-check-automatic, by message name
  bool getNMEAfield(const char *msgId, const char *field, String &value); // Generic field read, by message ID

  bool setAutoNMEA(const char *msgId, bool enabled = true, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoNMEA(const char *msgId, bool enabled, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool setAutoNMEArate(const char *msgId, uint8_t rate, bool implicitUpdate, uint8_t layer = VAL_LAYER_RAM_BBR, uint16_t maxWait = kUBLOXGNSSDefaultMaxWait);
  bool assumeAutoNMEA(const char *msgId, bool enabled, bool implicitUpdate = true);  // In case no config access to the GPS is possible and NMEA is send cyclically already
  void flushNMEA(const char *msgId); // Mark the NMEA data as read/stale
  void logNMEA(const char *msgId, bool enabled = true); // Log data to file buffer
  // Generic replacement for the old per-message setNMEA<MSG>callbackPtr() functions
  bool setNmeaCallbackPtr(const char *msgId, void (*callbackPointerPtr)(nmeaCallbackDataCommon_t *));

  // RTCM

  uint8_t getLatestRTCM1005(RTCM_1005_data_t *data);                           // Return the most recent RTCM 1005: 0 = no data, 1 = stale data, 2 = fresh data
  bool setRTCM1005callbackPtr(void (*callbackPointerPtr)(RTCM_1005_data_t *)); // Configure a callback for the RTCM 1005 Message

  uint8_t getLatestRTCM1005Input(RTCM_1005_data_t *data);                                // Return the most recent RTCM 1005 Input, extracted from pushRawData: 0 = no data, 1 = stale data, 2 = fresh data
  void setRTCM1005InputcallbackPtr(void (*rtcm1005CallbackPointer)(RTCM_1005_data_t *)); // Configure a callback for RTCM 1005 Input - from pushRawData
  uint8_t getLatestRTCM1006Input(RTCM_1006_data_t *data);                                // Return the most recent RTCM 1006 Input, extracted from pushRawData: 0 = no data, 1 = stale data, 2 = fresh data
  void setRTCM1006InputcallbackPtr(void (*rtcm1006CallbackPointer)(RTCM_1006_data_t *)); // Configure a callback for RTCM 1006 Input - from pushRawData

  void extractRTCM1005(RTCM_1005_data_t *destination, uint8_t *source); // Extract RTCM 1005 from source into destination
  void extractRTCM1006(RTCM_1006_data_t *destination, uint8_t *source); // Extract RTCM 1006 from source into destination

  // Helper functions for RTCM logging
  bool setRTCMLoggingMask(uint32_t messages = SFE_UBLOX_FILTER_RTCM_ALL); // Add selected RTCM messages to file buffer - if enabled. Default to adding ALL messages to the file buffer
  uint32_t getRTCMLoggingMask();                                          // Return which RTCM messages are selected for logging to the file buffer - if enabled

  // UBX Logging - log any UBX message using packetAuto and avoiding having to have and use "Auto" (setAutonnn and lognnn) methods
  void enableUBXlogging(uint8_t UBX_CLASS, uint8_t UBX_ID, bool logMe = true, bool processMe = false);

  // Functions to extract signed and unsigned 8/16/32-bit data from a ubxPacket
  // From v2.0: These are public. The user can call these to extract data from custom packets
  uint64_t extractLongLong(ubxPacket *msg, uint16_t spotToStart);      // Combine eight bytes from payload into uint64_t
  int64_t extractSignedLongLong(ubxPacket *msg, uint16_t spotToStart); // Combine eight bytes from payload into uint64_t
  uint32_t extractLong(ubxPacket *msg, uint16_t spotToStart);          // Combine four bytes from payload into long
  int32_t extractSignedLong(ubxPacket *msg, uint16_t spotToStart);     // Combine four bytes from payload into signed long (avoiding any ambiguity caused by casting)
  uint16_t extractInt(ubxPacket *msg, uint16_t spotToStart);           // Combine two bytes from payload into int
  int16_t extractSignedInt(ubxPacket *msg, uint16_t spotToStart);
  uint8_t extractByte(ubxPacket *msg, uint16_t spotToStart);      // Get byte from payload
  int8_t extractSignedChar(ubxPacket *msg, uint16_t spotToStart); // Get signed 8-bit value from payload
  float extractFloat(ubxPacket *msg, uint16_t spotToStart);       // Get signed 32-bit float (R4) from payload
  double extractDouble(ubxPacket *msg, uint16_t spotToStart);     // Get signed 64-bit double (R8) from payload

  // Functions to help extract RTCM bit fields
  uint64_t extractUnsignedBits(uint8_t *ptr, uint16_t start, uint16_t width);
  int64_t extractSignedBits(uint8_t *ptr, uint16_t start, uint16_t width);

  // Pointers to storage for the "automatic" messages
  // RAM is allocated for these if/when required.

  ubxMessageVector ubxMessages; // v4 scaffolding - the registry of per-message objects. See AGENTS.md "Reference Scaffolding"

  UBX_RXM_PMP_t *packetUBXRXMPMP = nullptr;                      // Pointer to struct. RAM will be allocated for this if/when necessary
  UBX_RXM_PMP_message_t *packetUBXRXMPMPmessage = nullptr;       // Pointer to struct. RAM will be allocated for this if/when necessary
  UBX_RXM_QZSSL6_message_t *packetUBXRXMQZSSL6message = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary
  // packetUBXRXMSFRBX no longer exists - ubxRXMSFRBX is now self-registered - see AGENTS.md
  // "Adding support for RXM-SFRBX".
  // packetUBXRXMRAWX/packetUBXRXMMEASX no longer exist - ubxRXMRAWX/ubxRXMMEASX are now
  // self-registered - see AGENTS.md "Adding the variable-length UBX messages".

  // packetUBXMONCOMMS no longer exists - ubxMONCOMMS is now self-registered - see AGENTS.md
  // "Adding the variable-length UBX messages".

  UBX_ESF_MEAS_t *packetUBXESFMEAS = nullptr;     // Pointer to struct. RAM will be allocated for this if/when necessary
  UBX_ESF_RAW_t *packetUBXESFRAW = nullptr;       // Pointer to struct. RAM will be allocated for this if/when necessary
  UBX_ESF_STATUS_t *packetUBXESFSTATUS = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary

  // packetUBXSECSIG no longer exists - ubxSECSIG is now self-registered - see AGENTS.md
  // "Adding the variable-length UBX messages".

  UBX_MGA_ACK_DATA0_t *packetUBXMGAACK = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary
  UBX_MGA_DBD_t *packetUBXMGADBD = nullptr;       // Pointer to struct. RAM will be allocated for this if/when necessary

  RTCM_1005_t *storageRTCM1005 = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary

  struct
  {
    union
    {
      uint8_t all;
      struct
      {
        uint8_t dataValid1005 : 1;
        uint8_t dataRead1005 : 1;
        uint8_t dataValid1006 : 1;
        uint8_t dataRead1006 : 1;
      } bits;
    } flags;
    RTCM_1005_data_t rtcm1005; // Latest RTCM 1005 parsed from pushRawData
    RTCM_1006_data_t rtcm1006; // Latest RTCM 1006 parsed from pushRawData
    void (*rtcm1005CallbackPointer)(RTCM_1005_data_t *);
    void (*rtcm1006CallbackPointer)(RTCM_1006_data_t *);
    void init(void) // Initializer / constructor
    {
      flags.all = 0;                     // Clear the RTCM Input flags
      rtcm1005CallbackPointer = nullptr; // Clear the callback pointers
      rtcm1006CallbackPointer = nullptr;
    }
  } rtcmInputStorage; // Latest RTCM parsed from pushRawData

  uint16_t rtcmFrameCounter = 0; // Tracks the type of incoming byte inside RTCM frame

protected:
  // Depending on the ubx binary response class, store binary responses into different places
  enum classTypes
  {
    CLASS_NONE = 0,
    CLASS_ACK,
    CLASS_NOT_AN_ACK
  } ubxFrameClass = CLASS_NONE;

  // Functions

  bool setAutoMsgRateVal(uint32_t key, uint8_t rate, bool implicitUpdate, ubxAutomaticFlags &flags, uint8_t layer, uint16_t maxWait); // Helper for setAuto*rate functions using VALSET

  bool checkUbloxInternal(ubxPacket *incomingUBX, uint8_t requestedClass = 0, uint8_t requestedID = 0); // Checks module with user selected commType
  void addToChecksum(uint8_t incoming);                                                                 // Given an incoming byte, adjust rollingChecksumA/B
  size_t pushAssistNowDataInternal(size_t offset, bool skipTime, const uint8_t *dataBytes, size_t numDataBytes, sfe_ublox_mga_assist_ack_e mgaAck, uint16_t maxWait);
  size_t findMGAANOForDateInternal(const uint8_t *dataBytes, size_t numDataBytes, uint16_t year, uint8_t month, uint8_t day, uint8_t daysIntoFuture);

  // Return true if this "automatic" message has storage allocated for it. Also provide the associated max payload size
  bool autoLookup(uint8_t Class, uint8_t ID, uint16_t *maxSize = nullptr);

  bool initGeofenceParams();  // Allocate RAM for currentGeofenceParams and initialize it
  bool initModuleSWVersion(); // Allocate RAM for moduleSWVersion and initialize it

  bool initPacketUBXRXMPMP();           // Allocate RAM for packetUBXRXMPMP and initialize it
  bool initPacketUBXRXMPMPmessage();    // Allocate RAM for packetUBXRXMPMPRaw and initialize it
  bool initPacketUBXRXMQZSSL6message(); // Allocate RAM for packetUBXRXMQZSSL6raw and initialize it
  bool initPacketUBXESFSTATUS();        // Allocate RAM for packetUBXESFSTATUS and initialize it
  bool initPacketUBXESFMEAS();          // Allocate RAM for packetUBXESFMEAS and initialize it
  bool initPacketUBXESFRAW();           // Allocate RAM for packetUBXESFRAW and initialize it
  bool initPacketUBXMGAACK();           // Allocate RAM for packetUBXMGAACK and initialize it
  bool initPacketUBXMGADBD();           // Allocate RAM for packetUBXMGADBD and initialize it

  bool initStorageRTCM(); // Allocate RAM for incoming RTCM messages and initialize it
  bool initStorageNMEA(); // Allocate RAM for incoming non-Auto NMEA messages and initialize it

  bool initStorageRTCM1005(); // Allocate RAM for incoming RTCM 1005 messages and initialize it

  // Variables
  SparkFun_UBLOX_GNSS::GNSSDeviceBus *_sfeBus;

  SparkFun_UBLOX_GNSS::SfePrint _nmeaOutputPort; // The user can assign an output port to print NMEA sentences if they wish
  SparkFun_UBLOX_GNSS::SfePrint _rtcmOutputPort; // The user can assign an output port to print RTCM sentences if they wish
  SparkFun_UBLOX_GNSS::SfePrint _ubxOutputPort;  // The user can assign an output port to print UBX sentences if they wish
  SparkFun_UBLOX_GNSS::SfePrint _outputPort;     // The user can assign an output port to print ALL characters to if they wish
  // _debugSerial/_printDebug/_printLimitedDebug are inherited from SfeDebugPrint (see sfe_debug.h)

  // The packet buffers
  // These are pointed at from within the ubxPacket
  uint8_t payloadAck[2];           // Holds the requested ACK/NACK
  uint8_t payloadBuf[2];           // Temporary buffer used to screen incoming packets or dump unrequested packets
  size_t packetCfgPayloadSize = 0; // Size for the packetCfg payload. .begin will set this to MAX_PAYLOAD_SIZE if necessary. User can change with setPacketCfgPayloadSize
  uint8_t *payloadCfg = nullptr;
  uint8_t *payloadAuto = nullptr;

  uint8_t *spiBuffer = nullptr;                                // A buffer to store any bytes being recieved back from the device while we are sending via SPI
  size_t spiBufferIndex = 0;                                   // Index into the SPI buffer
  size_t spiBufferSize = SFE_UBLOX_SPI_BUFFER_DEFAULT_SIZE;    // Default size of the SPI buffer
  uint8_t spiTransactionSize = SFE_UBLOX_SPI_TRANSACTION_SIZE; // Default size of SPI transactions

  // Init the packet structures and init them with pointers to the payloadAck, payloadCfg, payloadBuf and payloadAuto arrays
  ubxPacket packetAck = {0, 0, 0, 0, 0, payloadAck, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};
  ubxPacket packetBuf = {0, 0, 0, 0, 0, payloadBuf, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};
  ubxPacket packetCfg = {0, 0, 0, 0, 0, payloadCfg, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};
  ubxPacket packetAuto = {0, 0, 0, 0, 0, payloadAuto, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};

  // Flag if this packet is unrequested (and so should be ignored and not copied into packetCfg or packetAck)
  bool ignoreThisPayload = false;

  // Identify which buffer is in use
  // Data is stored in packetBuf until the requested class and ID can be validated
  // If a match is seen, data is diverted into packetAck or packetCfg
  //"Automatic" messages which have RAM allocated for them are diverted into packetAuto
  sfe_ublox_packet_buffer_e activePacketBuffer = SFE_UBLOX_PACKET_PACKETBUF;

  // Limit checking of new data to every X ms
  // If we are expecting an update every X Hz then we should check every quarter that amount of time
  // Otherwise we may block ourselves from seeing new data
  uint8_t i2cPollingWait = 100;    // Default to 100ms. Adjusted when user calls setNavigationFrequency() or setHNRNavigationRate() or setMeasurementRate()
  uint8_t i2cPollingWaitNAV = 100; // We need to record the desired polling rate for standard nav messages
  uint8_t i2cPollingWaitHNR = 100; // and for HNR too so we can set i2cPollingWait to the lower of the two

  // The SPI polling wait is a little different. checkUbloxSpi will delay for this amount before returning if
  // there is no data waiting to be read. This prevents waitForACKResponse from pounding the SPI bus too hard.
  uint8_t spiPollingWait = 9; // Default to 9ms; waitForACKResponse delays for 1ms on top of this. User can adjust with setSPIPollingWait.

  unsigned long lastCheck = 0;

  uint16_t ubxFrameCounter; // Count all UBX frame bytes. [Fixed header(2bytes), CLS(1byte), ID(1byte), length(2bytes), payload(x bytes), checksums(2bytes)]
  uint8_t rollingChecksumA; // Rolls forward as we receive incoming bytes. Checked against the last two A/B checksum bytes
  uint8_t rollingChecksumB; // Rolls forward as we receive incoming bytes. Checked against the last two A/B checksum bytes

  // NMEA logging / Auto support
  sfe_ublox_nmea_filtering_t _logNMEA;     // Flags to indicate which NMEA messages should be added to the file buffer for logging
  sfe_ublox_nmea_filtering_t _processNMEA; // Flags to indicate which NMEA messages should be passed to processNMEA

  int8_t nmeaByteCounter; // Count all NMEA message bytes.
  // Abort NMEA message reception if nmeaByteCounter exceeds maxNMEAByteCount.
  // The user can adjust maxNMEAByteCount by calling setMaxNMEAByteCount
  int8_t maxNMEAByteCount = SFE_UBLOX_MAX_NMEA_BYTE_COUNT;
  uint8_t nmeaAddressField[6]; // NMEA Address Field - includes the start character (*)
  bool logThisNMEA(const char *msgId); // Return true if we should log this NMEA message
  bool processThisNMEA(const char *msgId); // Return true if we should pass this NMEA message to processNMEA
  bool isNMEAHeaderValid(const char *msgId); // Return true if the six byte NMEA header appears valid. Used to set _signsOfLife
  bool isThisNMEAauto(const char *msgId); // Return true if msgId is known to be automatic / periodic
  bool doesThisNMEAHaveStorage(const char *msgId); // Return true if this msgId has "Auto" storage allocated - BUT it may not actually be "Auto"
  bool doesThisNMEAHaveCallback(const char *msgId); // Return true if this msgId has a callback

  NMEA_STORAGE_t *_storageNMEA = nullptr; // Pointer to struct. RAM will be allocated for this if/when necessary

  // RTCM logging
  sfe_ublox_rtcm_filtering_t _logRTCM; // Flags to indicate which NMEA messages should be added to the file buffer for logging

  RTCM_FRAME_t *_storageRTCM = nullptr;              // Pointer to struct. RAM will be allocated for this if/when necessary
  void crc24q(uint8_t incoming, uint32_t *checksum); // Add incoming to checksum as per CRC-24Q

  // Define the maximum possible message length for packetAuto and enableUBXlogging
  // On the ZED-X20P, we see:
  //   RXM-RAWX messages containing 3920 bytes (122 blocks)
  //   NAV-SAT messages containing 644 bytes (53 blocks)
  //   NAV-SIG messages containing 2248 bytes (140 blocks)
  const uint16_t SFE_UBX_MAX_LENGTH = UBX_NAV_SAT_MAX_LEN;

  // UBX logging
  sfe_ublox_ubx_logging_list_t *sfe_ublox_ubx_logging_list_head = nullptr; // Linked list of which messages to log
  bool logThisUBX(uint8_t UBX_CLASS, uint8_t UBX_ID);                      // Returns true if this UBX should be added to the logging buffer - for logging
  bool processThisUBX(uint8_t UBX_CLASS, uint8_t UBX_ID);                  // Returns true if this UBX should be added to the logging buffer - for processing
  bool logOrProcessThisUBX(uint8_t UBX_CLASS, uint8_t UBX_ID, bool log);   // Called by logThisUBX and processThisUBX

  // Flag to prevent reentry into checkCallbacks
  // Prevent badness if the user accidentally calls checkCallbacks from inside a callback
  volatile bool checkCallbacksReentrant = false;

  // Support for data logging
  uint8_t *ubxFileBuffer = nullptr;                             // Pointer to the file buffer. RAM is allocated for this if required in .begin
  uint16_t fileBufferSize = 0;                                  // The size of the file buffer. This can be changed by calling setFileBufferSize _before_ .begin
  uint16_t fileBufferHead = 0;                                  // The incoming byte is written into the file buffer at this location
  uint16_t fileBufferTail = 0;                                  // The next byte to be read from the buffer will be read from this location
  uint16_t fileBufferMaxAvail = 0;                              // The maximum number of bytes the file buffer has contained. Handy for checking the buffer is large enough to handle all the incoming data.
  bool createFileBuffer(void);                                  // Create the file buffer. Called by .begin
  uint16_t fileBufferSpaceAvailable(void);                      // Check how much space is available in the buffer
  uint16_t fileBufferSpaceUsed(void);                           // Check how much space is used in the buffer
  bool storePacket(ubxPacket *msg);                             // Add a UBX packet to the file buffer
  bool storeFileBytes(uint8_t *theBytes, uint16_t numBytes);    // Add theBytes to the file buffer
  void writeToFileBuffer(uint8_t *theBytes, uint16_t numBytes); // Write theBytes to the file buffer

  // Support for RTCM buffering
  uint8_t *rtcmBuffer = nullptr;                                // Pointer to the RTCM buffer. RAM is allocated for this if required in .begin
  uint16_t rtcmBufferSize = 0;                                  // The size of the RTCM buffer. This can be changed by calling setRTCMBufferSize _before_ .begin
  uint16_t rtcmBufferHead = 0;                                  // The incoming byte is written into the buffer at this location
  uint16_t rtcmBufferTail = 0;                                  // The next byte to be read from the buffer will be read from this location
  bool createRTCMBuffer(void);                                  // Create the RTCM buffer. Called by .begin
  uint16_t rtcmBufferSpaceAvailable(void);                      // Check how much space is available in the buffer
  uint16_t rtcmBufferSpaceUsed(void);                           // Check how much space is used in the buffer
  bool storeRTCMBytes(uint8_t *theBytes, uint16_t numBytes);    // Add theBytes to the buffer
  void writeToRTCMBuffer(uint8_t *theBytes, uint16_t numBytes); // Write theBytes to the buffer

  // .begin will return true if the assumeSuccess parameter is true and if _signsOfLife is true
  // _signsOfLife is set to true when: a valid UBX message is seen; a valig NMEA header is seen.
  bool _signsOfLife;

  // Keep track of how many keys have been added to CfgValset
  uint8_t _numCfgKeys = 0;

  // Keep track of how many keys have been added to CfgValget and what size the response will be
  uint8_t _numGetCfgKeys = 0;
  uint16_t _lenCfgValGetResponse = 0;
  uint8_t *cfgValgetValueSizes = nullptr; // A pointer to a list of the value sizes for each key in the cfgValget
  uint16_t _cfgValgetMaxPayload = 0;

  // Send the current CFG_VALSET message when packetCfg has less than this many bytes available
  size_t _autoSendAtSpaceRemaining = 0;

public:
  // Flag to indicate if currentSentence should be reset on a (I2C) bus error
  bool _resetCurrentSentenceOnBusError = true;

  typedef union
  {
    uint64_t unsigned64;
    int64_t signed64;
  } unsignedSigned64;

  typedef union
  {
    uint32_t unsigned32;
    int32_t signed32;
  } unsignedSigned32;

  typedef union
  {
    uint16_t unsigned16;
    int16_t signed16;
  } unsignedSigned16;

  typedef union
  {
    uint8_t unsigned8;
    int8_t signed8;
  } unsignedSigned8;

  typedef union
  {
    uint32_t unsigned32;
    float flt;
  } unsigned32float;

  typedef union
  {
    uint64_t unsigned64;
    double dbl;
  } unsigned64double;
};
