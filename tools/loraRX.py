import time
import spidev


#def SpiInit():
    # We only have SPI bus 0 available to us on the Pi
 #   bus = 0

    #Device is the chip select pin. Set to 0 or 1, depending on the connections
  #  device = 0

    # Enable SPI
   # spi = spidev.SpiDev()

    # Open a connection to a specific bus and device (chip select pin)
    #spi.open(bus, device)

    # Set SPI speed and mode
    #spi.max_speed_hz = 100000
    #spi.mode = 0



def LoraInit():
    #config LoRa
    #print("writing 0x80 (sleep, highFrq,LoRa) to reg config (0x01)")
    msg = [0x80 | 0x01, 0x80]
    result = spi.xfer2(msg)

    #high power tx mode
   # print("writing 0xFF to reg high power TX (0x09)")
    msg = [0x80 | 0x09,0xFF]
    result = spi.xfer2(msg)

    #verify Output Pin configuration
    #print("writing 0x00 to output PIN connection (0x40)")
    msg = [0x80 | 0x40, 0x00]
    result = spi.xfer2(msg)
   #print("Output Pin Config -> msg[0]: ",hex(msg[0]),"msg[1]: ",hex(msg[1]))
    print("Lora Initilized Succesfully")

def CheckMessage():
    msg = [0x00 | 0x12, 0x00]
    result = spi.xfer2(msg)
#    print("got here 1")
    if(result[1] & 0x40 == 0x40 and result[1] & 0x10 == 0x10): 
        #0x40 = Rx recived!
#        print("check message found message")
        #verify legitness
        msg = [0x00 | 0x13, 0x00]
        result = spi.xfer2(msg)
        numBytesReceived = result[1]
        if numBytesReceived > 10:
            msg = [0x80 | 0x12, 0xFF]
            result = spi.xfer2(msg)
            return False
        return True
    elif(result[1] & 0x80 == 0x80 or result[1] & 0x20 == 0x20):
        #0x80 = RX timeout
        #0x20 = CRC error
        #clear flag
        msg = [0x80 | 0x12, 0xFF]
        result = spi.xfer2(msg)
        return False
    else:
        return False
    return False


def readMessage():
    #assume 'CheckMessage()' returned true
#    print

    #clear flag
    msg = [0x80 | 0x12, 0xFF]
    result = spi.xfer2(msg)

    #verify flag has been cleared
    msg = [0x00 | 0x12, 0x00]
    result = spi.xfer2(msg)
#    print("SHOULD BE 0x00!!!: ",hex(result[1]))



    #extract data - - - 
    msg = [0x00 | 0x13, 0x00]
    result = spi.xfer2(msg)
    numBytesReceived = result[1]
    msg = [0x00 | 0x10, 0x00]
    result = spi.xfer2(msg)
    storageLocation = result[1]

    #set fifo to storage location
    msg = [0x80 | 0x0D, storageLocation]
    result = spi.xfer2(msg)

    storageArray = []
    #extract data
    for x in range(numBytesReceived):
        msg = [0x00 | 0x00, 0x00]
        result = spi.xfer2(msg)
        storageArray.append(result[1])

    #reset FIFO ptr
    msg = [0x80 | 0x0D, 0x00]
    result = spi.xfer2(msg)

    return storageArray




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

    #wait until Tx is done
    msg = [0x00 | 0x12, 0x00]
    result = spi.xfer2(msg) 
    while(result[1] & 0x08 != 0x08):
        msg = [0x00 | 0x12, 0x00]
        result = spi.xfer2(msg)

    #clear Tx flag
    msg = [0x80 | 0x12, 0x08]
    result = spi.xfer2(msg) 

    #print
    print("Sent Message: {",end =" ")
    for x in messageList:
        print(hex(x),end =" ")
    print("}")




#SpiInit()
#LoraInit()


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
print("Read Config Register: ",hex(result[1]))


print("all configured")

while True:
    #print("hello1")
    if CheckMessage() == True:
        listABC = readMessage()
        print("New Message: {",end =" ")
        for x in listABC:
            print(hex(x),end = " ")
        print("}")

#setup Tx fifoPtr
#msg = [0x80 | 0x0D, 0x80]
#result = spi.xfer2(msg)


#msgs = [0xAA, 0xBB, 0xCC, 0x44]
#LoraSendMessage(msgs, 4)