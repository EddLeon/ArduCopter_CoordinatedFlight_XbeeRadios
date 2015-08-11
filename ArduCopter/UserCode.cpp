/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Copter.h"

#ifdef USERHOOK_INIT
uint8_t payload[] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64};
uint8_t shCmd[] = {'N','D'};
//broadcast helloworld -> 7E 00 12 10 01 00 00 00 00 00 00 FF FF FF FE 00 00 57 45 44 45 CE
AtCommandRequest atRequest = AtCommandRequest(shCmd);
AtCommandResponse atResponse = AtCommandResponse();
ZBRxResponse rxResponse = ZBRxResponse();
XBeeAddress64 rxRespAddr64;
DroneTable droneTable = DroneTable();
Drone* droneAux;
Controller controller = Controller();

void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up
	
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
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
}
#endif