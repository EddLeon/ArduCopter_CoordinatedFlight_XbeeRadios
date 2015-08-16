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
	
	hal.uartD->begin(9600,128,128);
	hal.uartD->set_flow_control(AP_HAL::UARTDriver::FLOW_CONTROL_DISABLE);
	xbee.begin(hal.uartD);
	
	hal.uartE->begin(9600); 
	hal.uartE->println("*** Initializing Serial 4...");
	hal.uartE->printf("a) Report Table\nb) Send ND packet\nc) Clear Table\nd) Send EncDrone\n");
	hal.uartE->println(". . . . . . . . . . . . . . . . . . \n");
	delay(1000);
	
	hal.uartE->println("-- Sending AT-ND --");
	xbee.send(atRequest);  //  send Network Discovery AT command
	hal.uartE->println("\tDONE..");
	
	if(droneTable.isempty())
		hal.uartE->printf("Table empty : TRUE\n");
	else hal.uartE->printf("Table empty : FALSE\n");
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
	xbee.readPacket();
	
	 if (xbee.getResponse().isAvailable()) {
      // got something
		//hal.uartE->printf("ApiID Received: %x\n",xbee.getResponse().getApiId());
		
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
					
					if(atResponse.getCommand()[0] == 'N' && atResponse.getCommand()[1] == 'D'){ // Network Discovery
						userhook_netDiscoveryResponse();
					}
				}
			}
			else {
				hal.uartE->print("Command return error code: ");
				hal.uartE->printf("%x\n",atResponse.getStatus());
			}
		}
		else if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
			xbee.getResponse().getZBRxResponse(rxResponse);
			rxRespAddr64 = rxResponse.getRemoteAddress64();
			/* hal.uartE->printf("MAC : %x %x\n",rxRespAddr64.getMsb(),rxRespAddr64.getLsb());
			hal.uartE->println("Message : ");
			for (int i = 0; i < rxResponse.getDataLength(); i++) {
				hal.uartE->printf("%x ",rxResponse.getData(i));
				hal.uartE->print(" ");
			} */
			hal.uartE->println("");
			if(rxResponse.getOption()&0x2){ // was it a broadcast??
				//si es broadcast puede ser status report de un drone
				// o comando global
				
				//hal.uartE->printf("This was a BroadCast RX\n");
				//hal.uartE->printf("Data Length : %i\n",rxResponse.getDataLength());
				
				
				if(rxResponse.getDataLength() == sizeof(Drone)) // Drone type encoded...
				{	//recasting data into Drone object
					Drone *daux = static_cast < Drone* >(reinterpret_cast<Drone*>(rxResponse.getData()));
					//Drone *daux = static_cast<Drone*>(static_cast<void*>(rxResponse.getData()));
					
					//uint8_t memory[sizeof(Drone)];
					//void *place = rxResponse.getData();
					//Drone *daux = new(place)Drone();
					
					hal.uartE->printf("Reinterpreted Cast Drone\n");
					hal.uartE->printf("ID : %i\nMAC : %x %x\n",daux->getId(),daux->getMsb(),daux->getLsb());
					
					int _id = droneTable.getId(daux->getMsb(),daux->getLsb());
					if(_id>=0){ // received drone obj of an already existing drone
						//TODO -> not replacing the current drone for a reinterpobj...better send as parameter and take out data 
						daux->setId((uint32_t)_id);
						droneTable.updateDrone(daux);
					}
					else{// add drone if not on table 
					//TODO -> COPY CONSTRUCTOR OF CASTED OBJECT (to avoid delete issues..)
						daux->setId(droneTable.count());
						droneTable.addDrone(daux);	
					}
					daux=NULL;
					//place=NULL;
				}
				else{
					if(rxResponse.getData(0)==0x01){ // comando
						controller.setThrottle(int16_t(rxResponse.getData(1)<<8)|rxResponse.getData(2));
						controller.setYaw(int16_t(rxResponse.getData(3)<<8)|rxResponse.getData(4));
						controller.setPitch(int16_t(rxResponse.getData(5)<<8)|rxResponse.getData(6));
						controller.setRoll(int16_t(rxResponse.getData(7)<<8)|rxResponse.getData(8));
						controller.new_frame=true;
					}else if(rxResponse.getData(0)==0x02){
						hal.uartE->println("-- Setting Flight Mode --");
						set_mode(XBEE);
						hal.uartE->println("\tDONE..");
					}else if(rxResponse.getData(0)==0x03){
						init_arm_motors(true); //arming from xbee controll
					}else if(rxResponse.getData(0)==0x04){
						init_disarm_motors(); //desarming motors
					}
					
				}
			}
			else{
				//comando especifico a este dron
				/* TODO --> leer paquetes
					a) Valores de controller
					b) Comandos especificos
						-arm
						-land
						-takeoff
						-rtl
						-followme
						-lock alt
						-...
				*/
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
	//hal.uartE->println("wedee"); 
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
	
	//Read from S4 for debugging...
	char command;
	Drone* daux;
	while(hal.uartE->available()){
		command = hal.uartE->read();
		switch(command){
			case 'a':
				hal.uartE->println("-- Reporting Drone Table --");
				if(droneTable.isempty())
					hal.uartE->printf("Table empty : TRUE\n");
				else {
					hal.uartE->printf("Table empty : FALSE\n");
					hal.uartE->printf("Count : %u\n",droneTable.count());
					for(uint32_t x = 0; x<droneTable.count(); x++){
						daux=droneTable.getDrone(x);
						hal.uartE->printf("ID : %i\nMAC : %x %x\n",daux->getId(),daux->getMsb(),daux->getLsb());
						daux=NULL;
					}
				}
			break;
			case 'b' :
				hal.uartE->println("-- Sending AT-ND --");
				xbee.send(atRequest);
				hal.uartE->println("\tDONE..");
			break;
			case 'c' :
				hal.uartE->println("-- Clearing Table --");
				delay(200);
				droneTable.clearTable();
				hal.uartE->println("\tDONE..");
			break;
			case 'd' : {
				hal.uartE->println("-- Sending Encoded Drone --");
					
					XBeeAddress64 bcaddr64 = XBeeAddress64(0x00000000, 0x0000ffff); //broadcast..
					Drone* daux2 = new Drone(0x0013A200,0x40E6944E); // test drone with mac 0x00
					uint8_t *byteData = reinterpret_cast<uint8_t*>(daux2);
					//Reporting parameters and status in 4Hz
					Tx64Request tx64 = Tx64Request(bcaddr64, (uint8_t)0x01,byteData, sizeof(Drone),(uint8_t)0x00);
					xbee.send(tx64);  // sends to another xbee Hello world
					delete daux2;
					delete byteData;
				
				hal.uartE->println("\tDONE..");
				break;
			}
			case 'e':
				hal.uartE->println("-- Set Xbee Flight Mode --");
				if(set_mode(XBEE))
					hal.uartE->println("\tDONE..");
				else hal.uartE->println("\tFAILED..");
			break;
		}
	}
	
	//TODO
	// 		REPORT STATUS/ACTION
	// 		IF IDDLE, WAIT FOR X12 CYCLES OF THIS LOOP TO REPORT STATUS
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
	
	//update_simple_mode();
	/* hal.uartE->printf("Roll : %i\n",channel_roll->control_in);
	hal.uartE->printf("Yaw : %i\n",channel_yaw->control_in);
	hal.uartE->printf("Pitch : %i\n",channel_pitch->control_in);
	hal.uartE->printf("Throttle : %i\n",channel_throttle->control_in);
	hal.uartE->println("============================================"); */
	if(!controller.new_frame)
		return;
		
	hal.uartE->printf("Roll : %i\n",controller.getRoll());
	hal.uartE->printf("Yaw : %i\n",controller.getYaw());
	hal.uartE->printf("Pitch : %i\n",controller.getPitch());
	hal.uartE->printf("Throttle : %i\n",controller.getThrottle());
	hal.uartE->println("============================================");
	
}
#endif

#ifdef USERHOOK_NETDISCOVERYRESPONSE 
void Copter::userhook_netDiscoveryResponse(){
	uint32_t Msb;
	uint32_t Lsb;
	Msb = (uint32_t(atResponse.getValue()[2]) << 24) + (uint32_t(atResponse.getValue()[3]) << 16) + (uint16_t(atResponse.getValue()[4]) << 8) + (atResponse.getValue()[5]);
	Lsb = (uint32_t(atResponse.getValue()[6]) << 24) + (uint32_t(atResponse.getValue()[7]) << 16) + (uint16_t(atResponse.getValue()[8]) << 8) + (atResponse.getValue()[9]);
	hal.uartE->printf("MAC : %x %x\n",Msb,Lsb);
	
	//checking if the mac exists in the droneTable
	if(droneTable.isempty() || !droneTable.macExists(Msb,Lsb)){
		hal.uartE->printf("Got new Drone\n");
		droneAux = new Drone(Msb,Lsb,droneTable.count());
		droneTable.addDrone(droneAux);
		droneAux = NULL;
	}
	else 
		hal.uartE->printf("Drone already on Table\n");
}
#endif

#ifdef USERHOOK_REPORTSTATUS
void Copter::userhook_ReportStatus(){
	XBeeAddress64 bcaddr64 = XBeeAddress64(0x00000000, 0x0000ffff); //broadcast..
	Drone* daux = new Drone(0,0); // test drone with mac 0x00
	uint8_t *byteData = reinterpret_cast<uint8_t*>(daux);
	//Reporting parameters and status in 4Hz
	Tx64Request tx64 = Tx64Request(bcaddr64, (uint8_t)0x01,byteData, sizeof(Drone),(uint8_t)0x00);
	xbee.send(tx64);  // sends to another xbee Hello world
	delete daux;
	delete byteData;
}
#endif

bool Copter::xbee_init(bool ignore_checks)
{
   
    if (position_ok() || optflow_position_ok() || ignore_checks) {

        // set target to current position
        wp_nav.init_loiter_target();

        // initialize vertical speed and accelerationj
        pos_control.set_speed_z(-g.pilot_velocity_z_max, g.pilot_velocity_z_max);
        pos_control.set_accel_z(g.pilot_accel_z);

        // initialise altitude target to stopping point
        pos_control.set_target_to_stopping_point_z();

        return true;
    }else{
        return false;
    }
}

void Copter::xbee_run()
{
    float target_yaw_rate = 0;
    float target_climb_rate = 0;
    float takeoff_climb_rate = 0.0f;
	
	//hal.uartE->println(".");
	
    // if not auto armed or motor interlock not enabled set throttle to zero and exit immediately
    if(!ap.auto_armed || !motors.get_interlock()) {
        wp_nav.init_loiter_target();
        attitude_control.set_throttle_out_unstabilized(0,true,g.throttle_filt);
        pos_control.relax_alt_hold_controllers(get_throttle_pre_takeoff(controller.getThrottle())-throttle_average);
        return;
    }

    // process pilot inputs
    if (!failsafe.radio) {
        // apply SIMPLE mode transform to pilot inputs
        simpleModeXbee(); //mod 

        // process pilot's roll and pitch input
        wp_nav.set_pilot_desired_acceleration(controller.getRoll(), controller.getPitch());

        // get pilot's desired yaw rate
        target_yaw_rate = get_pilot_desired_yaw_rate(controller.getYaw());

        // get pilot desired climb rate
        target_climb_rate = get_pilot_desired_climb_rate(controller.getThrottle());
        target_climb_rate = constrain_float(target_climb_rate, -g.pilot_velocity_z_max, g.pilot_velocity_z_max);

        // get takeoff adjusted pilot and takeoff climb rates
        takeoff_get_climb_rates(target_climb_rate, takeoff_climb_rate);

        // check for take-off
        if (ap.land_complete && (takeoff_state.running || controller.getThrottle() > get_takeoff_trigger_throttle())) {
            if (!takeoff_state.running) {
                takeoff_timer_start(constrain_float(g.pilot_takeoff_alt,0.0f,1000.0f));
            }

            // indicate we are taking off
            set_land_complete(false);
            // clear i term when we're taking off
            set_throttle_takeoff();
        }
    } else {
        // clear out pilot desired acceleration in case radio failsafe event occurs and we do not switch to RTL for some reason
        wp_nav.clear_pilot_desired_acceleration();
    }

    // relax loiter target if we might be landed
    if (ap.land_complete_maybe) {
        wp_nav.loiter_soften_for_landing();
    }

    // when landed reset targets and output zero throttle
    if (ap.land_complete) {
        wp_nav.init_loiter_target();
        // move throttle to between minimum and non-takeoff-throttle to keep us on the ground
        attitude_control.set_throttle_out_unstabilized(get_throttle_pre_takeoff(controller.getThrottle()),true,g.throttle_filt);
        pos_control.relax_alt_hold_controllers(get_throttle_pre_takeoff(controller.getThrottle())-throttle_average);
    }else{
        // run loiter controller
        wp_nav.update_loiter(ekfGndSpdLimit, ekfNavVelGainScaler);

        // call attitude controller
        attitude_control.angle_ef_roll_pitch_rate_ef_yaw(wp_nav.get_roll(), wp_nav.get_pitch(), target_yaw_rate);

        // body-frame rate controller is run directly from 100hz loop

        // run altitude controller
        if (sonar_enabled && (sonar_alt_health >= SONAR_ALT_HEALTH_MAX)) {
            // if sonar is ok, use surface tracking
            target_climb_rate = get_surface_tracking_climb_rate(target_climb_rate, pos_control.get_alt_target(), G_Dt);
        }

        // update altitude target and call position controller
        pos_control.set_alt_target_from_climb_rate(target_climb_rate, G_Dt, false);
        pos_control.add_takeoff_climb_rate(takeoff_climb_rate, G_Dt);
        pos_control.update_z_controller();
    }
}
void Copter::simpleModeXbee(){
	float rollx, pitchx;

    // exit immediately if no new radio frame or not in simple mode
    if (ap.simple_mode == 0 || !controller.new_frame) {
        return;
    }

    // mark radio frame as consumed
    controller.new_frame = false;

    if (ap.simple_mode == 1) {
        // rotate roll, pitch input by -initial simple heading (i.e. north facing)
        rollx = controller.getRoll()*simple_cos_yaw - controller.getPitch()*simple_sin_yaw;
        pitchx = controller.getRoll()*simple_sin_yaw + controller.getPitch()*simple_cos_yaw;
    }else{
        // rotate roll, pitch input by -super simple heading (reverse of heading to home)
        rollx = controller.getRoll()*super_simple_cos_yaw - controller.getPitch()*super_simple_sin_yaw;
        pitchx = controller.getRoll()*super_simple_sin_yaw + controller.getPitch()*super_simple_cos_yaw;
    }

    // rotate roll, pitch input from north facing to vehicle's perspective
    controller.setRoll(rollx*ahrs.cos_yaw() + pitchx*ahrs.sin_yaw());
    controller.setPitch(-rollx*ahrs.sin_yaw() + pitchx*ahrs.cos_yaw());
	
}