#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "sessionrecord.h"
#include "session.h"
#include "profile.h"
#include <QSqlDatabase>
#include <QDateTime>
#include <QSqlQuery>
#include <QString>
#include <QDebug>
#include <QList>
#include <QApplication>

/* Purpose of class: The Database Manager that holds records and profiles
 *
 * Data Members:
 * -QSqlDatabase recordDB : the database object that is used to store records and profiles
 *
 * Class functions:
 * bool addSessionRecord(const QString& sessionTitle, SessionRecord session) : extracts information from the session variable and adds the record to the database
 * bool deleteSessionRecord(SessionRecord session) : deletes the given session from the database
 * bool addProfile(int id, double batteryLevel, int breathPace) : uses the given parameters to create a profile and adds it to the database
 * bool deleteProfile(int id) : deletes the profile pf the given id from the database
 * bool deleteAllRecords() : deletes all records from the database
 * Profile* getProfile(int id) : retrieves the profile with the given id
 * SessionRecord* getSessionRecord(int id) : retrieves the session with the given id
 * void displayRecord(SessionRecord session) : displays the record that is given as a parameter
 * bool DBInit() : initializes the database

 */

class DBManager {

public:

    DBManager();
    ~DBManager();
    static const QString DATABASE_PATH;

    bool addSessionRecord(const QString& sessionTitle, SessionRecord* session);
    bool deleteSessionRecord(SessionRecord session);

    bool addProfile(int id, double batteryLevel, int breathPace);
    bool deleteProfile(int id);
    bool deleteAllRecords();
    QList<SessionRecord*>* sessionRecords();

    Profile* getProfile(int id);
    SessionRecord* getSessionRecord(int id);
    void getAllRecords();
    QList<SessionRecord*> getAllSessionRecords();
    //gets the selected record from the recordDB object
    void displayRecord(SessionRecord session);



private:
    QSqlDatabase recordDB;
    bool DBInit();
};

#endif // DBMANAGER_H




