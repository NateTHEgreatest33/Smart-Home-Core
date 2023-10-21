from LoraAPI.loraRX     import *
from LoraAPI.loraTX     import *
from LoraAPI.loraCommon import *

import time
import spidev

def main():
    # We only have SPI bus 0 available to us on the Pi
    bus = 0

    #Device is the chip select pin. Set to 0 or 1, depending on the connections
    device = 0

    # Enable SPI
    spi = spidev.SpiDev()

    # Open a connection to a specific bus and device (chip select pin)
    spi.open(bus, device)

    # Set SPI speed and mode
    spi.max_speed_hz = 100000
    spi.mode = 0

    print("SPI initialized successfully")

    LoraInit()
    setRxMode()

    msg = [0x00 | 0x01,0x00]
    result = spi.xfer2(msg)
    print( "Read Config Register: ", hex(result[1]) )


    print("all configured")

    while True:
        if CheckMessage() == True:
            listABC = readMessage()
            print("New Message: {",end =" ")
            for x in listABC:
                print(hex(x),end = " ")
            print("}")







if __name__ == "__main__":
    main()