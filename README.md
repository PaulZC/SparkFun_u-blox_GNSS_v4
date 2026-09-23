# SparkFun u-blox GNSS Arduino Library - v4

<table class="table table-hover table-striped table-bordered">
  <tr align="center">
   <td><a href="https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html"><img src="https://www.sparkfun.com/media/catalog/product/cache/f3020b7489dcfc4d1d147cf4dad07b7f/2/8/28871-zed-x20p-breakout-feature.jpg"></a></td>
   <td><a href="https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html"><img src="https://www.sparkfun.com/media/catalog/product/cache/f3020b7489dcfc4d1d147cf4dad07b7f/1/5/15136-SparkFun_GPS-RTK2_Board_-_ZED-F9P__Qwiic_-03.jpg"></a></td>
   <td><a href="https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html"><img src="https://www.sparkfun.com/media/catalog/product/cache/f3020b7489dcfc4d1d147cf4dad07b7f/1/6/16481-SparkFun_GPS-RTK-SMA_Breakout_-_ZED-F9P__Qwiic_-01a.jpg"></a></td>
   <td><a href="https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html"><img src="https://www.sparkfun.com/media/catalog/product/cache/f3020b7489dcfc4d1d147cf4dad07b7f/1/8/18037-SparkFun_GNSS_Receiver_Breakout_-_MAX-M10S__Qwiic_-01_Default.jpg"></a></td>
    <td><a href="https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html"><img src="https://www.sparkfun.com/media/catalog/product/cache/f3020b7489dcfc4d1d147cf4dad07b7f/2/2/22693-_GPS_SparkFun_RTK_Dead_Reckoning_Breakout_ZED-F9R-_01.jpg"></a></td>
  </tr>
  <tr align="center">
    <td><a href="https://www.sparkfun.com/sparkfun-allband-gnss-rtk-breakout-zed-x20p-qwiic.html">SparkFun Allband GNSS RTK Breakout - ZED-X20P (GPS-28871)</a></td>
    <td><a href="https://www.sparkfun.com/sparkfun-gps-rtk2-board-zed-f9p-qwiic-gps-15136.html">SparkFun GPS-RTK2 - ZED-F9P (GPS-15136)</a></td>
    <td><a href="https://www.sparkfun.com/sparkfun-gps-rtk-sma-breakout-zed-f9p-qwiic.html">SparkFun GPS-RTK-SMA - ZED-F9P (GPS-16481)</a></td>
    <td><a href="https://www.sparkfun.com/sparkfun-gnss-receiver-breakout-max-m10s-qwiic.html">SparkFun GNSS Receiver Breakout - MAX-M10S (GPS-18037)</a></td>
    <td><a href="https://www.sparkfun.com/sparkfun-gps-rtk-dead-reckoning-breakout-zed-f9r-qwiic-gps-22693.html">SparkFun GPS-RTK Dead Reckoning Breakout - ZED-F9R (GPS-22693)</a></td>
  </tr>
</table>

u-blox make some incredible GNSS receivers covering everything from low-cost, highly configurable modules such as the MAX-M10S all the way up to the surveyor grade ZED-X20P with precision of the diameter of a dime. This library supports configuration and control of u-blox devices over I<sup>2</sup>C (called DDC by u-blox), Serial and SPI. This version uses the u-blox Configuration Interface to: detect the module; configure message intervals; etc.. We wrote it for the most recent u-blox modules which no longer support messages like UBX-CFG-PRT or UBX-CFG-MSG.

## v4 vs. v3

This library is the new and improved version of the very popular SparkFun u-blox GNSS Arduino Library. v4 contains some big changes and improvements:

* Written by AI, directed by SparkFun
  * We used Claude to rewrite this library, giving it careful direction using v3 as the starting point
  * It was quite the journey, taking around eight working days from start to highly polished finish
  * If you want to see how we did it, the files are in the [AGENTS](./AGENTS) folder
* v4 is a fresh start
  * It avoids the repetitive coding style of v3
  * Each UBX message type is supported by its own code Class
  * Each NMEA message type is also supported by its own code Class
  * **Unneeded UBX and NMEA message classes can be commented if needed - to save both RAM and program memory**
  * Message fields (both UBX and NMEA) can be found and extracted **by name**
  * Both Polling and Periodic messages are supported - with Callbacks for Periodic messages
* v4 is **not** backward-compatible with v3
  * But we have included many helper methods to make migrating to v4 as easy as possible
  * Please see [PollingExample1_PositionVelocityTime]() for details

## Compatibility

v4 of the library provides support for generation X20, F9 and M10 u-blox GNSS modules, which support the Configuration Interface

## Repository Contents

* [**/examples**](./examples) - Example sketches for the library (.ino). Run these from the Arduino IDE.
* [**/src**](./src) - Source files for the library (.cpp, .h).
* [**keywords.txt**](./keywords.txt) - Keywords from this library that will be highlighted in the Arduino IDE.
* [**library.properties**](./library.properties) - General library properties for the Arduino package manager.
* [**/keys**](./keys) - The u-blox Configuration Interface Key IDs extracted from multiple Interface Descriptions
* [**/Utils**](./Utils) - Python utilities we wrote to help analyze UBX/NMEA/RTC data and UBX format log files

## License Information

This library is _**open source**_!

Please see [LICENSE.md](./LICENSE.md) for full details.

- Your friends at SparkFun.
