void MainWindow::sampCVPressed()
{
    QString CVsv = QString::number(ui->CVstartVolt->value(),'f',2);
    QString CVpv = QString::number(ui->CVpeakVolt->value(),'f',2);
    QString CVsrBuf = QString::number(ui->CVscanRate->value());
    QString CVsr;

    switch (CVsrBuf.length()) {
    case 2: CVsr = "00" + CVsrBuf; break;
    case 3: CVsr = "0" + CVsrBuf; break;
    case 4: CVsr = CVsrBuf; break;
    }

    qDebug() << CVsr;

    QString mainInstructions = ("cycVolt"+CVsv+CVpv+CVsr+"2,");
    serial.write(mainInstructions.toStdString().c_str());

    qDebug() << mainInstructions.toStdString().c_str();

    //ui->customPlot->clearGraphs();
    ui->customPlot->replot();

    float CVpeakVolt = ui->CVpeakVolt->value();
    float CVstartVolt = ui->CVstartVolt->value();
    float CVscanRate = ui->CVscanRate->value();

    // MUST CONSIDER WAVE TYPE! - WILL MULTIPLY BY 2 IF USING TRIANGLE WAVE
    // switch case - no problem

    int CVsamples = round(2*2000.0*1000.0*(CVpeakVolt - CVstartVolt)/CVscanRate);
    // 2000 == sampling rate
    // 1000 == mV to Volts

    parseAndPlot(CVsamples);

    //ui->sampButton->setText(QString("Resample"));
}
