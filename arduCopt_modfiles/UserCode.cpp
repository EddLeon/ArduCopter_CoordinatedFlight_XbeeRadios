/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Copter.h"

uint8_t payload[] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64};
uint8_t shCmd[] = {'N','D'};
AtCommandRequest atRequest = AtCommandRequest(shCmd);
AtCommandResponse atResponse = AtCommandResponse();

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up
	
	hal.uartD->begin(9600,128,128);// YO LO AGREGUE
	hal.uartD->set_flow_control(AP_HAL::UARTDriver::FLOW_CONTROL_DISABLE);
	xbee.begin(hal.uartD);
	
	hal.uartE->begin(9600); 
	hal.uartE->println("Initializing Serial 4");
	delay(5000);
	
	xbee.send(atRequest);  //  send Network Discovery AT command
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
	xbee.readPacket();
	
	 if (xbee.getResponse().isAvailable()) {
      // got something
		hal.uartE->printf("ApiID Received: %x\n",xbee.getResponse().getApiId());
		
		if(xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE){
			xbee.getResponse().getAtCommandResponse(atResponse);
		
			if (atResponse.isOk()){
				if (atResponse.isOk()){
					hal.uartE->print("Command [");
					hal.uartE->printf("%c",atResponse.getCommand()[0]);
					hal.uartE->printf("%c",atResponse.getCommand()[1]);
					hal.uartE->println("] was successful!");
				}
				if (atResponse.getValueLength() > 0) {
					hal.uartE->print("Command value length is ");
					hal.uartE->printf("%d\n",atResponse.getValueLength());

					hal.uartE->print("Command value: ");
					  
					for (int i = 0; i < atResponse.getValueLength(); i++) {
						hal.uartE->printf("%x",atResponse.getValue()[i]);
						hal.uartE->print(" ");
					}
					hal.uartE->println("");
				}
			}
			else {
				hal.uartE->print("Command return error code: ");
				hal.uartE->printf("%x\n",atResponse.getStatus());
			}
		}
		else{
			hal.uartE->print("Expected AT response but got ");
			hal.uartE->println(xbee.getResponse().getApiId());
		}
	 }
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
    // put your 10Hz code here
	//Tx64Request tx64 = Tx64Request(addr64, payload, sizeof(payload));
	//xbee.send(tx64);  // sends to another xbee Hello world
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
	//0x7E00191001007D33A20040E6944EFFFE000048656C6C6F20776564656516
	//hal.uartE->println("wedee"); // YO LO AGREGUE 
	/*int frame[]={0x7E,0x00,0x19,0x10,0x01,0x00,0x7D,0x33,0xA2,0x00,
				 0x40,0xE6,0x94,0x4E,0xFF,0xFE,0x00,0x00,0x48,0x65,
				 0x6C,0x6C,0x6F,0x20,0x77,0x65,0x64,0x65,0x65,0x16};
	for(int x=0; x<30; x++)
		hal.uartE->write(frame[x]);
	*/
	
	/*uint8_t payload[] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64};
	Tx64Request tx64 = Tx64Request(addr64, payload, sizeof(payload));
	xbee.send(tx64);
	*/
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
	//hal.uartE->println("hello"); // YO LO AGREGUE 
	
	uint8_t frame[]={0x7E,0x00,0x19,0x10,0x01,0x00,0x7D,0x33,0xA2,0x00,
				 0x40,0xE6,0x94,0x4E,0xFF,0xFE,0x00,0x00,0x48,0x65,
				 0x6C,0x6C,0x6F,0x20,0x77,0x65,0x64,0x65,0x65,0x16};
	//for(int x=0; x<30; x++)
	//	hal.uartE->write(frame[x]); 
	//hal.uartD->write(frame,sizeof(frame)); 

	/*
	uint8_t payload[] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64};
	Tx64Request tx64 = Tx64Request(addr64, payload, sizeof(payload));
	xbee.send(tx64);
	*/
}
#endif
