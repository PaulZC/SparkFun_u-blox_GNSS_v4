/*
  Configuring the GNSS to automatically send ESF STATUS reports over I2C and display them using a callback
  By: Paul Clark
  SparkFun Electronics
  Date: September 2026
  License: MIT. Please see LICENSE.md for more information.

  This example configures the External Sensor Fusion STATUS sensor messages on the ZED-F9R and
  shows how to access the ESF data using callbacks.
  
  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html

  Hardware Connections:
  Plug a Qwiic cable into the GNSS and your microcontroller board

  Open the serial monitor at >>> 460800 <<< baud to see the output

*/

#include <Wire.h> //Needed for I2C to GPS

#include <SparkFun_u-blox_GNSS_v4.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v4

SFE_UBLOX_GNSS myGNSS;

// Callback: printESFSTATUSdata will be called when new ESF STATUS data arrives
void printESFSTATUSdata(ubxCallbackDataCommon_t *theData)
{
  ubxMessage *msg = myGNSS.getUbxMessagePtr(theData);

  if (1) // Change to 0 to disable the callback prints. Useful when trying to capture debug...
  {
    Serial.println();

    // Print the iTOW
    uint32_t iTOW = (uint32_t)myGNSS.getUbxMessageFieldCallback(msg, "iTOW");
    Serial.print(F("TOW:          "));
    Serial.println(iTOW);

    // numSens indicates how many sensor groups the data contains.
    // As a test, compare it to getUbxMessageBlockCount()
    uint8_t numSens = (uint8_t)myGNSS.getUbxMessageFieldCallback(msg, "numSens");
    Serial.print(F("Sensors:      "));
    Serial.println(numSens);
    if (numSens != myGNSS.getUbxMessageBlockCountCallback(msg))
    {
      Serial.print("!!! numSens (");
      Serial.print(numSens);
      Serial.print(") does not match getUbxMessageBlockCount (");
      Serial.print(myGNSS.getUbxMessageBlockCountCallback(msg));
      Serial.println(") !!!");
    }

    for (uint8_t i = 0; i < numSens; i++)
    {
      // Print the sensor data type
      // From the M8 interface description:
      //   0: None
      // 1-4: Reserved
      //   5: z-axis gyroscope angular rate deg/s * 2^-12 signed
      //   6: front-left wheel ticks: Bits 0-22: unsigned tick value. Bit 23: direction indicator (0=forward, 1=backward)
      //   7: front-right wheel ticks: Bits 0-22: unsigned tick value. Bit 23: direction indicator (0=forward, 1=backward)
      //   8: rear-left wheel ticks: Bits 0-22: unsigned tick value. Bit 23: direction indicator (0=forward, 1=backward)
      //   9: rear-right wheel ticks: Bits 0-22: unsigned tick value. Bit 23: direction indicator (0=forward, 1=backward)
      //  10: speed ticks: Bits 0-22: unsigned tick value. Bit 23: direction indicator (0=forward, 1=backward)
      //  11: speed m/s * 1e-3 signed
      //  12: gyroscope temperature deg Celsius * 1e-2 signed
      //  13: y-axis gyroscope angular rate deg/s * 2^-12 signed
      //  14: x-axis gyroscope angular rate deg/s * 2^-12 signed
      //  16: x-axis accelerometer specific force m/s^2 * 2^-10 signed
      //  17: y-axis accelerometer specific force m/s^2 * 2^-10 signed
      //  18: z-axis accelerometer specific force m/s^2 * 2^-10 signed
      uint8_t dataType = (uint8_t)myGNSS.getUbxMessageBlockFieldCallback(msg, i, "type");
      switch (dataType)
      {
      case 5:
        Serial.print(F("Z Gyro:       "));
        break;
      case 6:
        Serial.print(F("Front Left:   "));
        break;
      case 7:
        Serial.print(F("Front Right:  "));
        break;
      case 8:
        Serial.print(F("Rear Left:    "));
        break;
      case 9:
        Serial.print(F("Rear Right:   "));
        break;
      case 10:
        Serial.print(F("Speed Ticks:  "));
        break;
      case 11:
        Serial.print(F("Speed:        "));
        break;
      case 12:
        Serial.print(F("Temp:         "));
        break;
      case 13:
        Serial.print(F("Y Gyro:       "));
        break;
      case 14:
        Serial.print(F("X Gyro:       "));
        break;
      case 16:
        Serial.print(F("X Accel:      "));
        break;
      case 17:
        Serial.print(F("Y Accel:      "));
        break;
      case 18:
        Serial.print(F("Z Accel:      "));
        break;
      default:
        break;
      }

      uint8_t calibStatus = (uint8_t)myGNSS.getUbxMessageBlockFieldCallback(msg, i, "calibStatus");
      Serial.print("calibStatus:  ");
      switch (calibStatus)
      {
        case 0: // 00=not calibrated, 01=calibrating, 10/11=calibrated
          Serial.println("not calibrated");
          break;
        case 1: // 00=not calibrated, 01=calibrating, 10/11=calibrated
          Serial.println("calibrating");
          break;
        case 2: // 00=not calibrated, 01=calibrating, 10/11=calibrated
        case 3:
          Serial.println("calibrated");
          break;
        default:
          Serial.println("UNKNOWN");
          break;
      }
    }
  }
}

void setup()
{
  Serial.begin(460800); // <-- Use a fast baud rate to avoid the Serial prints slowing the code
  delay(1000); 
  Serial.println("SparkFun u-blox Example");

  Wire.begin(); // Start I2C
  Wire.setClock(400000); // <-- Use 400kHz I2C

  //myGNSS.enableDebugging(Serial, true); // Uncomment this line to enable the important debug messages on Serial

  while (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println("u-blox GNSS not detected at default I2C address. Retrying...");
    delay (1000);
  }

  // Enable the ESF STATUS Message on I2C, every navigation cycle
  myGNSS.setCfgValset(UBLOX_CFG_MSGOUT_UBX_ESF_STATUS_I2C, 1);

  // Set up a callback for ESF STATUS messages. Call newSECSIG() each time one arrives.
  // Note: this does not enable the SEC SIG message. The message is assumed to be periodic.
  // All this does is register the callback.
  myGNSS.setAutoCallbackPtr("ESF", "STATUS", &printESFSTATUSdata);
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.
}
