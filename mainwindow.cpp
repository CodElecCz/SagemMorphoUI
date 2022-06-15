#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "sagemmorpho.h"
#include "support/style/CustomStyle.h"

#include <QLabel>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_console(new SagemMorpho(this)),
    m_settings(new SettingsDialog),
    m_serial(new QSerialPort(this)),
    m_statusLabel(new QLabel(this))
{
    m_ui->setupUi(this);
    m_console->setEnabled(false);

    setCentralWidget(m_console);

    readAppSettings();

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->actionBreak->setEnabled(false);
    m_ui->action_BDR0->setEnabled(false);
    m_ui->action_BDR1->setEnabled(false);
    m_ui->action_SAG0->setEnabled(false);
    m_ui->action_SAG1->setEnabled(false);
    m_ui->action_TRACE->setEnabled(false);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    m_ui->statusBar->addPermanentWidget(m_statusLabel);

    //apply default
    qApp->setStyle(new CustomStyle(ECustomStyle_Light));

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_console, &SagemMorpho::getData, this, &MainWindow::writeData);
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);

    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        m_ui->actionBreak->setEnabled(true);
        m_ui->action_BDR0->setEnabled(true);
        m_ui->action_BDR1->setEnabled(true);
        m_ui->action_SAG0->setEnabled(true);
        m_ui->action_SAG1->setEnabled(true);
        m_ui->action_TRACE->setEnabled(true);

        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }   
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();

    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->actionBreak->setEnabled(false);
    m_ui->action_BDR0->setEnabled(false);
    m_ui->action_BDR1->setEnabled(false);
    m_ui->action_SAG0->setEnabled(false);
    m_ui->action_SAG1->setEnabled(false);
    m_ui->action_TRACE->setEnabled(false);

    showStatusMessage(tr("Disconnected"));
}

void MainWindow::breakSerialPort()
{
    m_console->sendBreak();
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void MainWindow::writeData(const QByteArray &data)
{
    m_tx += data.length();
    m_statusLabel->setText(QString("tx:%1 bytes; rx:%2 bytes").arg(m_tx).arg(m_rx));

    m_serial->write(data);
}

void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();
    m_rx += data.length();
    m_statusLabel->setText(QString("tx:%1 bytes; rx:%2 bytes").arg(m_tx).arg(m_rx));

    m_console->putData(data);
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::clearData()
{
    m_tx = 0;
    m_rx = 0;
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionBreak, &QAction::triggered, this, &MainWindow::breakSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &SagemMorpho::clear);
    connect(m_ui->actionClear, &QAction::triggered, this, &MainWindow::clearData);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(m_ui->action_SAG0, &QAction::triggered, m_console, &SagemMorpho::sendSag0);
    connect(m_ui->action_SAG1, &QAction::triggered, m_console, &SagemMorpho::sendSag1);
    connect(m_ui->action_BDR0, &QAction::triggered, m_console, &SagemMorpho::sendBdr0);
    connect(m_ui->action_BDR1, &QAction::triggered, m_console, &SagemMorpho::sendBdr1);
    connect(m_ui->action_TRACE, &QAction::triggered, m_console, &SagemMorpho::sendTrace);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeAppSettings();
}

void MainWindow::readAppSettings()
{
    QSettings settings("QT", "SagemClient");

    settings.beginGroup("MainWindow");

    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("WindowState").toByteArray());

    settings.endGroup();
}

void MainWindow::writeAppSettings()
{
    QSettings settings("QT", "SagemClient");

    settings.beginGroup("MainWindow");

    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());

    settings.endGroup();
}

