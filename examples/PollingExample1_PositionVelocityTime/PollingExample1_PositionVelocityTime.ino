/*
 * @date 2026
 * @copyright Copyright (c) 2026, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 * 
 * Reading Position, Velocity and Time (PVT) via UBX binary commands
 * By: Paul Clark
 * SparkFun Electronics
 *
 * This example shows how to poll the u-blox module position, velocity and time (PVT) data.
 *  
 * Feel like supporting open source hardware?
 * Buy a board from SparkFun!
 * https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html
 * https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html
 * https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html
 *
 * Hardware Connections:
 * Plug a Qwiic cable into the GNSS and your microcontroller board
 * Open the serial monitor at 115200 baud to see the output
 */

#include <Wire.h> //Needed for I2C to GNSS

#include <SparkFun_u-blox_GNSS_v4.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v4

SFE_UBLOX_GNSS myGNSS; // SFE_UBLOX_GNSS uses I2C. For Serial or SPI, see Example2 and Example3

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
}

void loop()
{
  // Poll the position, velocity and time (PVT) information.
  // getNAVPVT() returns true when new data is received.
  if (myGNSS.getNAVPVT() == true) // Use the helper method getNAVPVT()
  {
    int32_t latitude = myGNSS.getLatitude(); // Use the helper method
    Serial.print("Lat: ");
    Serial.print(latitude);

    int32_t longitude = myGNSS.getLongitude(); // Use the helper method
    Serial.print(" Long: ");
    Serial.print(longitude);
    Serial.print(" (degrees * 10^-7)");

    int32_t altitude = myGNSS.getAltitudeMSL(); // Helper method for Altitude above Mean Sea Level
    Serial.print(" Alt: ");
    Serial.print(altitude);
    Serial.print(" (mm)");

    Serial.println();
  }

  // Poll the position, velocity and time (PVT) information.
  // getUBX() returns true when new data is received.
  if (myGNSS.getUBX("NAV","PVT") == true)
  {
    ubxMessage *msg = myGNSS.ubxMessages.findByName("NAV","PVT");

    // getUbxMessageField converts everything to double. Convert lat back to uint32_t
    int32_t latitude = (int32_t)myGNSS.getUbxMessageField(msg, "lat");
    Serial.print("Lat: ");
    Serial.print(latitude);

    // Or, we could read the true "I4" (int32_t) directly, without going through double
    // To do that, we need to use the ubxAnyType struct
    ubxAnyType ubxAnyTypeLon = myGNSS.getUbxMessageField(msg, "lon");
    Serial.print(" Long: ");
    Serial.print(ubxAnyTypeLon.I4); // Print the longitude directly as int32_t
    Serial.print(" (degrees * 10^-7)");

    int32_t altitude = (int32_t)myGNSS.getUbxMessageField(msg, "hMSL");
    Serial.print(" Alt: ");
    Serial.print(altitude);
    Serial.print(" (mm)");

    Serial.println();
  }
}
