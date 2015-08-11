#include <stdint.h>
#include <AP_Common.h>
#include <AP_HAL.h>
#include <AP_HAL_PX4.h>
#include <vector>


class Drone{
	public:
		Drone();
		Drone(uint32_t msb, uint32_t lsb);
		Drone(uint32_t msb, uint32_t lsb, uint32_t id);
		void setMsb(uint32_t msb);
		void setLsb(uint32_t lsb);
		void setId(uint32_t id);
		uint8_t getRssi();
		uint32_t getMsb();
		uint32_t getLsb();
		uint8_t getFlags();
		void updateLocation(Location currentloc);
		Location getLocation();
		uint32_t timeStamp;
		uint32_t getId();
	private:
		Location loc;
		static uint32_t droneCount;
		uint32_t _id;
		uint32_t _msb;
		uint32_t _lsb;
		uint8_t rssi;
		union{
			struct{
				uint8_t hasGPS		: 1; //0
				uint8_t hasTelem	: 1; //1
				uint8_t	state		: 3; //2,3,4
				uint8_t	mode		: 3; //5,6,7
			};
			uint8_t value;
		}flags;
		struct {
			bool running;
			float speed;
			uint32_t start_ms;
			uint32_t time_ms;
		}takeoff_state;
};

class DroneTable{
	public:
		DroneTable();
		void addDrone(Drone* drone);
		void removeDrone(uint32_t id);
		void updateDrone(Drone* drone);
		Drone* getDrone(uint32_t id);
		void clearTable();
		bool isempty();
		bool macExists(uint32_t msb, uint32_t lsb);
		int getId(uint32_t msb, uint32_t lsb);
		uint32_t count();
		~DroneTable();
	private:
		std::vector<Drone*> droneVector;	
};

class Controller{
	public:
		void setYaw(int16_t Yaw);
		void setThrottle(int16_t Throttle);
		void setPitch(int16_t Pitch);
		void setRoll(int16_t Roll);
		int16_t getYaw();
		int16_t getThrottle();
		int16_t getPitch();
		int16_t getRoll();
		Controller();
		Controller(int16_t Yaw,int16_t Throttle, int16_t Pitch, int16_t Roll);
		bool new_frame;
	private:
		int16_t throttle;
		int16_t yaw;
		int16_t pitch;
		int16_t roll;
};