#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = new DBManager();

    //setting hrContact to false on initial start up;
    hrContact = false;

    //place holder for database query
    profile = db->getProfile(1);
    sessionRecordData = db->getAllSessionRecords();

    ui->breathPacerVal->setValue(profile->getBreathPace());
    ui->bpView->setText(QString::number(profile->getBreathPace()));
    ui->bpView->setVisible(false);
    ui->batteryVal->setValue(profile->getBatteryLvl());
    ui->batteryLevel->setValue(int (profile->getBatteryLvl()));

    //session set up stuff
    heartSession = NULL;
    selectTest = 0;

    //initialize menus
    masterMenu = new Menu("MAIN MENU", {"SESSION","SETTINGS","HISTORY"}, nullptr);
    mainMenuOG = masterMenu;
    initializeMainMenu(masterMenu);
    
    //add initialized menu to ui
    activeQListWidget = ui->menuListView;
    activeQListWidget->addItems(masterMenu->getMenuItems());
    activeQListWidget->setCurrentRow(0);
    ui->menuLabel->setText(masterMenu->getName());

    //
    ui->graphView->addGraph();
    ui->graphView->xAxis->grid()->setVisible(false);
    ui->graphView->yAxis->grid()->setVisible(false);
    ui->graphView->xAxis->setTickLabels(false);
    ui->graphView->yAxis->setTickLabels(false);
    ui->graphView->yAxis->setRange(50, 100);
    ui->graphView->xAxis->setRange(1, 10);

    //setup ui to show the screen as off
    powerStatus = false;
    changePowerStatus();
    connect(ui->powerButton, &QPushButton::released, this, &MainWindow::powerChange);

    //prepare the buttons to be used
    connect(ui->upButton, &QPushButton::pressed, this, &MainWindow::navigateUpMenu);
    connect(ui->downButton, &QPushButton::pressed, this, &MainWindow::navigateDownMenu);
    connect(ui->okButton, &QPushButton::pressed, this, &MainWindow::navigateSubMenu);
    connect(ui->menuButton, &QPushButton::pressed, this, &MainWindow::navigateToMainMenu);
    connect(ui->backButton, &QPushButton::pressed, this, &MainWindow::navigateBack);
    connect(ui->rightButton, &QPushButton::pressed, this, &MainWindow::changeBreathPaceUp);
    connect(ui->leftButton, &QPushButton::pressed, this, &MainWindow::changeBreathPaceDown);

    //prepare the admin hr contact val to be used
    connect(ui->batteryVal, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::changeBatteryLevel);
    connect(ui->breathPacerVal, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::changeAdminBreathPace);
    connect(ui->hrContactVal, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::applyToSkin);
    connect(ui->testDataSelectVal, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::changeTestSelection);
    connect(ui->chargeButton, &QPushButton::pressed, this, &MainWindow::rechargeBattery);
}

MainWindow::~MainWindow()
{
    db->addProfile(profile->getId(), profile->getBatteryLvl(), profile->getBreathPace());

    for (int i = 0; i < sessionRecordData.size(); i++)
    {
        delete sessionRecordData[i];
    }
    
    delete ui;
    delete mainMenuOG;
    delete profile;
    delete db;

}

void MainWindow::initializeMainMenu(Menu* m)
{
    QStringList recordList;

    for (SessionRecord* r : this->sessionRecordData)
    {
        recordList.append(r->getSessionDate().toString());
    }

    Menu* session = new Menu("SESSION", {}, m);
    Menu* settings = new Menu("SETTINGS", {"CHALLENGE LEVELS", "BREATH PACE"}, m);
    Menu* history = new Menu("HISTORY", {"VIEW","CLEAR"}, m);

    m->addChildMenu(session);
    m->addChildMenu(settings);
    m->addChildMenu(history);

    Menu* sessionRecord = new Menu("SESSION RECORD", {}, session);
    session->addChildMenu(sessionRecord);

    Menu* challengeLevels = new Menu("CHALLENGE LEVELS", {}, settings);
    Menu* breathPace = new Menu("BREATH PACE", {}, settings);
    settings->addChildMenu(challengeLevels);
    settings->addChildMenu(breathPace);

    Menu* viewHistory = new Menu("VIEW", recordList, history);
    Menu* clearHistory = new Menu("CLEAR", {"YES","NO"}, history);
    history->addChildMenu(viewHistory);
    history->addChildMenu(clearHistory);


}

void MainWindow::updateMenu(const QString selectedMenuItem, const QStringList menuItems)
{
    ui->bpView->setVisible(false);
    if (masterMenu->getName() != "SESSION" || masterMenu->getName() != "SESSION RECORD") ui->sessionView->setVisible(false);

    activeQListWidget->clear();
    activeQListWidget->addItems(menuItems);
    activeQListWidget->setCurrentRow(0);

    ui->menuLabel->setText(selectedMenuItem);
}

void MainWindow::changePowerStatus()
{
    ui->hrContact->setVisible(powerStatus);
    ui->menuLabel->setVisible(powerStatus);
    ui->menuListView->setVisible(powerStatus);
    ui->batteryLevel->setVisible(powerStatus);
    ui->screenView->setVisible(powerStatus);
    
    if (!powerStatus) {
        MainWindow::navigateToMainMenu();
        ui->hrContactVal->setCurrentIndex(0);
        MainWindow::applyToSkin();
        MainWindow::clearSession();
    }

    ui->upButton->setEnabled(powerStatus);
    ui->downButton->setEnabled(powerStatus);
    ui->leftButton->setEnabled(powerStatus);
    ui->rightButton->setEnabled(powerStatus);
    ui->menuButton->setEnabled(powerStatus);
    ui->okButton->setEnabled(powerStatus);
    ui->backButton->setEnabled(powerStatus);
    ui->hrContactVal->setEnabled(powerStatus);
}

void MainWindow::powerChange()
{
    if (profile->getBatteryLvl() > 0) {
        powerStatus  = !powerStatus;
        changePowerStatus();
        return;
    }
}

void MainWindow::applyToSkin()
{
    ui->hrContact->setEnabled(ui->hrContactVal->currentIndex());
    hrContact = ui->hrContactVal->currentIndex();

    if (heartSession != NULL && hrContact)
    {
        MainWindow::pauseSession();
    }
}

void MainWindow::changeTestSelection(){
    ui->testDataSelect->setEnabled(ui->testDataSelectVal->currentIndex());
    selectTest = ui->testDataSelectVal->currentIndex();
}


void MainWindow::navigateUpMenu() {

    int nextIndex = activeQListWidget->currentRow() - 1;

    if (nextIndex < 0) {
        nextIndex = activeQListWidget->count() - 1;
    }

    activeQListWidget->setCurrentRow(nextIndex);
}

void MainWindow::navigateDownMenu() {

    int nextIndex = activeQListWidget->currentRow() + 1;

    if (nextIndex > activeQListWidget->count() - 1) {
        nextIndex = 0;
    }

    activeQListWidget->setCurrentRow(nextIndex);
}

void MainWindow::navigateSubMenu()
{


    int index = activeQListWidget->currentRow();
    if (index < 0)
    {

        if (masterMenu->getName() == "SESSION" && hrContact && heartSession == NULL)
        {
            MainWindow::beginSession();
            return;
        }
        else if (masterMenu->getName() == "SESSION" && hrContact && heartSession != NULL)
        {
            MainWindow::endSession();

            masterMenu = masterMenu->get(0);
            MainWindow::updateMenu(masterMenu->getName(), masterMenu->getMenuItems());

            ui->sessionView->setVisible(true);

            return;
        }
        else if (masterMenu->getName() == "SESSION RECORD")
        {
            MainWindow::clearSession();
            MainWindow::navigateToMainMenu();

            return;
        }
        else return;
    }

    if (masterMenu->getName() == "CLEAR") {
        if (masterMenu->getMenuItems()[index] == "YES") {
            db->deleteAllRecords();
            db->deleteProfile(profile->getId());
            mainMenuOG->get(2)->get(0)->clearAllMenuItems(); //deletes String List of view
            profile->setBreathPace(10);
            sessionRecordData.clear();
            navigateBack();
            return;
        }
        else {
            navigateBack();
            return;
        }
    }

    if (masterMenu->getName() == "VIEW")
    {
        if (ui->sessionView->isVisible())
        {
            ui->sessionView->setVisible(false);
            MainWindow::clearSession();

        }
        else
        {
            ui->sessionView->setVisible(true);
            MainWindow::viewSessionRecord(sessionRecordData.at(index));
        }

        return;
    }


    if (masterMenu->get(index)->getMenuItems().length() > 0)
    {
        masterMenu = masterMenu->get(index);
        MainWindow::updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
    }
    else if (masterMenu->getName() == "SETTINGS")
    {
        if (masterMenu->getMenuItems()[index] == "BREATH PACE")
        {
            masterMenu = masterMenu->get(index);
            MainWindow::updateMenu(masterMenu->getName(), {});

            ui->bpView->setVisible(true);
        }
    }
    else if (masterMenu->getMenuItems()[index] == "SESSION")
    {
        masterMenu = masterMenu->get(index);
        MainWindow::updateMenu(masterMenu->getName(), {});
        ui->sessionView->setVisible(true);
    }


}

void MainWindow::navigateToMainMenu()
{
    while (masterMenu->getName() != "MAIN MENU") {
        masterMenu = masterMenu->getParent();
    }

    updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
}

void MainWindow::navigateBack()
{
    if (masterMenu->getName() == "MAIN MENU") {
        activeQListWidget->setCurrentRow(0);
    }
    else if (masterMenu->getName() == "SESSION" && heartSession != NULL)
    {
        MainWindow::endSession();
        MainWindow::clearSession();

        masterMenu = masterMenu->getParent();
        updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
    }
    else if (masterMenu->getName() == "SESSION RECORD")
    {
        MainWindow::clearSession();
        masterMenu = masterMenu->getParent()->getParent();
        updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
    }
    else if (masterMenu->getName() == "VIEW" && ui->sessionView->isVisible())
    {
        ui->sessionView->setVisible(false);
        MainWindow::clearSession();
        return;
    }
    else {
        masterMenu = masterMenu->getParent();
        updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
    }
}

void MainWindow::changeBreathPaceUp()
{
    if (masterMenu->getName() == "BREATH PACE" && profile->getBreathPace() < 30)
    {
        profile->setBreathPace(profile->getBreathPace() + 1);
        ui->breathPacerVal->setValue(profile->getBreathPace());
        ui->bpView->setText(QString::number(profile->getBreathPace()));
    }
}

void MainWindow::changeBreathPaceDown()
{
    if (masterMenu->getName() == "BREATH PACE" && profile->getBreathPace() > 1)
    {
        profile->setBreathPace(profile->getBreathPace() - 1);
        ui->breathPacerVal->setValue(profile->getBreathPace());
        ui->bpView->setText(QString::number(profile->getBreathPace()));
    }
}

void MainWindow::changeAdminBreathPace()
{
    profile->setBreathPace(ui->breathPacerVal->value());
    ui->bpView->setText(QString::number(profile->getBreathPace()));
}

void MainWindow::rechargeBattery()
{

    int batteryLevel = 100.0;
    changeBatteryLevel(batteryLevel);
}

void MainWindow::changeBatteryLevel(double newLevel)
{

    if (newLevel >= 0.0 && newLevel <= 100.0) {
        if (newLevel == 0.0 && powerStatus == true) {
            powerChange();
            profile->setBatteryLvl(0);
        }else{
            profile->setBatteryLvl(newLevel);
        }

        ui->batteryVal->setValue(newLevel);
        ui->batteryLevel->setValue(int (newLevel));
    }
}

void MainWindow::drainBattery()
{
    double batteryLevel = (profile->getBatteryLvl()-.25);

    changeBatteryLevel(batteryLevel);
}

void MainWindow::beginSession()
{
    initSessionData();

    ui->lowCoherence->setVisible(false);
    ui->medCoherence->setVisible(false);
    ui->highCoherence->setVisible(false);

    heartSession = new Session(profile->getBreathPace());

    initializeTimer();
}

void MainWindow::initializeTimer(){
    currentTimerCount = 0;
    sessionTimer = new QTimer();
    connect(sessionTimer, &QTimer::timeout, this, &MainWindow::updateTimer);

    if (hrContact == true) {
        sessionTimer->start(1000);
    }
}

void MainWindow::pauseSession(){
    if(sessionTimer->signalsBlocked()){
        sessionTimer->blockSignals(false);
        return;
    }
    sessionTimer->blockSignals(true);
}

void MainWindow::updateTimer(){

    MainWindow::drainBattery();
    if (!powerStatus)
    {
        MainWindow::endSession();
        return;
    }

    if (!hrContact)
    {
        MainWindow::pauseSession();
        return;
    }



    ++currentTimerCount;

    ui->coherenceVal->setText(QString::number(heartSession->getCoherenceScore()));
    ui->achievementVal->setText(QString::number(heartSession->getAchievementScore()));
    if (currentTimerCount/60 < 10 && currentTimerCount%60 < 10)
    {
        ui->timeVal->setText("0" + QString::number(int (currentTimerCount/60)) + ":" +  "0" + QString::number(currentTimerCount%60));
    }
    else if (currentTimerCount/60 >= 10 && currentTimerCount%60 < 10)
    {
        ui->timeVal->setText(QString::number(int (currentTimerCount/60)) + ":" + "0" + QString::number(currentTimerCount%60));
    }
    else if (currentTimerCount/60 < 10 && currentTimerCount%60 >= 10)
    {
        ui->timeVal->setText("0" + QString::number(int (currentTimerCount/60)) + ":" + QString::number(currentTimerCount%60));
    }
    else
    {
        ui->timeVal->setText(QString::number(int (currentTimerCount/60)) + ":" + QString::number(currentTimerCount%60));
    }

    float avgCo = heartSession->calculateAverageCoherence();

    if (avgCo < LOWCUTOFF) ui->coherenceLevel->setStyleSheet("background-color:red;");
    else if (avgCo > HIGHCUTOFF) ui->coherenceLevel->setStyleSheet("background-color:green;");
    else ui->coherenceLevel->setStyleSheet("background-color:blue;");

    ui->coherenceBar->setValue( ( currentTimerCount % profile->getBreathPace() ) * 100 / profile->getBreathPace() );

    if(!heartSession->inProgress()){
        currentTimerCount = 0;
        sessionTimer->stop();
        return;
    }
    if(!heartRates.isEmpty() && !coherenceScores.isEmpty()){
        if(currentTimerCount % 5 == 0){
            heartSession->updateCoherence(coherenceScores.takeFirst());
        }

        MainWindow::makePlot();
        heartSession->recieveHeartbeat(heartRates.takeFirst(), currentTimerCount);

        return;
    }
    else{
        initSessionData();
    }

}

void MainWindow::endSession(){
    sessionTimer->stop();

    ui->coherenceBar->setValue(0);

    heartRates.clear();
    coherenceScores.clear();
    currentTimerCount=0;

    SessionRecord* sessionData = heartSession->createSessionRecord();

    const QString sessionTitle = "New Session";
    db->addSessionRecord(sessionTitle, sessionData);

    sessionRecordData.push_back(sessionData);

    mainMenuOG->get(2)->get(0)->addMenuItem(sessionData->getSessionDate().toString());

    if(powerStatus){
        MainWindow::viewSessionRecord(sessionData);
    }
    heartSession->endSession();

    delete heartSession;
    heartSession = NULL;

    return;
}

void MainWindow::viewSessionRecord(SessionRecord* sessionData)
{
    ui->lowCoherence->setVisible(true);
    ui->lowCoherence->setText("low " + QString::number(round (sessionData->getCoherencePercent(0) * 10000) / 100) + "%");
    ui->medCoherence->setVisible(true);
    ui->medCoherence->setText("med " + QString::number(round (sessionData->getCoherencePercent(1) * 10000) / 100) + "%");
    ui->highCoherence->setVisible(true);
    ui->highCoherence->setText("high " + QString::number(round (sessionData->getCoherencePercent(2) * 10000) / 100) + "%");

    QVector<double>* hrT = sessionData->getHeartRateTimes();
    QVector<double>* hrV = sessionData->getHeartRateVariations();

    ui->graphView->graph(0)->setData(*hrT, *hrV);
    ui->graphView->graph()->rescaleKeyAxis(false);
    ui->graphView->graph()->rescaleValueAxis(true);

    ui->graphView->replot();

    ui->coherenceLabel->setText("Avg");
    ui->coherenceVal->setText(QString::number(round (sessionData->getAverageCoherence() * 10) / 10));

    int dur = sessionData->getDuration();

    if (dur/60 < 10 && dur % 60 < 10)
    {
        ui->timeVal->setText("0" + QString::number(int (dur/60)) + ":" +  "0" + QString::number(dur%60));
    }
    else if (dur/ 60 >= 10 && dur % 60 < 10)
    {
        ui->timeVal->setText(QString::number(int (dur/60)) + ":" + "0" + QString::number(dur%60));
    }
    else if (dur/ 60 < 10 && dur % 60 >= 10)
    {
        ui->timeVal->setText("0" + QString::number(int (dur/60)) + ":" + QString::number(dur%60));
    }
    else
    {
        ui->timeVal->setText(QString::number(int (dur/60)) + ":" + QString::number(dur%60));
    }

    ui->achievementVal->setText(QString::number(sessionData->getAchievementScore()));
}

void MainWindow::clearSession()
{
    ui->coherenceLevel->setStyleSheet("background-color:black;");

    ui->coherenceVal->setText("0.0");
    ui->achievementVal->setText("0.0");
    ui->timeVal->setText("00:00");

    ui->graphView->xAxis->setRange(1, 10);

    ui->coherenceBar->setValue(0);

    ui->coherenceLabel->setText("Co");

    ui->lowCoherence->setVisible(false);
    ui->medCoherence->setVisible(false);
    ui->highCoherence->setVisible(false);

    if(!ui->graphView->graph()->data()->isEmpty())
    {
        ui->graphView->graph()->data()->clear();
        ui->graphView->replot();
    }

}

void MainWindow::initSessionData(){ //this will be the high coherence Session Data
    if(selectTest == 0){ //low coherence data
        heartRates.push_back(75.0);
        heartRates.push_back(68.0);
        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(62.0);
        coherenceScores.push_back(1.2);

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.7); //00:10

        heartRates.push_back(60.0);
        heartRates.push_back(68.0);
        heartRates.push_back(66.0);
        heartRates.push_back(72.0);
        heartRates.push_back(80.0);
        coherenceScores.push_back(0.4);

        heartRates.push_back(85.0);
        heartRates.push_back(86.0);
        heartRates.push_back(92.0);
        heartRates.push_back(87.0);
        heartRates.push_back(85.0);
        coherenceScores.push_back(1.0); //00:20

        heartRates.push_back(80.0);
        heartRates.push_back(75.0);
        heartRates.push_back(72.0);
        heartRates.push_back(68.0);
        heartRates.push_back(60.0);
        coherenceScores.push_back(1.2);

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.7); //00:30

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.1);

        heartRates.push_back(56.0);
        heartRates.push_back(52.0);
        heartRates.push_back(58.0);
        heartRates.push_back(53.0);
        heartRates.push_back(59.0);
        coherenceScores.push_back(0.5); //00:40

        heartRates.push_back(57.0);
        heartRates.push_back(60.0);
        heartRates.push_back(53.0);
        heartRates.push_back(60.0);
        heartRates.push_back(54.0);
        coherenceScores.push_back(0.4);

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.4); //00:50

        heartRates.push_back(55.0);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(1.4);

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.4); //01:00

        heartRates.push_back(52.0);
        heartRates.push_back(50.0);
        heartRates.push_back(56.0);
        heartRates.push_back(54.0);
        heartRates.push_back(60.0);
        coherenceScores.push_back(0.4);

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(1.0); //01:10

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(4.2);

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(5.0);  //01:20

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(7.2);


        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.5); //01:30

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.4);


        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(1.2); //01:40

        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        heartRates.push_back(50.0);
        heartRates.push_back(55.0);
        coherenceScores.push_back(0.4);


        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(70.0);
        coherenceScores.push_back(0.1); //01:50
    }
    else{ //high coherence data
        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(5.0);

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(4.2); //00:10

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(5.0);

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(7.2); //00:20

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(6.0);

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(6.2); //00:30

        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(70.0);
        heartRates.push_back(75.0);
        heartRates.push_back(80.0);
        coherenceScores.push_back(1.2);

        heartRates.push_back(77.0);
        heartRates.push_back(73.0);
        heartRates.push_back(70.2);
        heartRates.push_back(68.0);
        heartRates.push_back(65.0);
        coherenceScores.push_back(7.0); //00:40

        heartRates.push_back(60.0);
        heartRates.push_back(67);
        heartRates.push_back(72.0);
        heartRates.push_back(75.0);
        heartRates.push_back(82.0);
        coherenceScores.push_back(2.3);

        heartRates.push_back(80.0);
        heartRates.push_back(76.7);
        heartRates.push_back(72.9);
        heartRates.push_back(70.1);
        heartRates.push_back(67.0);
        coherenceScores.push_back(4.3); //00:50

        heartRates.push_back(60.0);
        heartRates.push_back(67);
        heartRates.push_back(72.0);
        heartRates.push_back(75.0);
        heartRates.push_back(82.0);
        coherenceScores.push_back(2.3);

        heartRates.push_back(79.0);
        heartRates.push_back(77.7);
        heartRates.push_back(74.0);
        heartRates.push_back(68.0);
        heartRates.push_back(67.3);
        coherenceScores.push_back(1.9); //01:00

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(5.0);

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(4.2); //01:10

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(5.0);

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(7.2); //01:20

        heartRates.push_back(70.0);
        heartRates.push_back(73.0);
        heartRates.push_back(77.0);
        heartRates.push_back(72.0);
        heartRates.push_back(69.5);
        coherenceScores.push_back(6.0);

        heartRates.push_back(64.0);
        heartRates.push_back(62.7);
        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(71.0);
        coherenceScores.push_back(6.2); //01:30

        heartRates.push_back(60.0);
        heartRates.push_back(65.0);
        heartRates.push_back(70.0);
        heartRates.push_back(75.0);
        heartRates.push_back(80.0);
        coherenceScores.push_back(1.2);

        heartRates.push_back(77.0);
        heartRates.push_back(73.0);
        heartRates.push_back(70.2);
        heartRates.push_back(68.0);
        heartRates.push_back(65.0);
        coherenceScores.push_back(7.0); //01:40

        heartRates.push_back(60.0);
        heartRates.push_back(67);
        heartRates.push_back(72.0);
        heartRates.push_back(75.0);
        heartRates.push_back(82.0);
        coherenceScores.push_back(2.3);

        heartRates.push_back(80.0);
        heartRates.push_back(76.7);
        heartRates.push_back(72.9);
        heartRates.push_back(70.1);
        heartRates.push_back(67.0);
        coherenceScores.push_back(4.3); //01:50

        heartRates.push_back(60.0);
        heartRates.push_back(67);
        heartRates.push_back(72.0);
        heartRates.push_back(75.0);
        heartRates.push_back(82.0);
        coherenceScores.push_back(2.3);

        heartRates.push_back(79.0);
        heartRates.push_back(77.7);
        heartRates.push_back(74.0);
        heartRates.push_back(68.0);
        heartRates.push_back(67.3);
        coherenceScores.push_back(1.9); //02:00

    }
}

void MainWindow::makePlot()
{

    ui->graphView->graph()->addData(double (currentTimerCount) , double (heartRates.first()) );

    ui->graphView->graph()->rescaleKeyAxis(true);
    ui->graphView->graph()->rescaleValueAxis(true);

    //qInfo() << double (currentTimerCount) << ", " << double (heartRates.first());

    ui->graphView->replot();
}

