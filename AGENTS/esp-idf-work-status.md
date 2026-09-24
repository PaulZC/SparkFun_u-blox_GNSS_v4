# ESP-IDF component work: status

Repo: C:\Users\pc235\Documents\GitHub\SparkFun_u-blox_GNSS_v4 (PaulZC fork), branch `esp-idf-component`, based on 08943c2.
Paul does all commits, builds (ESP-IDF + Arduino IDE) and hardware tests; he pastes the results into the chat.
Registry name: `sparkfun/sparkfun_u-blox_gnss_v4`. Approach: option 2b (native ESP-IDF bus back end inside v4). See the report.
A copy of this file is kept in the project as claude/esp-idf-work-status.md.

## Done (24 Sep 2026): Phases 1–3 + Phase 4 example conversion
- New: src/sfe_platform.h (sfe_millis/micros/delay, sfe_pin_*, sfe_string_t, sfe_string_from_double, DEC/HEX)
- New: src/sfe_bus_esp_idf.h/.cpp (SfeI2C on i2c_master, SfeSPI on spi_master with manual CS, SfeSerial on uart; SfeOutput/SfeStdoutOutput/SfeUartOutput/SfeCallbackOutput; SfePrint; sfeStdout)
- sfe_bus.h/.cpp: Arduino classes wrapped in #if SFE_ARDUINO; typedef sfe_print_t (Print on Arduino, SfeOutput on IDF)
- u-blox_GNSS.h/.cpp, nmeaMessage.h, nmeaMessageVector.h, sfe_debug.h, ubxMessage.h: includes → sfe_platform.h; millis/delay/pinMode → sfe_*; String → sfe_string_t; Print& → sfe_print_t&; ESP-IDF enableDebugging default = sfeStdout; one strstr `char *ptr` → `const char *ptr`
- SparkFun_u-blox_GNSS_v4.h: ESP-IDF begin() overloads: I2C(bus, addr), SPI(host, cs, speed) / (spi_device_handle_t, cs), UART(uart_port_t); plus the SUPER class
- Root: CMakeLists.txt, idf_component.yml (idf >=5.3), Kconfig (I2C clock, I2C timeout)
- idf_examples/PollingExample1 (I2C), 2 (UART), 3 (SPI): main.cpp, Kconfig.projbuild pins, sdkconfig.defaults (FREERTOS_HZ=1000)
- README: ESP-IDF section; .gitignore: build outputs; idf_component.yml excludes *Dockerfile, *.bin, *.elf, *.bat

## Build results (24 Sep 2026)
- Arduino (arduino-cli, esp32:esp32 3.3.11, Arduino.Dockerfile): PollingExample1 compiles. Fixed: String(double, int) overload ambiguity in sfe_platform.h
- ESP-IDF v6.1 (GCC 15.2, -Werror, IDF.Dockerfile): PollingExample1 compiles and links. Fixed: Docker WORKDIR /${COMPONENT} (component dir name was empty); uart_port_t is an enum in IDF v6 (_port{UART_NUM_0})
- IDF_compile_example.bat args: COMPONENT EXAMPLE
- Hardware: Arduino PollingExample1 runs correctly on ESP32 + u-blox over I2C (Arduino regression OK)
- Hardware: ESP-IDF v6.1 PollingExample1 runs correctly on ESP32 (rev v3.0) + u-blox over I2C. PHASE 4 SUCCESS INDICATOR MET (24 Sep 2026)
- Hardware: ESP-IDF enableDebugging() output works (stdout sink, HEX formatting). NAV-PVT polls take 525–1025 ms with "checkUbloxI2C: 2856 bytes available" each poll
- Hardware: Arduino debug output matches: ~2732 bytes available per poll, NAV-PVT polls 915–1020 ms. ESP-IDF timing is equivalent to Arduino (polled replies wait for the next 1 Hz epoch; the module also outputs periodic data on I2C)
- Hardware: ESP-IDF PollingExample2 (UART1, 38400 baud) runs correctly with debug enabled. getVal uses the UART1 key (0x10730001); NAV-PVT polls 307–979 ms
- Example 3 SPI CS default changed to GPIO 4 (matches the Arduino example)
- Hardware: ESP-IDF PollingExample3 (SPI2_HOST, CS GPIO 4, 4 MHz) runs correctly with debug enabled. getVal uses the SPI key (0x10790001); spiBuffer 308 bytes; NAV-PVT polls 794–1005 ms
- ALL THREE BUSES (I2C, UART, SPI) CONFIRMED ON HARDWARE WITH ESP-IDF v6.1 (24 Sep 2026)
- Modules: I2C and UART tests used a ZED-X20P; the SPI test used an older ZED-F9P with D_SEL set for SPI (explains 18 vs 31 SVs and no diffSoln)
- Converted (24 Sep 2026): PeriodicExample1_NAVHPPOSLLH, PeriodicExample2_GPGGA, CallbackExample1_NAVHPPOSLLH, CallbackExample2_GPRMC (all I2C; added to idf_component.yml examples list)
- Hardware: ESP-IDF PeriodicExample1_NAVHPPOSLLH runs correctly (debug enabled): setAutoUBX CFG-VALSET ACKed; HPPOSLLH arrives at 1 Hz; printed fields match payload bytes; progress dots appear live (fflush OK)
- Hardware: ESP-IDF PeriodicExample2_GPGGA runs correctly: NMEA field getters return std::string; DDMM→degrees via sfe_string_from_double gives 8 dp (54.86654750); UTC time advances 1 s per GGA. NAV-HPPOSLLH still arriving because PeriodicExample1 enabled it in RAM+BBR
- Hardware: ESP-IDF CallbackExample1_NAVHPPOSLLH runs correctly: setCfgValset ACKed; static callback fires once per HPPOSLLH (1 Hz) via checkUblox()/checkCallbacks(); printed fields match payload bytes
- Hardware: ESP-IDF CallbackExample2_GPRMC runs correctly: NMEA callback fires once per RMC; std::string fields (time, date 240926, NS, EW) and DDMM→degrees OK. All 7 converted ESP-IDF examples now run on hardware
- Converted (24 Sep 2026; builds OK on ESP-IDF v6.1, 334 KB): DataloggingExample1_RAWX_and_SFRBX. SD card via ESP-IDF FATFS VFS (esp_vfs_fat_sdspi_mount) in main/sd_card.c (C, because of the sdmmc/sdspi C initializer macros); fopen/fwrite/fclose; key press via non-blocking stdin (fcntl O_NONBLOCK + fgetc); freeze() uses vTaskDelay; file opened with "a" (append, as the Arduino comment says; Arduino-ESP32 FILE_WRITE is actually "w")
- Hardware: ESP-IDF DataloggingExample1_RAWX_and_SFRBX STRESS TEST PASSED (24 Sep 2026, ZED-X20P, I2C, transaction size 32, Tera Term): ~193 s logging, 539,720 bytes to a 32 GB SDHC card. UBX_Integrity_Checker: 96 RAWX + 3596 SFRBX, no checksum failures, longest message 3672 bytes; counts match the callback counters exactly. No 80%-full buffer warning; enableDebugging(sfeStdout, true) printed no important errors. Key press via non-blocking stdin works. Benign IDF warning at unmount: "W gpio: conflict found for GPIO[5]" (SD CS)
- Not yet: remaining 15 examples

## Conventions for the ESP-IDF examples
- I2C: `busConfig.flags.enable_internal_pullup = false; // u-blox modules have their own internal active pull-ups` (Paul, 24 Sep 2026: extra pull-ups have caused I2C problems with u-blox modules). Tested OK on PollingExample1
- Example conversion: Serial.print → printf (%s with .c_str() for NMEA string fields); the loop's "." progress dots use fflush(stdout) because stdout is line-buffered

## Next
- Paul: commit (comment out enableDebugging() in any examples first)
- Claude: convert the remaining 15 examples
- Then: optional i2cTransactionSize tuning (32 is sufficient for RAWX+SFRBX), CI, registry
- Deferred: new(std::nothrow); FreeRTOS lock option; Arduino-as-component path in CMakeLists (untested)
