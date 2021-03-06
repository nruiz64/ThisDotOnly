#include "game.h"
#include <QDateTime>
#include "time.h"
#include <QMessageBox>
#include <QSize>
#include <QDebug>

Game::Game(QWidget *parent)
    : QWidget(parent)
{

}
Game::Game(int dpiX, int dpiY)
{

}

Game::~Game()
{

}


void Game::init()
{
    //INIT GAME
    parameter = new Parameter();
    turn = new Turn();
    buttonToKill = new OwnButton();


    // INIT LAYOUT
    vLayout = new QVBoxLayout(this);
    hInfoGame = new QHBoxLayout();
    vLayout->addLayout(hInfoGame);

    labelKill = new QLabel("Kill : ");
    labelButtonToKill = new QPushButton();
    labelSecondButtonToKill = new QPushButton();
    labelTurn = new QLabel("Turn : ");
    labelTurnCount = new QLabel();
    labelTime = new QLabel();
    labelAndOr = new QLabel();

    gridLayoutInfoGame = new QGridLayout();
    hInfoGame->addLayout(gridLayoutInfoGame);

    gridLayoutInfoGame->addWidget(labelKill,0 ,0);
    gridLayoutInfoGame->addWidget(labelButtonToKill, 0, 1);
    gridLayoutInfoGame->addWidget(labelAndOr, 0, 3);
    gridLayoutInfoGame->addWidget(labelTurn, 0, 6);
    gridLayoutInfoGame->addWidget(labelTurnCount, 0, 7);
    gridLayoutInfoGame->addWidget(labelTime, 0, 8);



    initGrille();

    //INIT TIME
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,&Game::timerUpdate);
    timer->start(1000);
    timerCounting = false;

    //First Turn
    newTurn();

}

void Game::initGrille()
{
    for(int i =0; i < 25 ; ++i){
        if(i%5 == 0){
            hLayout = new QHBoxLayout();
        }
        QPointer<QPushButton> button = new QPushButton();
        button->setObjectName(QString::number(i));
        QPointer<OwnCustomColor> randomOwnCustomColor = parameter->getRandomOwnCustomColor();
        QPointer<OwnCustomButton> customButton = new OwnCustomButton(randomOwnCustomColor, "circle");
        QPointer<OwnButton> ownButton = new OwnButton(button, i ,customButton);

        connect(ownButton->getButton(), &QPushButton::clicked, this, &Game::touchDot) ;
        buttonAddedInGrille.append(ownButton);
        hLayout->addWidget(ownButton->getButton());
        vLayout->addLayout(hLayout);
    }
}

void Game::newTurn()
{
    if(turn->hasWin() == false){
        labelAndOr->setText("");
        labelSecondButtonToKill->setStyleSheet("QPushButton{"
                                             "background-color : white;"
                                             "border-radius:15px;"
                                             "max-width:30px;"
                                             "max-height:30px;"
                                             "min-width:30px;"
                                             "min-height:30px;}");

        delete turn;
        turn = new Turn();
    }
    touchedDotCount = 0;
    changeButtonInGrille();

    turn->countIncrement();
    int currentTurn = turn->getCount();
    labelTurnCount->setText(QString::number(currentTurn));
    if(currentTurn <= 3){
        step1();
    }else if(currentTurn > 3 && currentTurn <= 5){
        step2();
    }else{
        step3();
    }
}


QPointer<OwnButton> Game::getRandomOwnButtonShowed()
{
    int i = parameter->getRandomNumber(0, buttonAddedInGrille.size()-1);
    return buttonAddedInGrille[i];
}

void Game::changeButtonInGrille()
{
    for(int i=0; i < buttonAddedInGrille.size(); ++i){
        QPointer<OwnCustomColor> randomOwnCustomColor = parameter->getRandomOwnCustomColor();
        buttonAddedInGrille[i]->getOwnCustomButton()->setOwnCustomColor(randomOwnCustomColor);
        buttonAddedInGrille[i]->getButton()->setStyleSheet("QPushButton{background-color:"+buttonAddedInGrille[i]->getOwnCustomButton()->getOwnCustomColor()->getHexa()+"}");
        buttonAddedInGrille[i]->getButton()->setEnabled(true);
    }
}

void Game::removeOwnButtonFromGrille(QPointer<OwnButton> ownButtonToRemove)
{
    int pos = ownButtonToRemove->getPosition();
    buttonAddedInGrille[pos]->getButton()->setStyleSheet("QPushButton{background-color:white}");
}


int Game::getNumberOfDotToKill(QPointer<OwnButton> ownButtonToKill)
{
    int count = 0;
    for(int i = 0; i< buttonAddedInGrille.size(); ++i){
        if(buttonAddedInGrille[i]->getOwnCustomButton()->getOwnCustomColor()->getHexa()
                == ownButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()){
            ++count;
        }
    }
    return count;
}


void Game::step1(){
    qDebug() << "Step 1";
    turn->clearMultiButtonToKill();
    QPointer<OwnButton> ownButtonToKill = getRandomOwnButtonShowed();
    turn->addInMultiButtonToKill(ownButtonToKill);
    turn->setDotToKillSize(getNumberOfDotToKill(turn->getMultiButtonToKill()[0]));
    labelButtonToKill->setStyleSheet("QPushButton{ background-color : "+ownButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()+";max-width:30px;max-height:30px;min-width:30px;min-height:30px;border-radius:15px}");

}


void Game::toWinStep1(int position)
{
    if(buttonAddedInGrille[position]->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
            turn->getMultiButtonToKill()[0]->getOwnCustomButton()->getOwnCustomColor()->getHexa()){
        buttonAddedInGrille[position]->getButton()->setDisabled(true);
        removeOwnButtonFromGrille(buttonAddedInGrille[position]);
        touchedDotCountIncrement();
        if(turn->getDotToKillSize() == getTouchedDotCount() ){
            turn->setWin(true);
            newTurn();
        }
    }else{
        qTime = QTime::fromString("00:00:00");
        labelTime->setText(qTime.toString(""));
        timerCounting = false;
        turn->setWin(false);
        newTurn();
    }

}

void Game::step2()
{
    qDebug() << "Step 2";
    turn->clearMultiButtonToKill();
    QPointer<OwnButton> ownButtonToKill = getRandomOwnButtonShowed();
    turn->addInMultiButtonToKill(ownButtonToKill);
    turn->setDotToKillSize(getNumberOfDotToKill(turn->getMultiButtonToKill()[0]));

    labelButtonToKill->setStyleSheet("QPushButton{ background-color : "+ownButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()+";max-width:30px;max-height:30px;min-width:30px;min-height:30px;border-radius:15px}");
    qTime = QTime::fromString("00:00:10");
    labelTime->setText(qTime.toString("ss"));
    timerCounting = true;
}

void Game::toWinStep2(int position){
    toWinStep1(position);
}

void Game::step3()
{
    qDebug() << "Step3";
    labelTime->setText("");
    turn->clearMultiButtonToKill();
    QPointer<OwnButton> firstButtonToKill =  getRandomOwnButtonShowed();
    QPointer<OwnButton> secondButtonToKill =  getRandomOwnButtonShowed();
    while(firstButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
          secondButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()){
            secondButtonToKill =  getRandomOwnButtonShowed();
    }
    turn->addInMultiButtonToKill(firstButtonToKill);
    turn->addInMultiButtonToKill(secondButtonToKill);

    int andOr = parameter->getRandomNumber(0, 1);

    if(andOr == 0){
        turn->setColorAndOr("or");
        labelButtonToKill->setStyleSheet("QPushButton{ background-color : "+firstButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()+";max-width:30px;max-height:30px;min-width:30px;min-height:30px;border-radius:15px}");
        labelAndOr->setText(turn->getColorAndOr());
        labelSecondButtonToKill->setStyleSheet("QPushButton{ background-color : "+secondButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()+";max-width:30px;max-height:30px;min-width:30px;min-height:30px;border-radius:15px}");
        gridLayoutInfoGame->addWidget(labelSecondButtonToKill,0 ,5);
    }else{
        turn->setColorAndOr("and");
        labelButtonToKill->setStyleSheet("QPushButton{ background-color : "+firstButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()+";max-width:30px;max-height:30px;min-width:30px;min-height:30px;border-radius:15px}");
        labelAndOr->setText(turn->getColorAndOr());
        labelSecondButtonToKill->setStyleSheet("QPushButton{ background-color : "+secondButtonToKill->getOwnCustomButton()->getOwnCustomColor()->getHexa()+";max-width:30px;max-height:30px;min-width:30px;min-height:30px;border-radius:15px}");
        gridLayoutInfoGame->addWidget(labelSecondButtonToKill,0 ,5);
        turn->setDotToKillSize(getNumberOfDotToKill(firstButtonToKill) +
                               getNumberOfDotToKill(secondButtonToKill));
    }

    timerCounting = false;

}

void Game::toWinStep3(int position)
{

    touchedDotCountIncrement();
    int andor = 0;
    if(turn->getColorAndOr() == "or"){
        andor = 0;
    }else{
        andor = 1;
    }
    switch (andor) {
    case 1:


        if(buttonAddedInGrille[position]->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
                turn->getMultiButtonToKill()[0]->getOwnCustomButton()->getOwnCustomColor()->getHexa()
                ||
                buttonAddedInGrille[position]->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
                turn->getMultiButtonToKill()[1]->getOwnCustomButton()->getOwnCustomColor()->getHexa()
                ){
            removeOwnButtonFromGrille(buttonAddedInGrille[position]);
            if(turn->getDotToKillSize() == getTouchedDotCount()){
                turn->setWin(true);
                newTurn();
            }

        }else{
            turn->setWin(false);
            newTurn();
        }
        break;
    case 0:
        if(buttonAddedInGrille[position]->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
                turn->getMultiButtonToKill()[0]->getOwnCustomButton()->getOwnCustomColor()->getHexa()
                ||
                buttonAddedInGrille[position]->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
                turn->getMultiButtonToKill()[1]->getOwnCustomButton()->getOwnCustomColor()->getHexa()
                )
        {

            if(getTouchedDotCount() == 1){
                turn->setDotToKillSize(getNumberOfDotToKill(buttonAddedInGrille[position]));
                turn->setButtonToKill(buttonAddedInGrille[position]);
                removeOwnButtonFromGrille(buttonAddedInGrille[position]);
            }else{
                if(buttonAddedInGrille[position]->getOwnCustomButton()->getOwnCustomColor()->getHexa() ==
                        turn->getOwnButtonToKill()->getOwnCustomButton()->getOwnCustomColor()->getHexa()){
                    removeOwnButtonFromGrille(buttonAddedInGrille[position]);
                }else{
                    turn->setWin(false);
                    newTurn();
                }
            }

            if(getTouchedDotCount() == turn->getDotToKillSize()){
                turn->setWin(true);
                newTurn();
            }
        }else{
            turn->setWin(false);
            newTurn();
        }
        break;
    default:
        break;
    }
}

void Game::touchDot()
{
    QTextStream out(stdout);
    QObject *senderObj = sender(); // This will give Sender object
    int position = senderObj->objectName().toInt();
    int currentTurn = turn->getCount();

    if(currentTurn <= 3){
        toWinStep1(position);
    }else if(currentTurn > 3 && currentTurn <= 5){
        toWinStep2(position);
    }else{
        toWinStep3(position);
    }

}

void Game::touchedDotCountIncrement(){
    touchedDotCount++;
}
int Game::getTouchedDotCount(){
    return touchedDotCount;
}

void Game::timerUpdate()
{
    if(timerCounting){
        qTime = qTime.addSecs(-1);
        labelTime->setText(qTime.toString("ss"));
        if(qTime.toString("ss").toInt() == 0){
            qTime = QTime::fromString("00:00:00");
            labelTime->setText(qTime.toString(""));
            timerCounting = false;
            turn->setWin(false);
            newTurn();
        }
    }
}




