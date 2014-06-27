/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011, 2012, 2013, 2014 Emanuel Eichhammer               **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 07.04.14                                             **
**          Version: 1.2.1                                                **
****************************************************************************/

/************************************************************************************************************
**                                                                                                         **
**  This is the example code for QCustomPlot.                                                              **
**                                                                                                         **
**  It demonstrates basic and some advanced capabilities of the widget. The interesting code is inside     **
**  the "setup(...)Demo" functions of MainWindow.                                                          **
**                                                                                                         **
**  In order to see a demo in action, call the respective "setup(...)Demo" function inside the             **
**  MainWindow constructor. Alternatively you may call setupDemo(i) where i is the index of the demo       **
**  you want (for those, see MainWindow constructor comments). All other functions here are merely a       **
**  way to easily create screenshots of all demos for the website. I.e. a timer is set to successively     **
**  setup all the demos and make a screenshot of the window area and save it in the ./screenshots          **
**  directory.                                                                                             **
**                                                                                                         **
*************************************************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <QSerialPort>
#include <QSerialPortInfo>

QSerialPort serial;

/*************************************************************************************************************/
/************************************************ CONSTRUCTOR ************************************************/
/*************************************************************************************************************/


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->customPlot->xAxis->setRange(0, 1000);
    ui->customPlot->yAxis->setRange(0, 3.3);

    fillPortsInfo();
    //setUpComPort();

    setupDemo();

    connect(ui->sampButton, SIGNAL(clicked()), this, SLOT(sampleButtonPressed()));
    connect(ui->reconButton, SIGNAL(clicked()), this, SLOT(reconnectButtonPressed()));
    connect(ui->clrButton, SIGNAL(clicked()), this, SLOT(clearButtonPressed()));
    connect(ui->integratePushButton, SIGNAL(clicked()), this, SLOT(integrateButtonPressed()));
    connect(ui->clearPushButton, SIGNAL(clicked()), this, SLOT(clearPushButtonPressed()));
    connect(ui->startPushButton, SIGNAL(clicked()), this, SLOT(startButtonPressed()));
    connect(ui->stopPushButton, SIGNAL(clicked()), this, SLOT(stopButtonPressed()));
}

/*************************************************************************************************************/
/******************************* INITIALIZE ALL SERIAL PORT DATA FOR SAMPLING ********************************/
/*************************************************************************************************************/

void MainWindow::setUpComPort()
{
    serial.setPortName("/dev/tty.usbmodem419571");
    //serial.setPortName(ui->serialPortInfoListBox->currentText());
    //if(serial.portName() == "/dev/tty.usbmodem419571") {
        if (serial.open(QIODevice::ReadWrite))
        {
            serial.setBaudRate(QSerialPort::Baud9600);
            serial.setDataBits(QSerialPort::Data8);
            serial.setParity(QSerialPort::NoParity);
            serial.setStopBits(QSerialPort::OneStop);
            serial.setFlowControl(QSerialPort::NoFlowControl);
            ui->statusBar->showMessage(QString("COM Port Successfully Linked"));
        }
   // }
    else
    {
        serial.close();
        ui->statusBar->showMessage(QString("Unable to Reach COM Port"));
    }
}

/*************************************************************************************************************/
/********************************************* CREATE THE GRAPH **********************************************/
/*************************************************************************************************************/

void MainWindow::setupDemo()
{

    setupAldeSensGraph(ui->customPlot);
    setWindowTitle("AldeSense");

    ui->customPlot->replot();
}

/*************************************************************************************************************/
/************************************ INITIALIZE PROPERTIES OF THE GRAPH *************************************/
/*************************************************************************************************************/

void MainWindow::setupAldeSensGraph(QCustomPlot *customPlot)
{
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot->graph(0)->setAntialiasedFill(false);

    customPlot->xAxis->setTickStep(2);
    customPlot->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:

//    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

//    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

/*************************************************************************************************************/
/********************************* ALLOW USERS TO CLICK AND DRAG THE GRAPH ***********************************/
/*************************************************************************************************************/

void MainWindow::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());

    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());

    else
        ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

/*************************************************************************************************************/
/********************** ALLOW THE USERS TO SCROLL ON TOP OF THE GRAPH TO ZOOM IN AND OUT *********************/
/*************************************************************************************************************/

void MainWindow::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());

    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());

    else
        ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

/*************************************************************************************************************/
/************************ FUNCTIONALITY OF SAMPLE BUTTON ON AMPEROMETRIC TAB IN GUI **************************/
/*************************************************************************************************************/

void MainWindow::sampleButtonPressed()
{
    dataTimer.stop();
    serial.write("cycVolt01223344,");

    ui->customPlot->clearGraphs();
    ui->customPlot->replot();

    QTimer::singleShot(1000, this, SLOT(sampleParseAndPlot()));

    ui->sampButton->setText(QString("Resample"));
    ui->startPushButton->setText(QString("Start"));
}

/*************************************************************************************************************/
/***************** READ DATA FROM SERIAL PORT AND GRAPH THE VALUES FOR AMPEROMETRIC GRAPH ********************/
/*************************************************************************************************************/

void MainWindow::sampleParseAndPlot()
{
    statusBar()->clearMessage();
    QString inByteArray;

    float x = 0;
    double y = 0;

    QVector<double> xValues(2000), yValues(2000);

    for (int i = 0; i < 2000; i++) {
        inByteArray = serial.readLine();
        y = inByteArray.toDouble();
        xValues[i] = x;
        yValues[i] = y;
        xSampleValues[i] = x;
        ySampleValues[i] = y;
        x += 0.5;
    }

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(xValues, yValues);
    ui->customPlot->replot();
}

/*************************************************************************************************************/
/******** READ DATA FROM SERIAL PORT AND GRAPH THE VALUES FOR CYCLIC VOLTAMETRY GRAPH IN REALTIME ************/
/*************************************************************************************************************/

void MainWindow::cyclicParseAndPlot(QCustomPlot *customPlot)
{
    statusBar()->clearMessage();
    demoName = "Real Time Data Demo";

    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot->graph(0)->setAntialiasedFill(false);

    customPlot->addGraph(); // blue dot
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);

    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(2);
    customPlot->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    //setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeData()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

/*************************************************************************************************************/
/******** READ DATA FROM SERIAL PORT AND GRAPH THE VALUES FOR CYCLIC VOLTAMETRY GRAPH IN REALTIME ************/
/*************************************************************************************************************/

void MainWindow::realtimeData()
{
    double xValues = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    static double lastPointKey = 0;
    if (xValues-lastPointKey > 0.01) // at most add point every 10 ms
    {
        serial.write("cycVolt01223344,");
        double value0 = (serial.readLine()).toDouble();
        // add data to lines
        ui->customPlot->graph(0)->addData(xValues, value0);
        // set data of dots:
        ui->customPlot->graph(1)->clearData();
        ui->customPlot->graph(1)->addData(xValues, value0);
        // remove data of lines that's outside visible range:
        ui->customPlot->graph(0)->removeDataBefore(xValues-8);
        // rescale value (vertical) axis to fit the current data:
        ui->customPlot->graph(0)->rescaleValueAxis();
        lastPointKey = xValues;
    }
    //make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(xValues+0.25, 8, Qt::AlignRight);
    ui->customPlot->replot();
}

/*************************************************************************************************************/
/************************ FUNCTIONALITY OF RECONNECT BUTTON ON SETTINGS TAB IN GUI ***************************/
/*************************************************************************************************************/

void MainWindow::reconnectButtonPressed()
{
    ui->statusBar->clearMessage();
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
    ui->sampButton->setText(QString("Sample"));
    serial.close();
    setUpComPort();
}

/*************************************************************************************************************/
/************************* FUNCTIONALITY OF CLEAR BUTTON ON AMPEROMETRIC TAB IN GUI **************************/
/*************************************************************************************************************/

void MainWindow::clearButtonPressed()
{
    dataTimer.stop();
    for (int i = 0; i < 2000; i++){
        xSampleValues[i] = 0;
        ySampleValues[i] = 0;
    }
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
    ui->sampButton->setText(QString("Sample"));
    ui->startPushButton->setText(QString("Start"));
    ui->returnIntegrationValue->setText("0");
}

/*************************************************************************************************************/
/********************** FUNCTIONALITY OF CLEAR BUTTON ON CYCLIC VOLTAMETRY TAB IN GUI ************************/
/*************************************************************************************************************/

void MainWindow::clearPushButtonPressed()
{
    dataTimer.stop();
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
    ui->sampButton->setText(QString("Sample"));
    ui->startPushButton->setText(QString("Start"));
    ui->returnIntegrationValue->setText("0");
}

/*************************************************************************************************************/
/************************************ COMPUTE THE INTEGRAL OF THE CURVE **************************************/
/*************************************************************************************************************/

void MainWindow::integrateButtonPressed()
{
    double ans = 0;
    double height = 0;
    double width = 0;

    for (int i = 1; i < 2000; i++){
        height = (ySampleValues[i] + ySampleValues[i-1])/2;
        width = xSampleValues[i] - xSampleValues[i-1];
        ans += (height * width);
    }

    ui->returnIntegrationValue->setText(QString::number(ans));
}

/*************************************************************************************************************/
/************* FUNCTIONALITY OF START BUTTON ON CYCLIC VOLTAMETRY TAB IN GUI TO GRAPH IN REALTIME ************/
/*************************************************************************************************************/

void MainWindow::startButtonPressed()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
    ui->startPushButton->setText(QString("Sampling"));
    ui->sampButton->setText(QString("Sample"));

    cyclicParseAndPlot(ui->customPlot);
}

/*************************************************************************************************************/
/********* FUNCTIONALITY OF STOP BUTTON ON CYCLIC VOLTAMETRY TAB IN GUI TO SET FLAG TO STOP GRAPHING *********/
/*************************************************************************************************************/

void MainWindow::stopButtonPressed()
{
    ui->startPushButton->setText(QString("Start"));
    dataTimer.stop();
}

/*************************************************************************************************************/
/********************** FILLS DROP DOWN MENU FOR SERIAL PORT INFO ON SETTINGS TAB IN GUI *********************/
/*************************************************************************************************************/

void MainWindow::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    static const QString blankString = QObject::tr("N/A");
//    QString description;
//    QString manufacturer;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
//        description = info.description();
//        manufacturer = info.manufacturer();
        list << info.portName();
//             << (!description.isEmpty() ? description : blankString)
//             << (!manufacturer.isEmpty() ? manufacturer : blankString)
//             << info.systemLocation();

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
}

/*************************************************************************************************************/
/*********************************************** DESTRUCTOR **************************************************/
/*************************************************************************************************************/

MainWindow::~MainWindow()
{
    delete ui;
    serial.close();
}
