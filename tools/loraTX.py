import time
import spidev

def LoraInit():
    #config LoRa
    #print("writing 0x80 (sleep, highFrq,LoRa) to reg config (0x01)")
    msg = [0x80 | 0x01, 0x80]
    result = spi.xfer2(msg)
    print(result)
    #high power tx mode
   # print("writing 0xFF to reg high power TX (0x09)")
    msg = [0x80 | 0x09,0xFF]
    result = spi.xfer2(msg)
    print(result)
    #verify Output Pin configuration
    #print("writing 0x00 to output PIN connection (0x40)")
    msg = [0x80 | 0x40, 0x00]
    result = spi.xfer2(msg)
   #print("Output Pin Config -> msg[0]: ",hex(msg[0]),"msg[1]: ",hex(msg[1]))
    print("Lora Initilized Succesfully")
    print(result)

def setRxMode():
    #print("writing SLEEP mode (0x81) to config register (0x01)")
    msg = [0x80 | 0x01,0x80]
    result = spi.xfer2(msg)

    #setup Rx fifo
    msg = [0x80 | 0x0D,0x00]
    result = spi.xfer2(msg)

    #set into Rx Continous Mode
    msg = [0x80 | 0x01,0x85]
    result = spi.xfer2(msg)


def LoraSendMessage( messageList, messageSize):
    #print("writing STBY mode (0x81) to config register (0x01)")
    msg = [0x80 | 0x01, 0x81]
    result = spi.xfer2(msg)

    #print("writing Tx_fifoPtr (0x80) to FifoPtr_address (0x0D)")
    msg = [0x80 | 0x0D,0x80]
    result = spi.xfer2(msg)

    #fillFifo
    for x in messageList:
        msg = [0x80 | 0x00,x]
        result = spi.xfer2(msg)
    #print("set payload length (in our case 0x03) to register (0x22)")
    msg = [0x80 | 0x22,messageSize]
    result = spi.xfer2(msg)

    #print("Put tranciver into TX mode")
    msg = [0x80 | 0x01, 0x83]
    result = spi.xfer2(msg)

    #print
    print("Sent Message: {",end =" ")
    for x in messageList:
        print(hex(x),end =" ")
    print("}")

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

#setup Tx fifoPtr
msg = [0x80 | 0x0D, 0x80]
result = spi.xfer2(msg)


#msgs = [0xAA, 0xBB, 0xCC, 0x44]
#LoraSendMessage(msgs, 4)

#       dest, src,  pad, ver/size key  data  data  data  data  crc
msgs = [0x01, 0x00, 0x00,    0x14, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0x45 ]
LoraSendMessage(msgs, 10)