#include "sagemmorpho.h"
#include "console.h"

#include "Morpho/morpho.h"
#include "Morpho/morpho_get_descriptor.h"
#include "Morpho/morpho_configure_uart.h"
#include "Morpho/morpho_add_base_record.h"
#include "Morpho/morpho_get_data.h"
#include "Morpho/morpho_get_base_config.h"
#include "Morpho/Ilv_errors.h"

#include "ui_sagemmorpho.h"

SagemMorpho::SagemMorpho(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SagemMorpho),
    m_request(MorphoRequest_None)
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
    //process response
    if( m_request != MorphoRequest_None)
    {
        //ui->console->putDataHex(data, false);

        //data
        m_response.append(data);       
        size_t packetSize = m_response.length();
        uint8_t packet[2048];
        memcpy(packet, m_response.data(), packetSize);

        //check ack-nack
        size_t sopSize = 0;
        uint8_t rc = 0;
        int err = MORPHO_ReciveSOP(packet, packetSize, &rc, &sopSize);
        if(err != MORPHO_OK)
        {
            if(err == MORPHO_ERR_RESPONSE_NACK)
            {
                m_request = MorphoRequest_None;
                return;
            }
        }

        //check data
        uint8_t ilvStatus = ILV_OK;
        uint8_t* value = NULL;
        size_t valueSize = 0;

        err = MORPHO_ReceiveData(&packet[sopSize], (packetSize - sopSize), &value, &valueSize);
        if(err != MORPHO_OK)
        {
            return;
        }

        switch(m_request)
        {
        case MorphoRequest_GetDescriptor:
            {
            const char* product = NULL;
            const char* sensor = NULL;
            const char* software = NULL;

            err = MORPHO_GetDescriptor_Response(value, valueSize, &ilvStatus, &product, &sensor, &software);
            if(product)
                ui->console->putData(product, false);
            if(sensor)
                ui->console->putData(sensor, false);
            if(software)
                ui->console->putData(software, false);
            }
            break;
        case MorphoRequest_GetBaseConfig:
            {
                err = MORPHO_GetBaseConfig_Response(value, valueSize, &ilvStatus);
            }
            break;
        case MorphoRequest_AddBaseRecord:
            {
                uint8_t baseStatus = 0;
                uint32_t userIndex = 0;

                err = MORPHO_AddBaseRecord_Response(value, valueSize, &ilvStatus, &baseStatus, &userIndex);
            }
            break;
        case MorphoRequest_GetData:
            {
                const char* userData = NULL;
                err = MORPHO_GetData_Response(value, valueSize, &ilvStatus, &userData);
            }
            break;
        case MorphoRequest_ConfigureUart:
            {
                err = MORPHO_ConfigureUart_Response(value, valueSize, &ilvStatus);
            }
            break;
        default:
            break;
        }

        //ui->console->putDataHex(m_response);
        ui->console->putDataRaw("\r\n");

        if(err==MORPHO_OK)
        {

        }
        else
        {
            switch(err)
            {
            case MORPHO_WARN_VAL_ILV_STATUS:
                {
                    QString ilv = QStringLiteral("0x%1").arg(ilvStatus, 2, 16, QLatin1Char('0'));
                    QString stat = QString("ILV status [%1] - %2\r\n").arg(ilv).arg(QString((const char*)IlvConvertError(ilvStatus)));
                    ui->console->putDataRaw(stat.toUtf8());
                }
                break;
            default:

                break;
            }

        }

        //ACK
        uint8_t ack[4];
        size_t ackSize = sizeof(data);
        MORPHO_ResponseAck(ack, &ackSize);

        QByteArray request;
        request.append((char*)ack, ackSize);

        ui->console->setDataHex(request);

        m_request = MorphoRequest_None;
    }
    else
    {
        ui->console->putData(data);
    }
}

void SagemMorpho::setLocalEchoEnabled(bool set)
{
    ui->console->setLocalEchoEnabled(set);
}

void SagemMorpho::clear()
{
    ui->console->clear();
}

void SagemMorpho::sendBreak()
{
    m_request = MorphoRequest_None;

    ui->console->setData("$BRK\r\n");

    MORPHO_ResetCounter();
}

void SagemMorpho::sendSag0()
{
    m_request = MorphoRequest_None;

    ui->console->setData("$SAG00\r\n");
}

void SagemMorpho::sendSag1()
{
    m_request = MorphoRequest_None;

    ui->console->setData("$SAG01\r\n");
}

void SagemMorpho::sendBdr0()
{
    m_request = MorphoRequest_None;

    ui->console->setData("$BDR00\r\n");
}

void SagemMorpho::sendBdr1()
{
    m_request = MorphoRequest_None;

    ui->console->setData("$BDR01\r\n");
}

void SagemMorpho::on_desciptorButton_clicked()
{
    m_request = MorphoRequest_GetDescriptor;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetDescriptor_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}


void SagemMorpho::on_configUartButton_clicked()
{
    m_request = MorphoRequest_ConfigureUart;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_ConfigureUart_Request(data, &dataSize, 115200);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

static const uint8_t tmplate[] =
{
        0x82, 0x39, 0x00, 0x52, 0x5D, 0xFF, 0x80, 0x80, 0x91, 0x69, 0x9E, 0x66, 0x5C, 0xAE, 0x9C, 0x7E,
        0xA7, 0x7D, 0x59, 0x9F, 0x58, 0x95, 0x3F, 0x95, 0x4D, 0x7C, 0x4E, 0x8D, 0x67, 0x8D, 0x35, 0x88,
        0x43, 0x68, 0x71, 0x88, 0x6C, 0x76, 0x80, 0x88, 0xB6, 0x93, 0x60, 0x64, 0x7E, 0x81, 0xB0, 0x9F,
        0x55, 0x58, 0x69, 0x66, 0x9F, 0xA2, 0x86, 0x6B, 0x82, 0x65, 0x85, 0x5A, 0x8D, 0x7E, 0x85, 0xA8,
        0x94, 0x62, 0xA2, 0x5B, 0x6A, 0xB2, 0x9B, 0x4C, 0xA1, 0x75, 0xBC, 0x5E, 0xA1, 0x83, 0x74, 0x9B,
        0xAC, 0x88, 0x9D, 0x8E, 0x36, 0x79, 0x46, 0x8F, 0x4D, 0x88, 0x29, 0x81, 0x29, 0x97, 0x3C, 0x64,
        0x52, 0x74, 0x59, 0x8A, 0x73, 0x8A, 0x66, 0x7D, 0xAE, 0x95, 0x4C, 0x6C, 0x4F, 0x60, 0x53, 0x58,
        0x95, 0x9B, 0x7B, 0x7F, 0x87, 0x95, 0x82, 0x71, 0x8C, 0xB0, 0xBB, 0x3F, 0x6B, 0x87, 0x18, 0x91,
        0xB5, 0xC9, 0xB2, 0xDD, 0xDD, 0x52, 0x20, 0xB1, 0x59, 0xFF
};

void SagemMorpho::on_addRecordButton_clicked()
{
    m_request = MorphoRequest_AddBaseRecord;

    m_response.clear();

    size_t tmplateSize = sizeof(tmplate);
    uint8_t tmplateId = ID_PK_COMP;
    const char* userData[] = {"name", "surname"};
    size_t userDataSize = 2;
    uint8_t no_check = 1;

    int userId = ui->userStartBox->value();
    QString user = QStringLiteral("%1 ").arg(userId, 8, 10, QLatin1Char('0'));

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_AddBaseRecord_Request(data, &dataSize, tmplate, tmplateSize, tmplateId, user.toStdString().c_str(), userData, userDataSize, no_check);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

