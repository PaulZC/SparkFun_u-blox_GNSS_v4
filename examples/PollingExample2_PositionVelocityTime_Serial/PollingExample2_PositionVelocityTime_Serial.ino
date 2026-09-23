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
 * This example shows how to poll the u-blox module position, velocity and time (PVT) data using Serial (UART).
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
 * Hook up the TX, RX and GND pins, plus 3V3 or 5V depending on your needs
 * Connect: GNSS TX to microcontroller RX; GNSS RX to microcontroller TX
 * Open the serial monitor at 115200 baud to see the output
 */

#include <SparkFun_u-blox_GNSS_v4.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v4

SFE_UBLOX_GNSS_SERIAL myGNSS; // SFE_UBLOX_GNSS_SERIAL uses Serial (UART). For I2C or SPI, see Example1 and Example3

#define mySerial Serial1 // Use Serial1 to connect to the GNSS module. Change this if required

void setup()
{
  Serial.begin(115200);
  delay(1000); 
  Serial.println("SparkFun u-blox Example");

  mySerial.begin(38400); // u-blox X20, F9 and M10 modules default to 38400 baud. Change this if required

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  while (myGNSS.begin(mySerial) == false) //Connect to the u-blox module using mySerial (defined above)
  {
    Serial.println("u-blox GNSS not detected. Retrying...");
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
}
