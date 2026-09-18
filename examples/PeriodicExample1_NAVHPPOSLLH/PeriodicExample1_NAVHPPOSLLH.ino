/*
  Making HPPOSLLH periodic ("Automatic") and reading its data in an if statement
  By: Paul Clark
  SparkFun Electronics
  Date: September 2026
  License: MIT. Please see LICENSE.md for more information.

  Here we make the NAV-HPPOSLLH message periodic by calling setAutoUBX("NAV", "HPPOSLLH");
  The library then understands that the HPPOSLLH message will arrive "automatically"
  (once per navigation interval). getNAVHPPOSLLH() no longer polls the HPPOSLLH
  message. Instead it becomes non-blocking, returning true (once) when a new HPPOSLLH
  message is received.
  
  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html
  https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html
  https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html
  https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html
  https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html

  Hardware Connections:
  Plug a Qwiic cable into the GNSS and your microcontroller board
  Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_v4.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v4

SFE_UBLOX_GNSS myGNSS; // SFE_UBLOX_GNSS uses I2C

void setup()
{
  Serial.begin(115200);
  delay(1000); 
  Serial.println("SparkFun u-blox Example");

  Wire.begin(); // Start I2C

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  while (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println("u-blox GNSS not detected at default I2C address. Retrying...");
    delay (1000);
  }

  // Enable periodic ("automatic") NAV HPPOSLLH messages on I2C
  myGNSS.setAutoUBX("NAV", "HPPOSLLH");
}

void loop()
{
  // Call the helper method getNAVHPPOSLLH()
  // Internally, it calls getUBX(UBX_CLASS_NAV, UBX_NAV_HPPOSLLH)
  // Because we called setAutoUBX("NAV", "HPPOSLLH"), the library knows
  // HPPOSLLH messages will be arriving periodically.
  // If no fresh NAV-HPPOSLLH message has arrived, getNAVHPPOSLLH() returns false.
  // It returns true after the arrival of a new fresh NAV-HPPOSLLH message.
  // It acts as a one-shot; it will only return true once on the arrival of the
  // new HPPOSLLH message. To ensure we are reading fresh data, we should read
  // the HPPOSLLH fields within the `if getNAVHPPOSLLH()`:

  if (myGNSS.getNAVHPPOSLLH())
  {
    Serial.println();

    ubxMessage *msg = myGNSS.ubxMessages.findByName("NAV","HPPOSLLH");

    // getUbxMessageField returns everything as double. Cast to other types as needed
    unsigned long timeOfWeek = (unsigned long)myGNSS.getUbxMessageField(msg, "iTOW");
    Serial.print("TimeOfWeek: ");
    Serial.print(timeOfWeek); // Print the Time Of Week
    Serial.print(" (ms)");

    long latitude = (long)myGNSS.getUbxMessageField(msg, "lat");
    Serial.print(" Lat: ");
    Serial.print(latitude); // Print the latitude

    // Or, we could read the true "I4" (int32_t) directly, without going through double
    // To do that, we need to use the ubxAnyType struct
    ubxAnyType ubxAnyTypeLon = myGNSS.getUbxMessageField(msg, "lon");
    Serial.print(" Long: ");
    Serial.print(ubxAnyTypeLon.I4); // Print the longitude directly as int32_t
    Serial.print(" (degrees * 10^-7)");

    float hAcc = (float)myGNSS.getUbxMessageField(msg, "hAcc");
    Serial.print(" Horiz Acc: ");
    Serial.print(hAcc / 10.0, 1); // Print the horizontal accuracy estimate
    Serial.println(" (mm)");
  }

  Serial.print(".");
  delay(50);
}
