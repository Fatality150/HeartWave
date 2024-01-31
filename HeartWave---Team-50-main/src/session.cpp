#include "session.h"

Session::Session()
{
    Session(0.0);
}

Session::Session(int bP){
    breathPace = bP;
    sessionStart = QDateTime::currentDateTime();
    achievementScore = 0;
    challengeLevel = 0;
    inSession = true;
}

Session::~Session(){
}

//public functions
SessionRecord* Session::createSessionRecord(){
    //ends the session so it no longer recieves data
    endSession();

    int sessionTime = 0;
    float avgCoherence = 0;
    float coherenceScoresSize = (float)coherenceScores.size();

    //average coherence for the entire session
    if(coherenceScoresSize != 0){
        avgCoherence = achievementScore / coherenceScoresSize;
    }

    //initializes SessionRecord onject
    if(!heartRateTimes.isEmpty()){ sessionTime = heartRateTimes.last(); }

    SessionRecord* thisSession =  new SessionRecord(1, breathPace, avgCoherence, challengeLevel, achievementScore, sessionStart, sessionTime);

    while(!heartRates.isEmpty() && !heartRateTimes.isEmpty()){
        thisSession->pushbackHRV(heartRates.takeFirst(), heartRateTimes.takeFirst());
    }

    //Calculates % of time in coherence scores (low, med, high)
    float low = 0;
    float med = 0;
    float high = 0;

    if(coherenceScoresSize == 0){
        thisSession->setCoherencePercents(low, med, high);
        return thisSession;
    }

    for(float c : coherenceScores){
        if(c<LOWCUTOFF){ //LOWCUTOFF = 0.6
            ++low;
            continue;
        }
        else if(c>HIGHCUTOFF){ //HIGHCUTOFF = 2.1
            ++high;
            continue;
        }
        else{
            ++med;
        }
    }

    thisSession->setCoherencePercents(low/coherenceScoresSize, med/coherenceScoresSize, high/coherenceScoresSize);
    return thisSession;
}

float Session::getCoherenceScore(){
    if(coherenceScores.empty()){
        return 0.0;
    }
    return coherenceScores.last();
}

float Session::getAchievementScore(){
    return achievementScore;
}

int Session::getBreathPace(){
    return breathPace;
}


//public slots
void Session::recieveHeartbeat(float hr, int time){
    if(!inSession){
        printf("ERROR: Session not in progress\n");
        return;
    }

    //printf("Heartbeat Recieved: %.2f HR at time: %d sec\n", hr, time);
    heartRates.push_back(hr);
    heartRateTimes.push_back(time);
}

void Session::updateCoherence(float inCoherence){
    if(!inSession){
        printf("ERROR: Session not in progress\n");
        return;
    }
    coherenceScores.push_back(inCoherence);
    achievementScore += inCoherence;
}

bool Session::inProgress(){
    return inSession;
}


void Session::endSession(){
    inSession = false;
    //for(int i = 0; i < heartRates.size(); ++i){
        //printf("Heartbeat %d: %.2f HR at time: %d ms\n", i, heartRates.at(i), heartRateTimes.at(i));
    //}
    return;
}

//private functions

float Session::calculateAverageCoherence(){
    float coherenceScoresSize = (float)coherenceScores.size();

    if(coherenceScoresSize < 12){
        return achievementScore / coherenceScoresSize;
    }

    float recentAvgCoherence = 0.0;

    for(int i = coherenceScoresSize-11; i<coherenceScoresSize; ++i){
        recentAvgCoherence += coherenceScores[i];
    }

    return recentAvgCoherence/12;
}

