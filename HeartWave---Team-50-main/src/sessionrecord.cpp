#include "sessionrecord.h"

SessionRecord::SessionRecord(int sesID, int sesBR, float avgCoh, int chall, float achievement, QDateTime sessionD, int duration)
{
    id = sesID;
    breathPace = sesBR;
    averageCoherence = avgCoh;
    challengeLevel = chall;
    achievementScore = achievement;
    sessionDate = sessionD;
    this->duration = duration;
    Heartrate = new QVector<double>;
    HeartrateTimes = new QVector<double>;
}

int SessionRecord::getID(){
    return id;
}
int SessionRecord::getBreathPace(){
    return breathPace;
}

float SessionRecord::getAverageCoherence(){
    return averageCoherence;
}
float SessionRecord::getCoherencePercent(int n){
    return coherencePercents[n%3];
}

int SessionRecord::getChallengeLevel(){
    return challengeLevel;
}

float SessionRecord::getAchievementScore(){
    return achievementScore;
}

QDateTime SessionRecord::getSessionDate(){
    return sessionDate;
}

int SessionRecord::getDuration(){
    return duration;
}

QVector<double>* SessionRecord::getHeartRateVariations(){
    return Heartrate;
}

QVector<double>* SessionRecord::getHeartRateTimes(){
    return HeartrateTimes;
}

void SessionRecord::pushbackHRV(float HR, int time){
    Heartrate->push_back(double (HR));
    HeartrateTimes->push_back(double (time));
}

void SessionRecord::setCoherencePercents(float low, float med, float high){
    coherencePercents[0] = low;
    coherencePercents[1] = med;
    coherencePercents[2] = high;
}

void SessionRecord::setHeartRateData(QString hrV, QString hrT){
    const QRegExp rx(QLatin1Literal("[^0-9.]+"));
    QStringList hrVList = hrV.split(rx, QString::SkipEmptyParts);
    QStringList hrTList = hrT.split(rx, QString::SkipEmptyParts);

    while(!hrVList.isEmpty() && !hrTList.isEmpty()){
        Heartrate->push_back(hrVList.takeFirst().toDouble());
        HeartrateTimes->push_back(hrTList.takeFirst().toDouble());
    }

}
