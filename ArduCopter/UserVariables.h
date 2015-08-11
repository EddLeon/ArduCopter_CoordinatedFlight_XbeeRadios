/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

// user defined variables

// example variables used in Wii camera testing - replace with your own
// variables
#ifdef USERHOOK_VARIABLES

#if WII_CAMERA == 1
WiiCamera           ircam;
int                 WiiRange=0;
int                 WiiRotation=0;
int                 WiiDisplacementX=0;
int                 WiiDisplacementY=0;
#endif  // WII_CAMERA



  // XBEE SHIT
	XBee xbee = XBee();
	XBeeResponse response = XBeeResponse();

	TxStatusResponse txStatus = TxStatusResponse();
	//48 65 6c 6c 6f 20 57 6f 72 6c 64   --> Hello World 
	//uint8_t payload[] = {0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,0x72,0x6c,0x64};
	XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x40E6944E);  // MAC ADDRS
	Rx64Response rx64 = Rx64Response();
	//Tx64Request tx64 = Tx64Request(addr64, payload, sizeof(payload));
	
	//Drone Auxiliary vars
	
	//Drone thisDrone = Drone();  // puede que no jale esto...
	
#endif  // USERHOOK_VARIABLES


