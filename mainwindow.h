#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SagemMorpho;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void breakSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void clearData();

    void handleError(QSerialPort::SerialPortError error);

    void on_actionACK_disable_triggered(bool checked);

private:
    void initActionsConnections();
    void readAppSettings();
    void writeAppSettings();

    void closeEvent(QCloseEvent *event) override;

private:
    void showStatusMessage(const QString &message);

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    SagemMorpho *m_console = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
    QLabel *m_statusLabel = nullptr;
    uint32_t m_tx = 0;
    uint32_t m_rx = 0;
};

#endif // MAINWINDOW_H
