/*
 * @date 2026
 * @copyright Copyright (c) 2026, SparkFun Electronics Inc. This project is released under the MIT License.
 *
 * SPDX-License-Identifier: MIT
 * 
 * Polling the NMEA ZDA message
 * By: Paul Clark
 * SparkFun Electronics
 *
 * u-blox GNSS modules output the NMEA  GGA, GLL, GSA, GSV, RMC, and VTG messages
 * by default. We do not need to enable them - unless they were previously disabled.
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
}

void loop()
{
  // ZDA is not output by default. We need to Poll it (or make it periodic)
  if (myGNSS.getNMEA("ZDA")) // This will poll (request) the ZDA message
  {
    nmeaMessage *msg = myGNSS.nmeaMessages.find("ZDA");

    Serial.print("UTC time: ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "time")); // Print the UTC time

    Serial.print(" ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "day")); // Print the day
    Serial.print("/");
    Serial.print(myGNSS.getNmeaMessageField(msg, "month")); // Print the month
    Serial.print("/");

    // getNmeaMessageField returns everything as String
    // Use atoi(), atof() or strtod() to convert to numeric types as needed
    int year = atoi(myGNSS.getNmeaMessageField(msg, "year").c_str()); // Convert String to int
    Serial.println(year);
  }
}
