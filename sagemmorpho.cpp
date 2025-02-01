#include "sagemmorpho.h"
#include "console.h"

#include "Morpho/morpho_protocol.h"
#include "Morpho/morpho_get_descriptor.h"
#include "Morpho/morpho_configure_uart.h"
#include "Morpho/morpho_add_base_record.h"
#include "Morpho/morpho_get_data.h"
#include "Morpho/morpho_get_base_config.h"
#include "Morpho/morpho_erase_base.h"
#include "Morpho/morpho_destroy_base.h"
#include "Morpho/morpho_create_base.h"
#include "Morpho/morpho_identify.h"
#include "Morpho/morpho_cancel.h"
#include "Morpho/morpho_get_public_fields.h"
#include "Morpho/morpho_async_message.h"
#include "Morpho/Ilv_errors.h"

#include "ui_sagemmorpho.h"

//#include <QCryptographicHash>

SagemMorpho::SagemMorpho(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SagemMorpho),
    m_request(MorphoRequest_None),
    m_userId(1),
    m_repeat(1),
    m_receiveState(ReceiveState_ReceiveSOP),
    m_ackDisable(false)
{
    ui->setupUi(this);

    connect(ui->console, &Console::getData, this, &SagemMorpho::getData);
}

SagemMorpho::~SagemMorpho()
{
    delete ui;
}

void SagemMorpho::receiveSOP()
{
    size_t responseSize = m_response.length();
    uint8_t packet[128];
    size_t packetSize = (responseSize < sizeof(packet))? responseSize : sizeof(packet);

    memset(packet, 0, sizeof(packet));
    memcpy(packet, m_response.data(), packetSize);

    //check ack-nack
    size_t sopSize = 0;
    uint8_t rc = 0;
    uint8_t flag = 0;
    int err = MORPHO_ReceiveSOP(packet, packetSize, &rc, &flag, &sopSize, 0);
    if(err != MORPHO_OK)
    {
        if(err == MORPHO_ERR_RESPONSE_NACK)
        {
            ui->console->putDataHex(m_response);

            m_request = MorphoRequest_None;
            return;
        }
    }
    else if(err == MORPHO_OK)
    {
        ui->console->putDataHex(m_response.mid(0, sopSize));
        m_response.remove(0, sopSize);

        m_receiveState = ReceiveState_ReceiveData;
    }
}

void SagemMorpho::receiveData()
{
    size_t responseSize = m_response.length();
    uint8_t packet[2048];
    size_t packetSize = (responseSize < sizeof(packet))? responseSize : sizeof(packet);

    memset(packet, 0, sizeof(packet));
    memcpy(packet, m_response.data(), packetSize);

    //check data
    uint8_t* value = NULL;
    size_t valueSize = 0;
    uint8_t identifier = 0;

    int err = MORPHO_ReceiveData(packet, packetSize, &identifier, &value, &valueSize);
    if(err != MORPHO_OK)
    {
        if(err == MORPHO_WARN_DATA_CONTINUE)
        {
            ui->console->putDataHex(m_response);

            m_responseExt.append(QByteArray::fromRawData((const char*)value, valueSize));
            m_response.clear();

            //SMorphoProtocol morpho = MORPHO_GetProtocol();
            //QString sfield = QString("MORPHO frame append index: %1, size: %2, sum: %3, total: %4 \r\n").arg(morpho.PacketIndex).arg(valueSize).arg(morpho.PacketSizeAct).arg(morpho.PacketSizeTotal);
            //ui->console->putData(sfield.toUtf8(), false);

            if(!m_ackDisable)
                ack();
        }

        return;
    }
    else
    {
        ui->console->putDataHex(m_response);

        m_responseExt.append(QByteArray::fromRawData((const char*)value, valueSize));
        m_response.clear();
        m_response.append(m_responseExt);
        m_responseExt.clear();

        //SMorphoProtocol morpho = MORPHO_GetProtocol();
        //QString sfield = QString("MORPHO frame final index: %1, size: %2, sum: %3, total: %4 \r\n").arg(morpho.PacketIndex).arg(valueSize).arg(morpho.PacketSizeAct).arg(morpho.PacketSizeTotal);
        //ui->console->putData(sfield.toUtf8(), false);

        if(!m_ackDisable)
            ack();
    }

    uint8_t ilvErr = ILV_OK;
    uint8_t ilvStatus = ILVSTS_OK;    

    switch(identifier)
    {
    case MorphoRequest_GetDescriptor:
        {
            uint8_t format = 0;
            err = MORPHO_GetDescriptor_Response(value, valueSize, &ilvErr, &format);

            switch(format)
            {
            case EMorpho_GetDescriptorFomat_Text:
                {
                const char* product = NULL;
                const char* sensor = NULL;
                const char* software = NULL;

                MORPHO_GetDescriptorText_Response(value, valueSize, &ilvErr, &product, &sensor, &software);
                if(product)
                    ui->console->putData(product, false);
                if(sensor)
                    ui->console->putData(sensor, false);
                if(software)
                    ui->console->putData(software, false);
                }
                break;
            case EMorpho_GetDescriptorFomat_BinVer:
                {
                const char* version = NULL;
                uint16_t versionSize = 0;
                MORPHO_GetDescriptorBinVer_Response(value, valueSize, &ilvErr, &version, &versionSize);
                if(version)
                {
                    char ver[32] = {};
                    memcpy(ver, version, versionSize);
                    ui->console->putData(ver, false);
                }
                }
                break;
            case EMorpho_GetDescriptorFomat_BinMaxUser:
                {
                uint16_t maxUser = 0;
                MORPHO_GetDescriptorBinMaxUser_Response(value, valueSize, &ilvErr, &maxUser);
                QString sfield = QString("MaxUser: %1").arg(maxUser);
                ui->console->putData(sfield.toUtf8(), false);
                }
                break;
            }

            ui->console->putDataRaw("\r\n");
        }
        break;
    case MorphoRequest_GetBaseConfig:
        {
            SMorpho_GetBaseConfig params = {};
            err = MORPHO_GetBaseConfig_Response(value, valueSize, &ilvErr, &params);

            QString sparams = QString("GetBaseConfig fingerNb:%1; recNb:%2 %3/%4; fieldNb:%5\r\n").arg(params.fingerNb).arg(params.recNb).arg(params.recNbFree).arg(params.recNbMax).arg(params.fieldNb);
            ui->console->putData(sparams.toUtf8(), false);
            for(int i = 0; i < params.fieldNb; i++)
            {
                QString sfield = QString("PublicField[%1] size: %2; name: '%3'\r\n").arg(i).arg(params.fieldDescription[i].size).arg(params.fieldDescription[i].name);
                ui->console->putData(sfield.toUtf8(), false);
            }
        }
        break;
    case MorphoRequest_AddBaseRecord:
        {            
            uint32_t userIndex = 0;

            err = MORPHO_AddBaseRecord_Response(value, valueSize, &ilvErr, &ilvStatus, &userIndex);            
        }
        break;
    case MorphoRequest_GetData:
        {
            const char* userData = NULL;
            err = MORPHO_GetData_Response(value, valueSize, &ilvErr, &userData);

            if(userData)
            {
                QString sfield = QString("Data: '%3'\r\n").arg(userData);
                ui->console->putData(sfield.toUtf8(), false);
            }
        }
        break;
    case MorphoRequest_ConfigureUart:
        {
            err = MORPHO_ConfigureUart_Response(value, valueSize, &ilvErr);
        }
        break;
    case MorphoRequest_EraseBase:
        {
            err = MORPHO_EraseBase_Response(value, valueSize, &ilvErr);
        }
        break;
    case MorphoRequest_DestroyBase:
        {
            err = MORPHO_DestroyBase_Response(value, valueSize, &ilvErr);
        }
        break;
    case MorphoRequest_CreateBase:
        {
            err = MORPHO_CreateBase_Response(value, valueSize, &ilvErr);
        }
        break;
    case MorphoRequest_GetPublicFields:
        {
            SMorpho_GetPublicFields params = {};

            err = MORPHO_GetPublicFields_Response((const uint8_t*)m_response.data(), m_response.size(), &ilvErr, &params);

            QString sfield = QString("PublicField fieldNb:'%1'\r\n").arg(params.fieldNb);
            ui->console->putData(sfield.toUtf8(), false);

            for(uint32_t i = 0; i < params.fieldNb; i++)
            {
                QString sfield = QString("PublicField[%1] name:'%2'\r\n").arg(i).arg(params.fieldDescription[i]);
                ui->console->putData(sfield.toUtf8(), false);
            }
        }
        break;
    case MorphoRequest_Identify:
        {
            uint32_t userIndex = 0;
            const char* userId = NULL;

            err = MORPHO_Identify_Response(value, valueSize, &ilvErr, &ilvStatus, &userIndex, &userId);
        }
        break;
    case MorphoRequest_AsyncMessage:
        {
            const char* msg = NULL;
            err = MORPHO_AsyncMeassage_Response(value, valueSize, &ilvErr, &msg);
            if(msg)
            {
                ui->console->putData(msg, false);
                ui->console->putDataRaw("\r\n");
            }
        }
        break;
    default:
        {
            QString stat = QString("Unexpected data [id:%1]\r\n").arg(identifier);
            ui->console->putDataRaw(stat.toUtf8());
        }
        break;
    }

    if(err != MORPHO_OK)
    {
        switch(err)
        {
        case MORPHO_WARN_VAL_ILV_ERROR:
            {
                QString ilv = QStringLiteral("0x%1").arg(ilvErr, 2, 16, QLatin1Char('0'));
                QString stat = QString("ILV err [%1] - %2\r\n").arg(ilv).arg(QString((const char*)IlvConvertError(ilvErr)));
                ui->console->putDataRaw(stat.toUtf8());
            }
            break;
        case MORPHO_WARN_VAL_ILV_STATUS:
            {
                QString ilv = QStringLiteral("0x%1").arg(ilvStatus, 2, 16, QLatin1Char('0'));
                QString stat = QString("ILV status [%1] - %2\r\n").arg(ilv).arg(QString((const char*)IlvConvertStatus(ilvStatus)));
                ui->console->putDataRaw(stat.toUtf8());
            }
            break;
        default:

            break;
        }

    }

    //repeat
    switch(m_request)
    {
    case MorphoRequest_AddBaseRecord:
        m_repeat--;
        if(m_repeat > 0)
        {
            addRecord(++m_userId);
        }
        else
        {
            m_repeat = 1;
            m_request = MorphoRequest_None;
        }
        break;
    default:
        m_repeat = 1;
        m_request = MorphoRequest_None;
        break;
    }
}

void SagemMorpho::ack()
{
    //ACK
    uint8_t ack[4];
    size_t ackSize = sizeof(ack);
    MORPHO_ResponseAck(ack, &ackSize);

    QByteArray request;
    request.append((char*)ack, ackSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::putData(const QByteArray &data)
{       
    //process response
    if( m_request != MorphoRequest_None)
    {        
        //data
        m_response.append(data);       

        switch(m_receiveState)
        {
        case ReceiveState_ReceiveSOP:
            receiveSOP();           

            //check if data
            if(m_receiveState == ReceiveState_ReceiveData)
            {
                receiveData();
            }
            break;
        case ReceiveState_ReceiveData:
            //ui->console->putDataHex(data, false);
            //ui->console->putData(data, false);
            receiveData();
            break;
        }
    }
    else
    {
        if(data.at(0) == 0x02)
        {
            //unexpected data
            if(data.size() > 3 &&
                    (uint8_t)data.at(0) == STX &&
                    (uint8_t)data.at(1) == 0xE1 && //data packet
                    (uint8_t)data.at(data.size() - 1) == ETX)
            {
                //data
                m_response.clear();
                m_response.append(data);

                receiveData();
            }
            else
            {
                ui->console->putDataHex(data);
            }
        }
        else
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

void SagemMorpho::sendTrace()
{
    m_request = MorphoRequest_None;

    ui->console->setData("$TRACE\r\n");
}

void SagemMorpho::sendIdentify0()
{
    m_request = MorphoRequest_None;

    uint8_t rc = MORPHO_GetCounter();

    uint8_t next = (uint8_t)(rc + 1);
    MORPHO_SetCounter(next);

    QString cmd = QString("$IDF00%1%2\r\n").arg(rc, 3, 10, QLatin1Char('0')).arg("05");
    ui->console->setData(QByteArray::fromStdString(cmd.toStdString()));
}

void SagemMorpho::sendCancel0()
{
    m_request = MorphoRequest_None;

    uint8_t rc = MORPHO_GetCounter();

    uint8_t next = (uint8_t)(rc + 1);
    MORPHO_SetCounter(next);

    QString cmd = QString("$IDC00%1\r\n").arg(rc, 3, 10, QLatin1Char('0'));
    ui->console->setData(QByteArray::fromStdString(cmd.toStdString()));
}

void SagemMorpho::sendIdentify1()
{
    m_request = MorphoRequest_None;

    uint8_t rc = MORPHO_GetCounter();

    uint8_t next = (uint8_t)(rc + 1);
    MORPHO_SetCounter(next);

    QString cmd = QString("$IDF01%1%2\r\n").arg(rc, 3, 10, QLatin1Char('0')).arg("05");
    ui->console->setData(QByteArray::fromStdString(cmd.toStdString()));
}

void SagemMorpho::sendCancel1()
{
    m_request = MorphoRequest_None;

    uint8_t rc = MORPHO_GetCounter();

    uint8_t next = (uint8_t)(rc + 1);
    MORPHO_SetCounter(next);

    QString cmd = QString("$IDC01%1\r\n").arg(rc, 3, 10, QLatin1Char('0'));
    ui->console->setData(QByteArray::fromStdString(cmd.toStdString()));
}

void SagemMorpho::on_desciptorButton_clicked()
{
    m_request = MorphoRequest_GetDescriptor;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetDescriptorText_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_desciptorButton_2_clicked()
{
    m_request = MorphoRequest_GetDescriptor;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetDescriptorBinVer_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_desciptorButton_3_clicked()
{
    m_request = MorphoRequest_GetDescriptor;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetDescriptorBinMaxUser_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_configUartButton_clicked()
{
    m_request = MorphoRequest_ConfigureUart;
    m_receiveState = ReceiveState_ReceiveSOP;

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
    m_userId = ui->userStartBox->value();
    m_repeat = ui->userCountBox->value();

    addRecord(m_userId);
}

void SagemMorpho::addRecord(int userId)
{
    m_request = MorphoRequest_AddBaseRecord;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    size_t tmplateSize = sizeof(tmplate);
    uint8_t tmplateId = ID_PK_COMP;

    uint8_t no_check = 1;

    QString user = QStringLiteral("%1").arg(userId, 16, 10, QLatin1Char('0'));

    //QByteArray hash = QCryptographicHash::hash(QByteArray((const char*)tmplate, sizeof(tmplate)), QCryptographicHash::Sha256);
    //hash.truncate(16);

    //user data
    size_t userDataSize = 1;
    QString userData0 = user;
    char cuserData0[32+1] = {};
    strcpy(cuserData0, userData0.toStdString().c_str());
    const char* userData[] = {cuserData0};

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_AddBaseRecord_Request(data, &dataSize, tmplate, tmplateSize, tmplateId, user.toStdString().c_str(), userData, userDataSize, no_check);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}


void SagemMorpho::on_eraseBaseButton_clicked()
{
    m_request = MorphoRequest_EraseBase;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_EraseBase_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_destroyBaseButton_clicked()
{
    m_request = MorphoRequest_DestroyBase;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_DestroyBase_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_createBaseButton_clicked()
{
    m_request = MorphoRequest_CreateBase;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    uint16_t nbRec = ui->dbsRecNbBox->value();
    MORPHO_CreateBase_Request(data, &dataSize, nbRec);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}


void SagemMorpho::on_getBaseConfigButton_clicked()
{
    m_request = MorphoRequest_GetBaseConfig;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetBaseConfig_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_getPublicFieldsButton_clicked()
{
    m_request = MorphoRequest_GetPublicFields;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();
    m_responseExt.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetPublicFields_Request(data, &dataSize, 0);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_getPublicFieldsButton_2_clicked()
{
    m_request = MorphoRequest_GetPublicFields;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();
    m_responseExt.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_GetPublicFields_Request(data, &dataSize, 1);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_getDataButton_clicked()
{
    m_request = MorphoRequest_GetData;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();
    m_responseExt.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    uint16_t indexField = ui->indexFieldBox->value();
    uint16_t indexUser = ui->indexUserBox->value();
    MORPHO_GetData_Request(data, &dataSize, indexField, indexUser);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

void SagemMorpho::on_identifyButton_clicked()
{
    m_request = MorphoRequest_Identify;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    uint16_t keepAlive = 0;
    if(ui->keepAliveBox->isChecked())
        keepAlive = 10;

    MORPHO_Identify_Request(data, &dataSize, 0, 5, keepAlive);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}


void SagemMorpho::on_cancelButton_clicked()
{
    m_request = MorphoRequest_Cancel;
    m_receiveState = ReceiveState_ReceiveSOP;

    m_response.clear();

    uint8_t data[1024];
    size_t dataSize = sizeof(data);
    MORPHO_Cancel_Request(data, &dataSize);

    QByteArray request;
    request.append((char*)data, dataSize);

    ui->console->setDataHex(request);
}

