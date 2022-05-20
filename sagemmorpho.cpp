#include "sagemmorpho.h"
#include "console.h"
#include "ui_sagemmorpho.h"

SagemMorpho::SagemMorpho(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SagemMorpho)
{
    ui->setupUi(this);

    connect(ui->console, &Console::getData, this, &SagemMorpho::getData);
}

SagemMorpho::~SagemMorpho()
{
    delete ui;
}


void SagemMorpho::putData(const QByteArray &data)
{
    ui->console->putData(data);
}

void SagemMorpho::setLocalEchoEnabled(bool set)
{
    ui->console->setLocalEchoEnabled(set);
}

void SagemMorpho::clear()
{
    ui->console->clear();
}


void SagemMorpho::on_desciptorButton_clicked()
{

}

