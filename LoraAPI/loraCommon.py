import time
import spidev

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