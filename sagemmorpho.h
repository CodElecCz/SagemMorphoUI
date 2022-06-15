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

signals:
    void getData(const QByteArray &data);

public slots:
    void clear();

    void sendSag0();
    void sendSag1();
    void sendBdr0();
    void sendBdr1();
    void sendTrace();

private slots:
    void on_desciptorButton_clicked();
    void on_configUartButton_clicked();
    void on_addRecordButton_clicked();
    void on_eraseBaseButton_clicked();
    void on_destroyBaseButton_clicked();
    void on_createBaseButton_clicked();

    void on_getBaseConfigButton_clicked();

private:
    void addRecord(int userId);
    void ack();

private:
    Ui::SagemMorpho*        ui;
    enum MorphoRequest		m_request;
    QByteArray              m_response;
    int                     m_userId;
    int                     m_repeat;
};

#endif // SAGEMMORPHO_H
