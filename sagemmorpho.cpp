#include "sagemmorpho.h"
#include "console.h"
#include "Morpho/morpho.h"
#include "Morpho/morpho_get_descriptor.h"

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
        ui->console->putDataHex(data, false);

        m_response.append(data);
        const uint8_t* cdata = (const uint8_t*)m_response.data();
        size_t cdataSize = m_response.length();

        const uint8_t* response = NULL;
        size_t responseSize = 0;

        int err = MORPHO_CheckResponse(cdata, cdataSize, &response, &responseSize);
        if(err != MORPHO_OK)
        {
            if(err == MORPHO_ERR_RESPONSE_NACK)
            {
                m_request = MorphoRequest_None;
                return;
            }
        }

        if(responseSize>0)
        {
            uint8_t ilvStatus = ILV_OK;
            const uint8_t* value = NULL;
            size_t valueSize = 0;

            err = MORPHO_CheckResponseData(response, responseSize, &value, &valueSize);
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
                }
                break;
            }

            uint8_t data[4];
            size_t dataSize = sizeof(data);
            MORPHO_ResponseAck(data, &dataSize);

            QByteArray request;
            request.append((char*)data, dataSize);

            //ui->console->putDataHex(m_response);
            ui->console->putDataRaw("\r\n");
            ui->console->setDataHex(request);

            if(err==MORPHO_OK)
            {

            }
            else
            {
                switch(err)
                {
                case MORPHO_WARN_VAL_ILV_STATUS:

                    break;
                default:

                    break;
                }

            }

            m_request = MorphoRequest_None;
        }
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

