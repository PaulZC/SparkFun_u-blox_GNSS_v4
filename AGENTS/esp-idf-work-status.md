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
- Not yet: PollingExample2/3 builds and hardware; RAWX stress test

## Next
- Paul: build and run PollingExample2 (UART) and PollingExample3 (SPI)
- Then: stress test (RAWX), tune i2cTransactionSize (still 32), convert the remaining 20 examples, CI, registry
- Deferred: new(std::nothrow); FreeRTOS lock option; Arduino-as-component path in CMakeLists (untested)
