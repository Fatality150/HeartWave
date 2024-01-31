#ifndef PROFILE_H
#define PROFILE_H


/* Purpose of Class: To store information about a session in the database and in mainwindow
 *
 * Data Members:
 * -int id : profile ID
 * -double getBatteryLvl : battery %
 * -int breathPace : breathPace of the session
 *
 * Class Functions:
 * Getters and Setters for ID, batteryLevel, and breathPace
 */

class Profile
{
public:
    Profile(int, double, int);

    int getId();
    double getBatteryLvl();
    int getBreathPace();

    void setBatteryLvl(double);
    void setBreathPace(int);

private:
    int id;
    double batteryLevel;
    int breathPace;
};

#endif // PROFILE_H
