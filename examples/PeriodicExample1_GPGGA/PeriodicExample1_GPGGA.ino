/*
  Marking NMEA GPGGA as periodic ("Automatic") and reading its data in an if statement
  By: Paul Clark
  SparkFun Electronics
  Date: September 2026
  License: MIT. Please see LICENSE.md for more information.

  u-blox GNSS modules output the NMEA GGA message by default. We do not need to enable
  it - unless it was previously disabled. In this example, we tell the library to
  expect the GGA periodic messages without specifically enabling them.
  
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

  // NMEA GGA should already be periodic ("automatic"). Tell the library to expect it
  myGNSS.assumeAutoNMEA("GGA", true);
}

void loop()
{
  // Check for the arrival of a new NMEA GGA message
  if (myGNSS.getNMEA("GGA"))
  {
    Serial.println();

    nmeaMessage *msg = myGNSS.nmeaMessages.find("GGA");

    Serial.print("UTC time: ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "time")); // Print the UTC time

    Serial.print(" Lat: ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "lat")); // Print the latitude
    Serial.print(" ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "NS")); // Print the North / South indicator

    // getNmeaMessageField returns everything as String
    // Use atoi(), atof() or strtod() to convert to numeric types as needed
    Serial.print(" Long: ");
    float longitude = atof(myGNSS.getNmeaMessageField(msg, "lon").c_str()); // Convert String to float
    Serial.print(longitude, 8); // Print the longitude with 8 decimal places
    Serial.print(" ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "EW")); // Print the East / West indicator
    Serial.print(" (degrees)");

    Serial.print(" Alt: ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "alt")); // Altitude above MSL
    Serial.println(" (m)");
  }

  Serial.print(".");
  delay(50);
}
