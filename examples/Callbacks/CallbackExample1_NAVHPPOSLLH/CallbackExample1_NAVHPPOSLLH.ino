/*
  Reading HPPOSLLH using a Callback
  By: Paul Clark
  SparkFun Electronics
  Date: September 2026
  License: MIT. Please see LICENSE.md for more information.

  This example shows how to use a callback to print the High Precision PVT LLH
  data from the GNSS.
  
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

void printPVTdata(ubxCallbackDataCommon_t *theData)
{
    auto theDataStruct = getCallbackDataStruct(theData);

    auto timeOfWeek = getFieldFromCallbackDataStruct(theDataStruct, "iTOW");
    Serial.print(F("TimeOfWeek: "));
    Serial.print(timeOfWeek); // Print the Time Of Week
    Serial.print(F(" (ms)"));

    auto latitude = getFieldFromCallbackDataStruct(theDataStruct, "lat");
    Serial.print(F(" Lat: "));
    Serial.print(latitude); // Print the latitude

    auto longitude = getFieldFromCallbackDataStruct(theDataStruct, "lon");
    Serial.print(F(" Long: "));
    Serial.print(longitude); // Print the longitude
    Serial.print(F(" (degrees * 10^-7)"));

    auto hAcc = getFieldFromCallbackDataStruct(theDataStruct, "hAcc");
    Serial.print(F(" Horiz Acc: "));
    Serial.print(hAcc / 10); // Print the horizontal accuracy estimate
    Serial.println(F(" (mm)"));
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
    Serial.println(F("u-blox GNSS not detected at default I2C address. Retrying..."));
    delay (1000);
  }

  // Enable the NAV HPPOSLLH Message on I2C at the navigation rate
  myGNSS.setCfgValset(UBLOX_CFG_MSGOUT_UBX_NAV_POSLLH_I2C, 1);

  // Enable automatic NAV HPPOSLLH messages with callback to printPVTdata
  myGNSS.setAutoCallbackPtr("NAV", "HPPOSLLH", &printPVTdata);
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(50);
}
