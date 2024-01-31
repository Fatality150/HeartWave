#include "profile.h"

Profile::Profile(int id, double batteryLevel, int breathPace) {

    this->id = id;
    this->batteryLevel = batteryLevel;
    this->breathPace = breathPace;
}

// getters
int Profile::getId() { return id; }
double Profile::getBatteryLvl() { return batteryLevel; }
int Profile::getBreathPace() {return breathPace; }

// setters
void Profile::setBatteryLvl(double newLvl) { batteryLevel = newLvl; }
void Profile::setBreathPace(int newPace) {breathPace = newPace; }

