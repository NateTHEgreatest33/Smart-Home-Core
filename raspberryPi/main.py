from messageAPI import messageAPI


#Test API
module = messageAPI( bus=0, chip_select=0, currentModule=0x00, listOfModules=[0x00,0x01] )
module.InitAPI()
module.TXMessage( message=[0x11,0x22,0x33], destination=0x01 )

while True:
	return_tpl = module.RXMessage()
	if return_tpl[0] == True:
		print("source:", return_tpl[1], "data recvied: ", return_tpl[2]," valid: ", return_tpl[3] )