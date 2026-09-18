/*
  Reading NMEA RMC (GPRMC) using a Callback
  By: Paul Clark
  SparkFun Electronics
  Date: September 2026
  License: MIT. Please see LICENSE.md for more information.

  u-blox GNSS modules output the NMEA GGA, GLL, GSA, GSV, RMC, and VTG messages
  by default. We do not need to enable them - unless they were previously disabled.
  In this example, we tell the library to trigger a callback when the RMC message
  arrives.
  
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

void printNMEAdata(nmeaCallbackDataCommon_t *theData)
{
    Serial.println();

    nmeaMessage *msg = myGNSS.getNmeaMessagePtr(theData);

    Serial.print("UTC time: ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "time")); // Print the UTC time

    Serial.print(" Date: ");
    Serial.print(myGNSS.getNmeaMessageField(msg, "date")); // UTC date: DDMMYY

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
    Serial.println(" (degrees)");
}

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

  // Set up a callback for NMEA RMC messages. Call printNMEAdata() each time one arrives.
  // Note: this does not enable the RMC message. The message is assumed to be periodic.
  // All this does is register the callback.
  myGNSS.setNmeaCallbackPtr("RMC", &printNMEAdata);
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(50);
}
