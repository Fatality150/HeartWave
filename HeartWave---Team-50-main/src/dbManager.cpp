#include "dbManager.h"

const QString DBManager::DATABASE_PATH = "/database/heartwave.db";

DBManager::DBManager() {

    recordDB = QSqlDatabase::addDatabase("QSQLITE");
    recordDB.setDatabaseName("HeartWaveData.db");

    if (!recordDB.open()) {
        throw "Error: Database could not be opened";
    }

    if (!DBInit()) {
        throw "Error: Database could not be initialized";
    }
    //qInfo() << "Record DB Open = " << recordDB.isOpen();
}

DBManager::~DBManager() {
    recordDB.close();
}

bool DBManager::DBInit() {

    recordDB.transaction();

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS records ( "
               "recordID INT, "
               "breathPace INT, "
               "averageCoherence FLOAT, "
               "coherencePercent1 FLOAT, "
               "coherencePercent2 FLOAT, "
               "coherencePercent3 FLOAT, "
               "heartRateVariations TEXT, "
               "heartRateTimes TEXT, "
               "challengeLevel INT, "
               "achievementScore FLOAT, "
               "sessionDate DATETIME, "
               "duration INT"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS profiles ("
               "id INTEGER PRIMARY KEY, "
               "batteryLevel DOUBLE, "
               "breathPace INT"
               ");");

    return recordDB.commit();
}

bool DBManager::addSessionRecord(const QString& sessionTitle, SessionRecord* session) {

    recordDB.transaction();

    QSqlQuery query;

    //Get the variables
    int recordID = session->getID();
    int breathPace = session->getBreathPace();
    float averageCoherence = session->getAverageCoherence();
    float coherencePercent1 = session->getCoherencePercent(0);
    float coherencePercent2 = session->getCoherencePercent(1);
    float coherencePercent3 = session->getCoherencePercent(2);
    QVector<double>* heartRateVariations = session->getHeartRateVariations();
    QVector<double>* heartRateTimes = session->getHeartRateTimes();
    int challengeLevel = session->getChallengeLevel();
    float achievementScore = session->getAchievementScore();
    QDateTime sessionDate = session->getSessionDate();
    int duration = session->getDuration();

    query.prepare("REPLACE INTO records (recordID, "
                  "breathPace, averageCoherence, "
                  "coherencePercent1, coherencePercent2, coherencePercent3, "
                  "heartRateVariations, heartRateTimes, challengeLevel, "
                  "achievementScore, sessionDate, duration) "
                  "VALUES (:recordID, "
                  ":breathPace, :averageCoherence, "
                  ":coherencePercent1, :coherencePercent2, :coherencePercent3, "
                  ":heartRateVariations, :heartRateTimes, :challengeLevel, "
                  ":achievementScore, :sessionDate, :duration);");

    query.bindValue(":recordID", recordID);
    query.bindValue(":breathPace", breathPace);
    query.bindValue(":averageCoherence", averageCoherence);

    query.bindValue(":coherencePercent1", coherencePercent1);
    query.bindValue(":coherencePercent2", coherencePercent2);
    query.bindValue(":coherencePercent3", coherencePercent3);

    QString heartRateString = "";
    for (int i = 0; i < heartRateVariations->size(); ++i) {
        heartRateString += QString::number(heartRateVariations->at(i));
        if (i < heartRateVariations->size() - 1) {
            heartRateString += ", ";
        }
    }
    query.bindValue(":heartRateVariations", heartRateString);

    QString heartRateTimeString = "";
    for (int i = 0; i < heartRateTimes->size(); ++i) {
        heartRateTimeString += QString::number(heartRateTimes->at(i));
        if (i < heartRateTimes->size() - 1) {
            heartRateTimeString += ", ";
        }
    }
    query.bindValue(":heartRateTimes", heartRateTimeString);


    query.bindValue(":challengeLevel", challengeLevel);
    query.bindValue(":achievementScore", achievementScore);
    query.bindValue(":sessionDate", sessionDate);
    query.bindValue(":duration", duration);

    query.exec();

    return recordDB.commit();
}

bool DBManager::addProfile(int id, double batteryLevel, int breathPace){
    recordDB.transaction();

    QSqlQuery query;

    query.prepare("REPLACE INTO profiles (id, batteryLevel, breathPace) VALUES (:id, :batteryLevel, :breathPace);");
    query.bindValue(":id", id);
    query.bindValue(":batteryLevel", batteryLevel);
    query.bindValue(":breathPace", breathPace);

    query.exec();

    return recordDB.commit();
}

bool DBManager::deleteSessionRecord(SessionRecord session){
    int recordID = session.getID();

    recordDB.transaction();

    QSqlQuery query;

    query.prepare("DELETE FROM records WHERE recordID = :recordID;");
    query.bindValue(":recordID", recordID);
    query.exec();

    return recordDB.commit();
}

bool DBManager::deleteProfile(int id){
    recordDB.transaction();

    QSqlQuery query;

    query.prepare("DELETE FROM profiles WHERE id = :id;");
    query.bindValue(":id", id);
    query.exec();

    return recordDB.commit();
}

bool DBManager::deleteAllRecords() {

    QSqlQuery query;
    query.prepare("DELETE FROM records;");
    query.exec();
    return query.exec();
}

Profile* DBManager::getProfile(int id) {

    recordDB.transaction();

    QSqlQuery query;
    query.prepare("SELECT * FROM profiles WHERE id=:id;");
    query.bindValue(":id", id);
    query.exec();

    if (!recordDB.commit()) {
        throw "Error: Query failed to execute";
    }

   // if the profile does not exist
    if (!query.next()) {
        addProfile(id, 100.0, 10);
        Profile* pro = new Profile(id, 100.0, 10);
        return pro;
    }

    // if it does exist
    Profile* pro = new Profile(query.value(0).toInt(), query.value(1).toDouble(), query.value(2).toInt());
    return pro;
}

SessionRecord* DBManager::getSessionRecord(int id) {

    recordDB.transaction();

    QSqlQuery query;
    query.prepare("SELECT * FROM records WHERE id=:recordID;");
    query.bindValue(":recordID", id);
    query.exec();

    if (!recordDB.commit()) {
        throw "Error: Query failed to execute";
    }

    // if it does exist
    SessionRecord* sessionRecord = new SessionRecord(query.value(0).toInt(), query.value(1).toInt(), query.value(2).toFloat(), query.value(8).toInt(), query.value(9).toFloat(), query.value(10).toDateTime(), query.value(11).toInt());
    return sessionRecord;
}

QList<SessionRecord*> DBManager::getAllSessionRecords() {
    //Create a list to store all the session records
    QList<SessionRecord*> sessionRecords;

    recordDB.transaction();

    QSqlQuery query;

    //selecting every record
    query.prepare("SELECT * FROM records");

    query.exec();

    while (query.next()) {
        SessionRecord* sessionRecord = new SessionRecord(query.value(0).toInt(), query.value(1).toInt(), query.value(2).toFloat(), query.value(8).toInt(), query.value(9).toFloat(), query.value(10).toDateTime(), query.value(11).toInt());
        sessionRecord->setCoherencePercents(query.value(3).toFloat(), query.value(4).toFloat(), query.value(5).toFloat());
        sessionRecord->setHeartRateData(query.value(6).toString(), query.value(7).toString());
        sessionRecords.append(sessionRecord);
    }

    //throw a message if the query does not execute
    if (!recordDB.commit()) {
        throw "Error: Query failed to execute";
    }

    return sessionRecords;
}

void DBManager::getAllRecords(){
    recordDB.transaction();

    QSqlQuery query;
    query.exec("SELECT * FROM records;");

    while(query.next()){
        qInfo() << "ID: " << query.value(0).toInt() << ", BP: " << query.value(1).toInt() << ", avgCoh: " << query.value(2).toFloat();
        qInfo() << "coh%s: " << query.value(3) << ", hrV: " << query.value(4)<< ", hrT: " << query.value(5);
        qInfo() << "chall: " << query.value(6) << ", ach: " << query.value(7)<< ", sessDate: " << query.value(8) << ",  dura" << query.value(9);
    }
    query.finish();
}


