# QtRvi
Qt Library for Remote Vehicle Interaction

Purpose:
----------------
Qt wrapper to create an event driven interface for an application developer to easily integrate GENIVI's RVI protocol into C++ and/or QML applications.

Copyright and Authors:
----------------
(C) 2017 Jaguar Land Rover - All rights reserved.

Jack Sanchez - <jsanche4@jaguarlandrover.com>

Tatiana Jamison - <tjamison@jaguarlandrover.com>

This program is licensed under the terms and conditions of the
Mozilla Public License, version 2.0. The full text of the
Mozilla Public License is at https://www.mozilla.org/MPL/2.0/


Dependencies:
---------------
Minimum tested Qt version is currently 5.6.2, it is likely the module can be easily backported to older versions.

Requires rvi_lib <https://github.com/GENIVI/rvi_lib> to be installed

Also requires a C++11 compliant compiler


Features:
--------------
* QRviNode (C++ and QML) for creating connections to RVI nodes
* QRviServiceInterface (C++ and QML) for registering services with your QRviNode


Building:
---------------
Currently QtRvi expects for two environment variables to be defined which point to the installation location path of your rvi_lib:
```
RVI_INCLUDE_PATH
RVI_LIB_PATH
```

Then this module will follow basic Qt module build steps:
```
qmake
make
make install
```

The module will be deployed to the location of the Qt SDK referenced by your qmake used during the build phase.

Examples:
---------------
Note: Due to the current limitation of rvi_lib, a given service cannot invoke another service which is registered on the same QRviNode.

Ex 1)

rviserviceping:
* This example intends to demonstrate the minimal amount of QML required to implement an RVI service.
* Functions as the user application to instigate behavior on the rviservicepong example application.

Ex 2)

rviservicepong:
* This example intends to demonstrate the minimal amount of C++ required to implement an Rvi Service.
* Functions as a command line application waiting for input from the rviserviceping user application.
