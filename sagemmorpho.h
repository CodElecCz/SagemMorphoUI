#ifndef SAGEMMORPHO_H
#define SAGEMMORPHO_H

#include <QWidget>
#include "Morpho/morpho_definitions.h"

namespace Ui {
class SagemMorpho;
}

class SagemMorpho : public QWidget
{
    Q_OBJECT

public:
    explicit SagemMorpho(QWidget *parent = nullptr);
    ~SagemMorpho();

    void putData(const QByteArray &data);
    void setLocalEchoEnabled(bool set);

    void sendBreak();

    enum ReceiveState
    {
        ReceiveState_ReceiveSOP = 0,
        ReceiveState_ReceiveData = 1
    };

signals:
    void getData(const QByteArray &data);

public slots:
    void clear();

    void sendSag0();
    void sendSag1();
    void sendBdr0();
    void sendBdr1();
    void sendTrace();
    void sendIdentify0();
    void sendCancel0();
    void sendIdentify1();
    void sendCancel1();
    void setAck(bool disabled) {m_ackDisable = disabled;}

private slots:
    void on_desciptorButton_clicked();
    void on_desciptorButton_2_clicked();
    void on_desciptorButton_3_clicked();
    void on_configUartButton_clicked();
    void on_addRecordButton_clicked();
    void on_eraseBaseButton_clicked();
    void on_destroyBaseButton_clicked();
    void on_createBaseButton_clicked();
    void on_getBaseConfigButton_clicked();
    void on_identifyButton_clicked();
    void on_cancelButton_clicked();
    void on_getPublicFieldsButton_clicked();
    void on_getPublicFieldsButton_2_clicked();
    void on_getDataButton_clicked();

private:
    void addRecord(int userId);
    void ack();

    void receiveSOP();
    void receiveData();

private:
    Ui::SagemMorpho*        ui;
    enum MorphoRequest		m_request;    
    QByteArray              m_response;
    QByteArray              m_responseExt;
    int                     m_userId;
    int                     m_repeat;
    ReceiveState            m_receiveState;
    bool                    m_ackDisable;
};

#endif // SAGEMMORPHO_H
