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

private slots:
    void on_desciptorButton_clicked();

private:
    Ui::SagemMorpho*        ui;
    enum MorphoRequest		m_request;
    QByteArray              m_response;
};

#endif // SAGEMMORPHO_H
