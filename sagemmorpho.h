#ifndef SAGEMMORPHO_H
#define SAGEMMORPHO_H

#include <QWidget>

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

signals:
    void getData(const QByteArray &data);

public slots:
    void clear();

private:
    Ui::SagemMorpho *ui;
};

#endif // SAGEMMORPHO_H
