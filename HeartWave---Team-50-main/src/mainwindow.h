#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QGraphicsView>
#include <QTime>
#include <QStatusBar>
#include <QVector>
#include <QtGlobal>
#include <QTimer>
#include <QtDebug>

#include "menu.h"
#include "session.h"
#include "profile.h"
#include "dbManager.h"
#include "sessionrecord.h"
#include "qcustomplot.h"

/* Purpose of class: The main window is used to create and facilitate intercations between the system and the ui. It is the main hub were all elements in the system are temporarily stored during runtime.
 *
 * Data Members:
 * 
 * -Menu* masterMenu : The current menu the system uses for the ui. 
 * -Menu* mainMenuOG : The root of the menu the system uses to reset the ui to the original element.
 * -Ui::MainWindow *ui : the ui element the mainwindow uses to visually display all information to the user.
 * -QListWidget *activeQListWidget : a list widget the used by the system to display sub menus of the masterMenu.
 * -DBManager* db : the database that the system pulls from on start up and adds to on program end.
 * -Profile* profile : the profile the system uses to hold information about key system features such as breath pace and battery level.
 * -bool powerStatus : if true the system should be on otherwise it is false
 * -bool hrContact : if true the system is in physical contact with the user and should begin a session if prompted by the user.
 * -Session *heartSession : an instance of a session used to hold information on the session and calculate statistics for the record.
 * -QTimer *sessionTimer : a timer thats used to measure the amount of time that has passed in between each update of a session.
 * -int currentTimerCount : a value that denotes the number of seconds a session has run for.
 * -QVector<float> heartRates : a vector of flaot values that hold the heart rate inputs used by the session.
 * -QVector<float> coherenceScores : a vector of float values that hold the coherence inputs used by the session.
 *
 * Class functions:
 * 
 * void initializeMainMenu(Menu*) : initializes the menu by creating and adding children to the root of the menu, which is made at the beggining of runtime.
 * void updateMenu(const QString, const QStringList) : updates the current menu and adds all children of the current menu to the active list widget.
 * void changePowerStatus(): switches the current status of the system from off to on and vice versa.
 * void drainBattery() : reduces the systems battery level while it is in a session.
 * void beginSession() : begins a session by initializing the values to be stored in it.
 * void initializeTimer() : initializes a timer to be used by the session.
 * void updateTimer() : updates the session every timer count.
 * void endSession() : ends the session, save it, and transfers to a view of the session record.
 * void viewSessionRecord(SessionRecord sessionData) : shows key information about the specified session.
 * void clearSession() : clears session elements from the ui so that it can be used for other menus.
 * void initSessionData() :  holds the input values for the session.
 * void pauseSession() : stops the session timer under specific circumstances such as a loss of hr contact.
 * void makePlot() : makes the plot that shows the change in heart rate over time.
 * void powerChange() : changes the power status and calls changePowerStatus().
 * void applyToSkin() : changes the hr contact value.
 * void navigateDownMenu() : increases the current index of the menu's item's and moves the cursor down in the ui (or loops back to the top if there are no items below the one previously selected).
 * void navigateUpMenu() : decreases the current index of the menu's item's and moves the cursor up in the ui (or loops to the bottom if there are no items below the on previously selected).
 * void navigateSubMenu() : updates the menu to show the child menus of the previously selected element, it will also start or end a session or loop back to a previous menu depending on the current 
 * state of the master menu.
 * 
 * void navigateToMainMenu() : resets the master menu to be the original menu and updates the ui.
 * void navigateBack() : move to the parent menu of the current one.
 * void changeBreathPaceUp() : if the program is in the breath pace tab it will increase the breath pace of the system by 1 and update the user profile.
 * void changeBreathPaceDown() if the program is in the breath pace tab it will decrease the breath pace of the system by 1 and update the user profile.
 * void changeAdminBreathPace() : this will change the value of the breath pace if the user changes the value of the spin box on the admin panel of the program.
 * void rechargeBattery() : if the user presses the charge button on the admin panel this will set the battery level to 100.0.
 * void changeBatteryLevel(double) : if the user changes the battery level in the admin section using the spin box, this will update the current battery level to whatever it was set to.
<<<<<<< HEAD
*/

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Menu* masterMenu;
    Menu* mainMenuOG;

    Ui::MainWindow *ui;
    QListWidget *activeQListWidget;

    DBManager* db;
    Profile* profile;

    bool powerStatus;
    bool hrContact;

    int selectTest;

    Session *heartSession;
    QTimer *sessionTimer;
    int currentTimerCount;
    QVector<float> heartRates;
    QVector<float> coherenceScores;
    QList<SessionRecord*> sessionRecordData;

    void initializeMainMenu(Menu*);
    void updateMenu(const QString, const QStringList);

    void changePowerStatus();
    void drainBattery();

    void beginSession();
    void initializeTimer();
    void updateTimer();
    void endSession();
    void viewSessionRecord(SessionRecord* sessionData);
    void clearSession();
    void initSessionData();
    void pauseSession();

    void makePlot();

private slots:
    void powerChange();
    void applyToSkin();
    void changeTestSelection();

    void navigateDownMenu();
    void navigateUpMenu();
    void navigateSubMenu();
    void navigateToMainMenu();
    void navigateBack();

    void changeBreathPaceUp();
    void changeBreathPaceDown();
    void changeAdminBreathPace();

    void rechargeBattery();
    void changeBatteryLevel(double);

};
#endif // MAINWINDOW_H
