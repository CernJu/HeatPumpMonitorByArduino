
HeatPumpMonitor-Arduino
=======================

(C)2015-2019 

Common mobile AC has been modified for heating small water pool... embedded temperature sensors, powermeter, flowmeter are controled by CU based on arduino platform. The performance of that heat pump is monitored on android App via bluetooth.

See 

* [Freematics OBD-II UART Adapter](https://freematics.com/pages/products/freematics-obd-ii-uart-adapter-mk2/)
* [Freematics OBD-II I2C Adapter](https://freematics.com/pages/products/arduino-obd-adapter/)
* [OBD-II Telematics Mega Kit](https://freematics.com/pages/products/arduino-telematics-kit-3/)

![Image](http://www.arduinodev.com/wp-content/uploads/2012/03/obdkit1-150x150.jpg)
<img src="https://raw.githubusercontent.com/DennisHartrampf/DennisHartrampf/master/img/header.jpg"

About the library
-----------------
Most commonly use PIDs are defined in OBD library as followings.

Engine

    PID_RPM – Engine RPM (rpm)
    PID_ENGINE_LOAD – Calculated engine load (%)
    PID_COOLANT_TEMP – Engine coolant temperature (°C)
    PID_ABSOLUTE_ENGINE_LOAD – Absolute Engine load (%)
    PID_TIMING_ADVANCE – Ignition timing advance (°)
    PID_ENGINE_OIL_TEMP – Engine oil temperature (°C)
    PID_ENGINE_TORQUE_PERCENTAGE – Engine torque percentage (%)
    PID_ENGINE_REF_TORQUE – Engine reference torque (Nm)

Intake/Exhaust
