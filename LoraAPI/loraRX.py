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


