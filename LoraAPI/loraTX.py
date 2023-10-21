import time
import spidev

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
