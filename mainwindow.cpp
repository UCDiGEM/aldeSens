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
**  This was developed from example code for QCustomPlot.                                                  **
**  UC Davis iGEM 2014                                                                                     **
**                                                                                                         **
**                                                                                                         **
*************************************************************************************************************/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>
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
    setWindowTitle("OliView");
    ui->setupUi(this);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectPlottables);

    ui->customPlot->xAxis->setRange(0, 1000);
    ui->customPlot->yAxis->setRange(0, 3.3);
    ui->customPlot->xAxis->setLabel("Milliseconds (ms)");
    ui->customPlot->yAxis->setLabel("Volts (V)");

    setUpComPort();

    setupAldeSensGraph(ui->customPlot);

    connect(ui->sampButtonCV, SIGNAL(clicked()), this, SLOT(sampCVPressed()));
    connect(ui->sampButtonPA, SIGNAL(clicked()), this, SLOT(sampPAPressed()));
    connect(ui->sampButtonAS, SIGNAL(clicked()), this, SLOT(sampASPressed()));
    connect(ui->reconButton, SIGNAL(clicked()), this, SLOT(reconnectButtonPressed()));
    connect(ui->clrButton, SIGNAL(clicked()), this, SLOT(clearButtonPressed()));
}

/*************************************************************************************************************/
/******************************* INITIALIZE ALL SERIAL PORT DATA FOR SAMPLING ********************************/
/*************************************************************************************************************/

void MainWindow::setUpComPort()
{
    serial.setPortName("com6");
    if (serial.open(QIODevice::ReadWrite))
    {
        serial.setBaudRate(QSerialPort::Baud9600);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);
        ui->statusBar->showMessage(QString("COM Port Successfully Linked"));
    }

    else
    {
        serial.close();
        ui->statusBar->showMessage(QString("Unable to Reach COM Port"));
    }
}

/*************************************************************************************************************/
/************************************ INITIALIZE PROPERTIES OF THE GRAPH *************************************/
/*************************************************************************************************************/

void MainWindow::setupAldeSensGraph(QCustomPlot *customPlot)
{


    // applies a different color brush to the first 9 graphs
    for (int i=0; i<10; ++i) {
        customPlot->addGraph(); // blue line
        customPlot->graph(i)->setBrush(QBrush(QColor(240, 255, 200))); // I think this is the fill under the line, it's ugly.
        customPlot->graph(i)->setAntialiasedFill(false);

        switch (i) {
        case 0: customPlot->graph(i)->setPen(QPen(Qt::blue)); break;
        case 1: customPlot->graph(i)->setPen(QPen(Qt::red)); break;
        case 2: customPlot->graph(i)->setPen(QPen(Qt::green)); break;
        case 3: customPlot->graph(i)->setPen(QPen(Qt::yellow)); break;
        case 4: customPlot->graph(i)->setPen(QPen(Qt::cyan)); break;
        case 5: customPlot->graph(i)->setPen(QPen(Qt::magenta)); break;
        case 6: customPlot->graph(i)->setPen(QPen(Qt::darkRed)); break;
        case 7: customPlot->graph(i)->setPen(QPen(Qt::darkGreen)); break;
        case 8: customPlot->graph(i)->setPen(QPen(Qt::darkBlue)); break;
        }
    }

    customPlot->xAxis->setTickStep(2);
    customPlot->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

    // setup policy and connect slot for context menu popup:
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    ui->customPlot->replot();
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
/*********************************** GRAPH SELECTION FUNCTIONS ***********************************************/
/*************************************************************************************************************/

void MainWindow::removeSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
    ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    ui->customPlot->replot();
  }
}

void MainWindow::removeAllGraphs()
{
  ui->customPlot->clearGraphs();
  ui->customPlot->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->customPlot->selectedGraphs().size() > 0)
      menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    if (ui->customPlot->graphCount() > 0)
      menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));

  menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::selectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable)
{
  ui->statusBar->showMessage(QString("Clicked on graph '%1'.").arg(plottable->name()), 2000);
}

/*************************************************************************************************************/
/************************************* FUNCTIONALITY OF 3 SAMPLE BUTTONS *************************************/
/*************************************************************************************************************/

//---------------------------------------------------------------------------------Anodic Stripping
// Example Instruction "anoStrip0.101.005000,"
//      Start Volt (0.00 Volts)    ASstartVolt  float (must be 3 digits)
//      Peak Volt  (1.00 Volts)    ASpeakVolt   float (must be 3 digits)
//      Scan Rate  (500 mV/S)      ASscanRate   float (must be 3 digits)
//      Wave Type  (  0 - constant )            int   (must be 1 digit)
//                   1 - sin wave
//                   2 - triangle wave
//

void MainWindow::sampASPressed()
{
    serial.write("anoStrip0.001.005000,");
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();

    QTimer::singleShot(600, this, SLOT(parseAndPlot()));

    //ui->sampButton->setText(QString("Resample"));
}

// Cyclic Voltametry
//---------------------------------------------------------------------------------Cyclic Voltammetry
// Example Instruction "cycVolt0.101.001001,"
//      Start Volt (0.10 Volts)    CVstartVolt  float (must be 3 digits)
//      Peak Volt  (1.00 Volts)    CVpeakVolt   float (must be 3 digits)
//      Scan Rate  (100 mV/S)      CVscanRate   float (must be 3 digits)
//      Wave Type (  0 - constant  )            int   (must be 1 digit)
//                   1 - sin wave
//                   2 - triangle wave
//

void MainWindow::sampCVPressed()
{
    serial.write("cycVolt0.101.009992,");
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();

    QTimer::singleShot(1800, this, SLOT(parseAndPlot()));

    //ui->sampButton->setText(QString("Resample"));
}

//---------------------------------------------------------------------------------Potentiostatic Amperometry
// Example Instruction "potAmpero1.000.60,"
//      Sampling Time     (1.00 seconds) AsampTime  float
//      Potential Voltage (0.60 Volts)   PApoten    float
//

void MainWindow::sampPAPressed()
{
    serial.write("potAmpero1.000.80,");
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();

    QTimer::singleShot(600, this, SLOT(parseAndPlot()));

    //ui->sampButton->setText(QString("Resample"));
}



/*************************************************************************************************************/
/***************************** READ DATA FROM SERIAL PORT AND GRAPH THE VALUES *******************************/
/*************************************************************************************************************/

void MainWindow::parseAndPlot()
{
    statusBar()->clearMessage();
    QString inByteArray;

    float x = 0;
    double y = 0;

    QVector<double> xValues(3600), yValues(3600);

    for (int i = 0; i<3600; i++) {
        inByteArray = serial.readLine();  // This can obviously work better.
        y = inByteArray.toDouble();       // The array length should be calculated before hand, or
        xValues[i] = x;                   // we can send over the number of samples on the first line from the teensy
        yValues[i] = y;                   // Connect a signal to canReadLine() perhaps?
        x += 0.5;                         // I'd like to discuss this at the next meeting
    }

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(xValues, yValues);
    ui->customPlot->replot();
}

/*************************************************************************************************************/
/************************************ FUNCTIONALITY OF RECONNECT BUTTON **************************************/
/*************************************************************************************************************/

void MainWindow::reconnectButtonPressed()
{
    ui->statusBar->clearMessage();
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();

    serial.close();
    setUpComPort();
}

/*************************************************************************************************************/
/************************************** FUNCTIONALITY OF CLEAR BUTTON ****************************************/
/*************************************************************************************************************/

void MainWindow::clearButtonPressed()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();

}

/*************************************************************************************************************/
/***************************************** CREATE MENU FUNCTIONS *********************************************/
/*************************************************************************************************************/



/*************************************************************************************************************/
/********************** FILLS MENU FOR SERIAL PORT INFO ON SETTINGS TAB IN GUI *********************/
/*************************************************************************************************************/

//void MainWindow::fillPortsInfo()
//{
 //   ui->serialPortInfoListBox->clear();
   // static const QString blankString = QObject::tr("N/A");
//    QString description;
//    QString manufacturer;

 //   foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
      //  QStringList list;
//        description = info.description();
//        manufacturer = info.manufacturer();
     //   list << info.portName();
//             << (!description.isEmpty() ? description : blankString)
//             << (!manufacturer.isEmpty() ? manufacturer : blankString)
//             << info.systemLocation();

 //       ui->serialPortInfoListBox->addItem(list.first(), list);
//    }
//}

/*************************************************************************************************************/
/*********************************************** DESTRUCTOR **************************************************/
/*************************************************************************************************************/

MainWindow::~MainWindow()
{
    delete ui;
    serial.close();
}


