## Background

This is an Arduino Library, written by SparkFun, to communicate with u-blox GNSS modules.
It supports communication over I2C, UART (Serial), and SPI.
The top level class instances are: ```SFE_UBLOX_GNSS``` for I2C, ```SFE_UBLOX_GNSS_SERIAL``` for UART, and ```SFE_UBLOX_GNSS_SPI``` for SPI.
The hardware physical interface is defined in sfe_bus.h and sfe_bus.cpp.

SparkFun_u-blox_GNSS_v4 is to be an updated version of [SparkFun_u-blox_GNSS_v3](https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3)

u-blox GNSS modules use a binary communication protocol called "UBX".
They also support "NMEA" - defined by the National Marine Electronics Association standard NMEA 0183: Standard For Interfacing Marine Electronic Devices.
They also support "RTCM" - defined by the Radio Technical Commission for Maritime Services standard RTCM STANDARD 10403.3: Differential GNSS Services - Version 3.

## Important References

[u-blox HPG 2.11 Interface description](https://www.u-blox.com/sites/default/files/documents/u-blox-X20-HPG-2.11_InterfaceDescription_UBXDOC-304424225-21617.pdf)
[ZED-X20P-01B Data sheet](https://www.u-blox.com/sites/default/files/documents/ZED-X20P-01B_DataSheet_UBXDOC-304424225-21281.pdf)
[ZED-X20P Integration manual](https://www.u-blox.com/sites/default/files/documents/ZED-X20P_IntegrationManual_UBXDOC-963802114-12901.pdf)

National Marine Electronics Association standard NMEA 0183 is a copyright document. A copy will be provided to you when you request it in a chat session.

Radio Technical Commission for Maritime Services standard RTCM STANDARD 10403.3 is a copyright document. A copy will be provided to you when you request it in a chat session.

## The Problem

The coding style in SparkFun_u-blox_GNSS_v3 needs improvement.
The code contains many repetitions of the same or similar code blocks for each UBX message.
In the [v3 Library notes for Adding_New_Messages](https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3/blob/main/Adding_New_Messages.md) you can see how the code to support each message is added to ```end()```, ```autoLookup()```, ```processUBXpacket()```, ```checkCallbacks()```, etc.
The key objective for v4 is to avoid this code repetition.

## The Objective

The objective is to re-write the existing code from the v3 library, to create the new v4 library.

The top level class instances ```SFE_UBLOX_GNSS```, ```SFE_UBLOX_GNSS_SERIAL``` and ```SFE_UBLOX_GNSS_SPI``` are to be retained.

The hardware physical interface defined in sfe_bus.h and sfe_bus.cpp is to be retained.

The u-blox UBX Class and ID definitions in u-blox_Class_and_ID.h are to be retained.

The examples in the examples folder are to be retained and not modified.

## Implement each UBX message in its own Class

For each UBX message supported by the original v3 library, implement support for that message in a separate Class.

E.g.: for UBX-NAV-PVT, add a file called ubxNAVPVT.h containing:

```
// UBX-NAV-PVT (0x01 0x07): Navigation position velocity time solution
class ubxNAVPVT : public ubxMessage
{
public:
    const uint8_t Class = UBX_CLASS_NAV;
    const uint8_t ID = UBX_NAV_PVT;

    const char classStr[4] = "NAV"; // The message Class - from the interface description
    const char idStr[4] = "PVT";    // The message ID. Increase the array size if needed

    // These are the configuration keys needed to enable this message on I2C, SPI, UART1, UART2
    const uint32_t msgOutKeys[4] = {UBLOX_CFG_MSGOUT_UBX_NAV_PVT_I2C, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_SPI,
                                    UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART1, UBLOX_CFG_MSGOUT_UBX_NAV_PVT_UART2};

    const ubxField ubxFields[] = {
        {"iTOW", ubxDataType8bit(UBX_CFG_U4), 0, -1, -1},
        {"year", ubxDataType8bit(UBX_CFG_U2), 4, -1, -1},
        {"month", ubxDataType8bit(UBX_CFG_U1), 6, -1, -1},
        {"day", ubxDataType8bit(UBX_CFG_U1), 7, -1, -1},
        {"hour", ubxDataType8bit(UBX_CFG_U1), 8, -1, -1},
        {"min", ubxDataType8bit(UBX_CFG_U1), 9, -1, -1},
        {"sec", ubxDataType8bit(UBX_CFG_U1), 10, -1, -1},
        {"valid", ubxDataType8bit(UBX_CFG_X1), 11, -1, -1},
        {"validDate", ubxDataType8bit(UBX_CFG_L), 11, 0, 1},
        {"validTime", ubxDataType8bit(UBX_CFG_L), 11, 1, 1},
        {"fullyResolved", ubxDataType8bit(UBX_CFG_L), 11, 2, 1},
        {"validMag", ubxDataType8bit(UBX_CFG_L), 11, 3, 1},
        {"tAcc", ubxDataType8bit(UBX_CFG_U4), 12, -1, -1},
        {"nano", ubxDataType8bit(UBX_CFG_I4), 16, -1, -1},
        {"fixType", ubxDataType8bit(UBX_CFG_U1), 20, -1, -1},
        {"flags", ubxDataType8bit(UBX_CFG_X1), 21, -1, -1},
        {"gnssFixOK", ubxDataType8bit(UBX_CFG_L), 21, 0, 1},
        {"diffSoln", ubxDataType8bit(UBX_CFG_L), 21, 1, 1},
        {"psmState", ubxDataType8bit(UBX_CFG_U1), 21, 2, 3},
        {"headVehValid", ubxDataType8bit(UBX_CFG_L), 21, 5, 1},
        {"carrSoln", ubxDataType8bit(UBX_CFG_U1), 21, 6, 2},
        {"flags2", ubxDataType8bit(UBX_CFG_X1), 22, -1, -1},
        {"confirmedAvai", ubxDataType8bit(UBX_CFG_L), 22, 5, 1},
        {"confirmedDate", ubxDataType8bit(UBX_CFG_L), 22, 6, 1},
        {"confirmedTime", ubxDataType8bit(UBX_CFG_L), 22, 7, 1},
        {"numSV", ubxDataType8bit(UBX_CFG_U1), 23, -1, -1},
        {"lon", ubxDataType8bit(UBX_CFG_I4), 24, -1, -1},
        {"lat", ubxDataType8bit(UBX_CFG_I4), 28, -1, -1},
        {"height", ubxDataType8bit(UBX_CFG_I4), 32, -1, -1},
        {"hMSL", ubxDataType8bit(UBX_CFG_I4), 36, -1, -1},
        {"hAcc", ubxDataType8bit(UBX_CFG_U4), 40, -1, -1},
        {"vAcc", ubxDataType8bit(UBX_CFG_U4), 44, -1, -1},
        {"velN", ubxDataType8bit(UBX_CFG_I4), 48, -1, -1},
        {"velE", ubxDataType8bit(UBX_CFG_I4), 52, -1, -1},
        {"velD", ubxDataType8bit(UBX_CFG_I4), 56, -1, -1},
        {"gSpeed", ubxDataType8bit(UBX_CFG_I4), 60, -1, -1},
        {"headMot", ubxDataType8bit(UBX_CFG_I4), 64, -1, -1},
        {"sAcc", ubxDataType8bit(UBX_CFG_U4), 68, -1, -1},
        {"headAcc", ubxDataType8bit(UBX_CFG_U4), 72, -1, -1},
        {"pDOP", ubxDataType8bit(UBX_CFG_U2), 76, -1, -1},
        {"flags3", ubxDataType8bit(UBX_CFG_X2), 78, -1, -1},
        {"invalidLlh", ubxDataType8bit(UBX_CFG_L), 78, 0, 1},
        {"lastCorrectionAge", ubxDataType8bit(UBX_CFG_U1), 78, 1, 4},
        {"headVeh", ubxDataType8bit(UBX_CFG_I4), 84, -1, -1},
        {"magDec", ubxDataType8bit(UBX_CFG_I2), 88, -1, -1},
        {"magAcc", ubxDataType8bit(UBX_CFG_U2), 90, -1, -1}
    };
};
```

The ```ubxField``` is defined in ```ubxMessage``` and could be something like:

```
    // Definition of the data type to hold a single UBX message field
    typedef struct
    {
        const char *fieldName;  // The field name - taken from the u-blox interface description
        const uint8_t ubxDataType; // The UBX data type L,U1,U2 etc. in 8-bit form from the macro ubxDataType8bit
        const uint8_t startByte;   // The position of the field in the UBX message payload. (Add 6 when extracting)
        const int8_t startBit;     // The bit position if this is a bit field. -1 otherwise
        const int8_t bitWidth;     // The bit width if this is a bit field. -1 otherwise
    } ubxField;
```

```ubxNAVPVT``` is to be a singleton, which is automatically instantiated on execution.

If the user wishes to save memory, they can delete ubxNAVPVT.h. The remainder of the code will compile, but will not provide support for NAV PVT.

```getPVT()```, ```getLatitude()```, ```getLongitude()``` and ```getAltitudeMSL()``` are simple helper methods defined within the parent ```class DevUBLOXGNSS```.
The code for ```getLatitude()``` could be:

```
    // Get the current latitude in degrees
    // Returns a long representing the number of degrees *10^-7
    int32_t DevUBLOXGNSS::getLatitude(uint16_t maxWait)
    {
        ubxAnyType *value;
        if (!getUBXfield(UBX_CLASS_NAV, UBX_NAV_PVT, "lat", ubxAnyType * value, uint16_t maxWait))
            return 0;
        return value.I4;
    }
```

By default, ```getPVT()``` will Poll the NAV-PVT message. The code for ```getPVT()``` could be:

```
bool DevUBLOXGNSS::getPVT(uint16_t maxWait)
{
    return getUBX(UBX_CLASS_NAV, UBX_NAV_PVT, maxWait);
}
```

The ```getUBX()``` method is to be provided by the ```class DevUBLOXGNSS```. It is a generic method which:

* Creates storage to hold the full UBX message
    * The storage is to be created using a ```virtual``` ```alloc``` method
* Understands if the message is Periodic (has had a message rate set), or whether it should be Polled (requested once)
* If the NAV-PVT message needs to be Polled (requested)
    * The library should send a UBX message containing:
        * UBX Class ```UBX_CLASS_NAV```
        * UBX ID ```UBX_NAV_PVT```
        * Length zero
    * Wait for up to ```maxWait``` milliseconds for the message to arrive, calling ```checkUbloxInternal()``` every few 10s of millisconds
    * If the NAV-PVT message is received within ```maxWait```, it is stored in the allocated storage
    * ```getPVT()``` returns ```true``` if a NAV-PVT message was received within ```maxWait```, ```false``` otherwise
    * ```getPVT()``` is blocking in this case. It will wait / stall for the full ```maxWait``` if needed
* If the NAV-PVT message is periodic:
    * ```getUBX()``` will call the method ```checkUbloxInternal()``` to check for the availablelity of new I2C/UART/SPI data
    * If a new NAV-PVT message has arrived, it is stored in the allocated storage
    * ```getPVT()``` returns true if ```checkUbloxInternal()``` provided a new NAV-PVT message, ```false``` otherwise.
    * ```getPVT()``` is non-blocking in this case. After calling ```checkUbloxInternal()``` once, it returns ```true``` or ```false``` immediately. It does not wait for ```maxWait``` milliseconds.

## Test

Compile the example code in examples/Example1_PositionVelocityTime using the batch file compile_example.bat.
Parse the Docker (Dockerfile) output. Check that the example compiles successfully. 



