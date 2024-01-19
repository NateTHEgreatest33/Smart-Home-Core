# Module Base

Module_base is a fully customizable template for using the suite of utlities and messaging formats developed by myself. It is written with in style and intent of DO-178, with requirements, tests, and code all tracing to each other!

Modules included†
- **MessageAPI**: A full messaging layer built upon loraAPI
- **loraAPI**: low level driver for interfacing with the RFM95 Tranciver.
- **ConsoleAPI**: a full console application including logging 
- **UtilLib**: a group of common functions written using RAII principles built upon pico SDK.
- **buttonComponent**: a simple interface for adding push buttons to your system

†**Please Note**: not all modules are supported on all hardware variations, but we hope to bring full support! 

## Features

- **Modularity**: Use what you want, don't what you don't need!
- **Configuration**: Simple configuration setup to add new modules to your system!
- **Requirements**: Requirements writen and testing in DO-178B style for a robust system.
- **Testing**: System testing framework supported in python and constantly being expanded!
- **Compatibility**: Compatible with both Pi Pico, Raspberry Pi 3+, and Tiva Launchpad

## Hardware list 
- https://www.adafruit.com/product/3072
- https://www.raspberrypi.org/products/raspberry-pi-3-model-b-plus/
- https://www.ti.com/tool/EK-TM4C123GXL
- https://www.raspberrypi.com/products/raspberry-pi-pico/
 
## Requirements
[High Level Requirements](https://smrt-home.atlassian.net/wiki/spaces/HOS/pages/7634976/High+Level+Requirements)

[Low Level Requirements](https://smrt-home.atlassian.net/wiki/spaces/HOS/pages/8126465/Low+Level+Requirements)

## Quick Start

1. within lib/messageAPI/sys_def.h add your module to the following enum

```c++
enum 
    {
    RPI_MODULE,                    /* raspberry pi module           */
    TIVA_MODULE,                   /* tiva launchpad module         */
    PICO_MODULE,                   /* pi pico module                */
              **[ADD MODULE HERE]**

    NUM_OF_MODULES,                /* number of modules             */
    
    MODULE_NONE,                   /* no module                     */
    INVALID_LOCATION               /* invalid module address        */
    }; 
```

2. within main, redefine current_location to the location you just defined!

```c++
/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/
const location current_location = NEW_MODULE;
```

3. Load your software onto the board and run the system test to make sure everything is working!


## Contributing

Contributions are welcome!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Pi Pico SDK, for 
