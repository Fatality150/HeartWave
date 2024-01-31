#ifndef SESSIONRECORD_H
#define SESSIONRECORD_H

#include <QString>
#include <QDateTime>
#include <QTime>
#include <QVector>

/* Purpose of Class: To store information about a session in the database and in mainwindow
 *
 * Data Members:
 * -int breathPace : breathPace of the session
 * -float averageCoherence : average Coherence of the session
 * -float coherencePercents[3] : % of time spent in low, med, high coherence [0-2]
 * -QVector<double>* Heartrate : all heartrates
 * -QVector<double>* HeartrateTimes : when the heartrates occurred (in secs)
 * -int challengeLevel : N/A
 * -float achievementScore : total Coherence
 * -QDateTime sessionDate : Date/Time at session start
 * -int duration : length of session in seconds
 * -int id : sessionID
 *
 * Class Functions:
 * Getters and Setters for Data
 * void setHeartRateData(QString hrV, QString hrT) : adds the vectors,
 * void pushbackHRV(float HR, int time) : adds heartrate, time pair to the data set
 */

class SessionRecord
{
public:
    SessionRecord(int sesID, int sesBR, float avgCoh, int chall, float achievement, QDateTime sessionD, int duration);

    int getID();
    int getBreathPace();
    float getAverageCoherence();
    float getCoherencePercent(int n);
    int getChallengeLevel();
    float getAchievementScore();
    QDateTime getSessionDate();
    int getDuration();
    QVector<double>* getHeartRateVariations();
    QVector<double>* getHeartRateTimes();

    void setHeartRateData(QString hrV, QString hrT);

    void pushbackHRV(float HR, int time);
    void setCoherencePercents(float low, float med, float high);

private:
    int breathPace;
    float averageCoherence;
    float coherencePercents[3];
    QVector<double>* Heartrate;
    QVector<double>* HeartrateTimes;
    int challengeLevel;
    float achievementScore;

    QDateTime sessionDate;
    int duration;
    int id;

};

#endif // SESSIONRECORD_H
