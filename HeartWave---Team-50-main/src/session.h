#ifndef SESSION_H
#define SESSION_H

#define LOWCUTOFF 0.6
#define HIGHCUTOFF 2.1

#include <QObject>
#include <QString>
#include <QElapsedTimer>
#include <QDateTime>
#include <QVector>
#include "sessionrecord.h"

/* Purpose of class: Session object to hold information about an ongoing session
 *
 * Data Members:
 * -int breathPace : Session's selected breath pacer time
 * -bool inSession : true if a session is in progress
 * -int challengeLevel : changes low, med, high threshold
 * -QDateTime sessionStart : Date Time at session start
 * -float achievementScore : total of all coherence score
 * -QVector<float> coherenceScores : all the coherence scores
 * -QVector<int> heartRateTimes : all the heart rate input times
 * -QVector<float> heartRates : all the heartrates
 *
 * Class functions:
 * Some Getters for most recent coherence score, achievement score
 * SessionRecord createSessionRecord() : creates and returns a session record using the data from the session
 * void recieveHeartbeat(float hr, int time) : slot that recieves heart rate value & its recorded time
 * void endSession() : handles ending of the session
 * void updateCoherence(float inCoherence) : slot that recieves a coherence value
 * bool inProgress() : returns if the session is in progress
 * float calculateAverageCoherence() : calculates the average coherence score over the previous 64 seconds
 */

class Session : public QObject{
    Q_OBJECT

public:
    Session();
    Session(int bP);
    ~Session();

    SessionRecord* createSessionRecord();

    float getCoherenceScore();
    float getAchievementScore();
    int getBreathPace();

public slots:
    void recieveHeartbeat(float hr, int time);
    void endSession();
    void updateCoherence(float inCoherence);
    bool inProgress();
    float calculateAverageCoherence(); //over the last 64 seconds

private:
    bool inSession;
    int breathPace;
    int challengeLevel;
    QDateTime sessionStart;
    float achievementScore;

    QVector<float> coherenceScores;
    QVector<int> heartRateTimes;
    QVector<float> heartRates;
};

#endif // SESSION_H
