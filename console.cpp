#include "console.h"

#include <QScrollBar>
#include <QDateTime>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(50000);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, Qt::black);
    setPalette(p);
}

void Console::putData(const QByteArray &data, bool timestamp)
{
    QString text;
    if(timestamp)
        text.append(QString("[%1|Rx] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).arg(data.data()));
    else
        text = data;

    insertPlainText(text);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::putDataRaw(const QByteArray &data)
{
    insertPlainText(data);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::putDataHex(const QByteArray &data, bool timestamp)
{
    QString hex;
    if(timestamp)
        hex.append(QString("[%1|Rx] ").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));

    foreach(auto d, data)
    {
        uint8_t ud = (uint8_t)d;
        QString number = QStringLiteral("%1 ").arg(ud, 2, 16, QLatin1Char('0'));
        hex.append(number);
    }
    if(timestamp)
        hex.append('\n');

    insertPlainText(hex);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void Console::setData(const QByteArray &data)
{
    QString text;
    text.append(QString("[%1|Tx] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")).arg(data.data()));

    QPlainTextEdit::insertPlainText(text);
    emit getData(data);
}

void Console::setDataHex(const QByteArray &data)
{
    QString hex;
    hex.append(QString("[%1|Tx] ").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));

    foreach(auto d, data)
    {
        uint8_t ud = (uint8_t)d;
        QString number = QStringLiteral("%1 ").arg(ud, 2, 16, QLatin1Char('0'));
        hex.append(number);
    }
    hex.append('\n');

    QPlainTextEdit::insertPlainText(hex);

    emit getData(data);
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (m_localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
