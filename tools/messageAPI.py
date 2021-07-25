import time
import spidev

class messageAPI:

	def __init__(self, bus, chip_select, currentModule, listOfModules):
		#setup local var's
		self.currentModule = currentModule
		self.listOfModules = listOfModules

		# Enable SPI
		self.spi = spidev.SpiDev()
		self.spi.open(bus, chip_select)
		self.spi.max_speed_hz = 100000
		self.spi.mode = 0

	#public functions
	def messageAPI_init(self):
		return 0

	def TXMessage(self):
		return 0

	def RXMessage(self):
		return 0	

	def updateKey(self):
		return 0	

	#private functions
	def updateCRC(self):
		return 0

	def LoraInit(self):
	    #config LoRa
	    msg = [0x80 | 0x01, 0x80]
	    result = self.spi.xfer2(msg)

	    msg = [0x80 | 0x09,0xFF]
	    result = self.spi.xfer2(msg)

	    msg = [0x80 | 0x40, 0x00]
	    result = self.spi.xfer2(msg)


	def LoraCheckMessage(self):
	    msg = [0x00 | 0x12, 0x00]
	    result = self.spi.xfer2(msg)

	    if(result[1] & 0x40 == 0x40 and result[1] & 0x10 == 0x10): 

	        msg = [0x00 | 0x13, 0x00]
	        result = self.spi.xfer2(msg)
	        numBytesReceived = result[1]

	        if numBytesReceived > 10:
	            msg = [0x80 | 0x12, 0xFF]
	            result = self.spi.xfer2(msg)
	            return False
	        return True

	    elif(result[1] & 0x80 == 0x80 or result[1] & 0x20 == 0x20):
	        #0x80 = RX timeout
	        #0x20 = CRC error
	        #clear flag
	        msg = [0x80 | 0x12, 0xFF]
	        result = self.spi.xfer2(msg)
	        return False

	    else:
	        return False

	    return False

	def LoraReadMessage(self):

	    #clear flag
	    msg = [0x80 | 0x12, 0xFF]
	    result = self.spi.xfer2(msg)

	    #verify flag has been cleared
	    msg = [0x00 | 0x12, 0x00]
	    result = self.spi.xfer2(msg)

	    #extract data - - - 
	    msg = [0x00 | 0x13, 0x00]
	    result = self.spi.xfer2(msg)
	    numBytesReceived = result[1]
	    msg = [0x00 | 0x10, 0x00]
	    result = self.spi.xfer2(msg)
	    storageLocation = result[1]

	    #set fifo to storage location
	    msg = [0x80 | 0x0D, storageLocation]
	    result = self.spi.xfer2(msg)

	    #extract data
	    storageArray = []
	    for x in range(numBytesReceived):
	        msg = [0x00 | 0x00, 0x00]
	        result = self.spi.xfer2(msg)
	        storageArray.append(result[1])

	    #reset FIFO ptr
	    msg = [0x80 | 0x0D, 0x00]
	    result = self.spi.xfer2(msg)

	    return storageArray


	def LoraSetRxMode(self):
	    msg = [0x80 | 0x01,0x80]
	    result = self.spi.xfer2(msg)

	    #setup Rx fifo
	    msg = [0x80 | 0x0D,0x00]
	    result = self.spi.xfer2(msg)

	    #set into Rx Continous Mode
	    msg = [0x80 | 0x01,0x85]
	    result = self.spi.xfer2(msg)


	def LoraSendMessage( messageList, messageSize):
	    msg = [0x80 | 0x01, 0x81]
	    result = self.spi.xfer2(msg)

	    msg = [0x80 | 0x0D,0x80]
	    result = self.spi.xfer2(msg)

	    #fillFifo
	    for x in messageList:
	        msg = [0x80 | 0x00,x]
	        result = self.spi.xfer2(msg)

	    msg = [0x80 | 0x22,messageSize]
	    result = self.spi.xfer2(msg)

	    msg = [0x80 | 0x01, 0x83]
	    result = self.spi.xfer2(msg)

	    #wait until Tx is done
	    msg = [0x00 | 0x12, 0x00]
	    result = self.spi.xfer2(msg) 
	    while(result[1] & 0x08 != 0x08):
	        msg = [0x00 | 0x12, 0x00]
	        result = self.spi.xfer2(msg)

	    #clear Tx flag
	    msg = [0x80 | 0x12, 0x08]
	    result = self.spi.xfer2(msg) 

	    #print
	    print("Sent Message: {",end =" ")
	    for x in messageList:
	        print(hex(x),end =" ")
	    print("}")




#Test API
module = messageAPI(0, 0, 0x01, [0x00,0x01])
module.LoraInit()
module.LoraSetRxMode()


while True:

    if module.LoraCheckMessage() == True:
        return_msg = module.LoraReadMessage()
        print("New Message: {",end =" ")
        for x in return_msg:
            print(hex(x),end = " ")
        print("}")

