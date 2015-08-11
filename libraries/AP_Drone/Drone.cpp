#include "Drone.h"

Drone::Drone(){
	_id=droneCount++;
}

Drone::Drone(uint32_t msb, uint32_t lsb){
	_msb=msb;
	_lsb=lsb;
	_id=droneCount++;
}
Drone::Drone(uint32_t msb, uint32_t lsb, uint32_t id){
	_msb=msb;
	_lsb=lsb;
	_id=id;
}
uint32_t Drone::droneCount=0;

void Drone::setMsb(uint32_t msb){
	_msb=msb;
}
void Drone::setLsb(uint32_t lsb){
	_lsb=lsb;
}
void Drone::setId(uint32_t id){
	_id=id;
}

uint8_t Drone::getRssi(){
	return rssi;
}
uint32_t Drone::getMsb(){
	return _msb;
}
uint32_t Drone::getLsb(){
	return _lsb;
}
uint8_t Drone::getFlags(){
	return flags.value;
}

uint32_t Drone::getId(){
	return _id;
}

//TODO
void Drone::updateLocation(Location currentloc){
	
}

Location Drone::getLocation(){
	return loc;
}

DroneTable::DroneTable(){
	
}
void DroneTable::addDrone(Drone* drone){
		droneVector.push_back(drone);
}
void DroneTable::updateDrone(Drone* drone){
	delete droneVector[drone->getId()];
	droneVector[drone->getId()]=drone;
}
void DroneTable::removeDrone(uint32_t id){
	for(uint32_t x=0;x<droneVector.size();x++)
		if(droneVector[x]->getId()==id){
			delete droneVector[x];
			droneVector.erase(droneVector.begin()+x);
			return;
		}
}
Drone* DroneTable::getDrone(uint32_t id){
	//for(uint32_t x=0;x<droneVector.size();x++)
	//	if(droneVector[x]->getId()==id){
			//std::vector<Drone*>::iterator tempIt = droneVector.begin();
			 //std::vector<Drone*>::iterator i = droneVector.begin();
			//return droneVector[i+id];
	//	}
	//return NULL;
	return droneVector[id];
}
DroneTable::~DroneTable(){
	while(!droneVector.empty()){
		delete droneVector[0];
		droneVector.erase(droneVector.begin());
	}
}
uint32_t DroneTable::count(){
	return droneVector.size();
}
bool DroneTable::isempty(){
	return droneVector.empty();
}
bool DroneTable::macExists(uint32_t msb, uint32_t lsb){
	for(uint32_t x=0;x<droneVector.size();x++){
		if(!(lsb^droneVector[x]->getLsb()) && !(msb^droneVector[x]->getMsb()))
			return true;
	}
	return false;
}
int DroneTable::getId(uint32_t msb, uint32_t lsb){
	for(uint32_t x=0;x<droneVector.size();x++){
		if(!(lsb^droneVector[x]->getLsb()) && !(msb^droneVector[x]->getMsb()))
			return x;
	}
	return -1;
}
void DroneTable::clearTable(){
	/*
	//	DELETING REINTERPRETED DRONES WILL CRASH THE DRONE
	while(!droneVector.empty()){
		delete droneVector[0];
		droneVector.erase(droneVector.begin());
	}
	*/
}

void Controller::setYaw(int16_t Yaw){
	yaw = Yaw;
}
void Controller::setThrottle(int16_t Throttle){
	throttle = Throttle;
}
void Controller::setPitch(int16_t Pitch){
	pitch = Pitch;
}
void Controller::setRoll(int16_t Roll){
	roll = Roll;
}
int16_t Controller::getYaw(){
	return yaw;
}
int16_t Controller::getThrottle(){
	return throttle;
}
int16_t Controller::getPitch(){
	return pitch;
}
int16_t Controller::getRoll(){
	return roll;
}
Controller::Controller(){
	yaw=0;
	roll=0;
	pitch=0;
	throttle=0;
	new_frame=false;
}
Controller::Controller(int16_t Yaw,int16_t Throttle, int16_t Pitch, int16_t Roll){
	yaw=Yaw;
	roll=Roll;
	pitch=Pitch;
	throttle=Throttle;
	new_frame=true;
}