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
int graphMemory = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->customPlot->xAxis->setRange(0, 1000);
    ui->customPlot->yAxis->setRange(0, 3.3);

    serial.setPortName("com6");
    if (serial.open(QIODevice::ReadWrite))
    {
        serial.setBaudRate(QSerialPort::Baud9600);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);
        ui->statusBar->showMessage(
                    QString("COM Port Successfully Linked"));

    }
    else{
        serial.close();
        ui->statusBar->showMessage(
                    QString("Unable to Reach COM Port"));

    }

    setupDemo(3);
    // 3:  setupScatterStyleDemo(ui->customPlot);


    // for making screenshots of the current demo or all demos (for website screenshots):
    //QTimer::singleShot(1500, this, SLOT(allScreenShots()));
    //QTimer::singleShot(4000, this, SLOT(screenShot()));
}

void MainWindow::setupDemo(int demoIndex)
{

    setupAldeSensGraph(ui->customPlot);
    setWindowTitle("aldeSens");

    currentDemoIndex = demoIndex;
    ui->customPlot->replot();
}




void MainWindow::setupAldeSensGraph(QCustomPlot *customPlot)
{
    for (int i=0; i<10; ++i) {
        customPlot->addGraph(); // blue line
        customPlot->graph(i)->setBrush(QBrush(QColor(240, 255, 200)));
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

        // connect slot that ties some axis selections together (especially opposite axes):
        connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

        // make left and bottom axes transfer their ranges to right and top axes:
        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

        connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    }

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

    void MainWindow::selectionChanged()
    {
        /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

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

    MainWindow::~MainWindow()
    {
        delete ui;
        serial.close();
    }

    void MainWindow::screenShot()
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        QPixmap pm = QPixmap::grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#else
        QPixmap pm = qApp->primaryScreen()->grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#endif
        QString fileName = demoName.toLower()+".png";
        fileName.replace(" ", "");
        pm.save("./screenshots/"+fileName);
        qApp->quit();
    }

    void MainWindow::allScreenShots()
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        QPixmap pm = QPixmap::grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#else
        QPixmap pm = qApp->primaryScreen()->grabWindow(qApp->desktop()->winId(), this->x()+2, this->y()+2, this->frameGeometry().width()-4, this->frameGeometry().height()-4);
#endif
        QString fileName = demoName.toLower()+".png";
        fileName.replace(" ", "");
        pm.save("./screenshots/"+fileName);

        if (currentDemoIndex < 18)
        {
            if (dataTimer.isActive())
                dataTimer.stop();
            dataTimer.disconnect();
            delete ui->customPlot;
            ui->customPlot = new QCustomPlot(ui->centralWidget);
            ui->verticalLayout->addWidget(ui->customPlot);
            setupDemo(currentDemoIndex+1);
            // setup delay for demos that need time to develop proper look:
            int delay = 250;
            if (currentDemoIndex == 10) // Next is Realtime data demo
                delay = 12000;
            else if (currentDemoIndex == 15) // Next is Item demo
                delay = 5000;
            QTimer::singleShot(delay, this, SLOT(allScreenShots()));
        } else
        {
            qApp->quit();
        }
    }





    void MainWindow::on_pushButton_clicked()
    {
        serial.write("cycVolt01223344,");

        QTimer::singleShot(1000, this, SLOT(parseAndPlot()));

    }

    void MainWindow::parseAndPlot()
    {
        statusBar()->clearMessage();
        QString inByteArray;

        float key = 0;
        double value0;
        graphMemory += 1;

        QVector<double> x(2000), y(2000);
        for (int q=0; q<50; ++q) {
            for (int k=0; k<40; ++k) {

                inByteArray = serial.readLine();

                value0 = inByteArray.toDouble();

                ui->customPlot->graph(graphMemory)->addData(key, value0);

                // set data of dots:
                //ui->customPlot->graph(2)->addData(key, value0);

                // remove data of lines that's outside visible range:

                // rescale value (vertical) axis to fit the current data:

                key += 0.5;

            }
            ui->customPlot->replot();
        }
        // make key axis range scroll with the data (at a constant range size of 8):



    }


