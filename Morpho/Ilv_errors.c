#include "Ilv_errors.h"
#include "Ilv_definitions.h"

#include <stdio.h>

// Error table struct
typedef struct
{
	uint8_t	ErrorCode;
    char  ErrorString[64];
} IVL_ERROR_TABLE;

// Error Table with string explanation
static IVL_ERROR_TABLE IlvErrorTable[] =   /*01234567890123456789012345678901234567890123456789*/
		{ 
			{ ILV_OK,						"Successful result"},								 		// 0x00
			{ ILVERR_ERROR,					"An error occurred" },										// 0xFF
			{ ILVERR_BADPARAMETER,			"Input parameters are not valid" }, 						// 0xFE
			{ ILVERR_INVALID_MINUTIAE,		"The minutiae is not valid" },      				 		// 0xFD
            { ILVERR_INVALID_USER_ID,		"The User ID is not valid" },                               // 0xFC
			{ ILVERR_INVALID_USER_DATA,		"The user data are not valid" },    			 			// 0xFB 
			{ ILVERR_TIMEOUT,				"No response after defined time" }, 						// 0xFA 
			{ ILVERR_INVALID_ID_PROTOCOL,	"The protocol used is not valid" },						// 0xF9
			{ ILVERR_ALREADY_ENROLLED,		"The person is already in the base"},						// 0xF8
			{ ILVERR_BASE_NOT_FOUND,		"The specified base does not exist"},   					// 0xF7
			{ ILVERR_BASE_ALREADY_EXISTS,	"The specified base already exist"},    					// 0xF6
			{ ILVERR_BIO_IN_PROGRESS,		"Command received during biometric processing"},			// 0xF5
			{ ILVERR_CMD_INPROGRESS,		"Command received while another command is running"},		// 0xF4
			{ ILVERR_FLASH_INVALID,			"Flash type invalid" },										// 0xF3
			{ ILVERR_NO_SPACE_LEFT,			"Not Enough memory for the creation of a dbs" },			// 0xF2
			{ ILVERR_FIELD_NOT_FOUND, 		"Field does not exist"  },   								// 0xE9
			{ ILVERR_FIELD_INVALID,  		"Field size or field name is invalid"  },					// 0xE8
			{ ILVERR_SECURITY_MODE,  		"The request is not compatible with security mode" },		// 0xE7
			{ ILVERR_USER_NOT_FOUND, 		"The searched user is not found" },							// 0xE6
			{ ILVERR_CMD_ABORTED,   		"Commanded has been aborted by the user" },					// 0xE5
			{ ILVERR_SAME_FINGER,   		"There are two templates of the same finger" },				// 0xE4
			{ ILVERR_NO_HIT,				"Presented finger does not match" },						// 0xE3
			{ ILVERR_FFD,					"False finger detected"},									// 0xDB 
			{ ILVERR_MOIST_FINGER,			"Too moist finger detected" },								// 0xDA
		};

static IVL_ERROR_TABLE IlvStatusTable[] =   /*01234567890123456789012345678901234567890123456789*/
        {
            { ILVSTS_OK,					"Successful"},                                              // 0x00
            { ILVSTS_HIT,					"Authentication or Identification succeeded" },				// 0x01
            { ILVSTS_NO_HIT,                "Authentication or Identification failed" },                // 0x02
            { ILVSTS_LATENT,                "Latent" },                                                 // 0x03
            { ILVSTS_DB_FULL,               "The database is full" },                                  // 0x04
            { ILVSTS_DB_EMPTY,              "The database is empty" },                                 // 0x05
            { ILVSTS_BAD_QUALITY,           "Bad finger and-or enroll quality" }, 						// 0x06
            { ILVSTS_DB_OK,                 "The database is right" },                                  // 0x07
            { ILVSTS_ACTIVATED,             "The MorphoModule is activated"},                           // 0x08
            { ILVSTS_NOTACTIVATED,          "The MorphoModule is not activated"},   					// 0x09
            { ILVSTS_DB_KO,                 "The flash can not be accessed"},                           // 0x10
        };

static size_t	NbError = sizeof(IlvErrorTable)/sizeof(IVL_ERROR_TABLE);
static size_t	NbStatus = sizeof(IlvStatusTable)/sizeof(IVL_ERROR_TABLE);

static const char IlvErrorNotFound[] = "Error not found";
static const char IlvStatusNotFound[] = "Status not found";

/*****************************************************************************/
/*****************************************************************************/
const char* IlvConvertError(uint8_t error)
{
    size_t i;

	for(i=0; i<NbError; i++)
	{
		if(IlvErrorTable[i].ErrorCode == error)
		{
			return IlvErrorTable[i].ErrorString;
		}
	}

	if( i == NbError)
	{
		return IlvErrorNotFound;
	}

	return NULL;
}

const char* IlvConvertStatus(uint8_t status)
{
    size_t i;

    for(i=0; i<NbStatus; i++)
    {
        if(IlvStatusTable[i].ErrorCode == status)
        {
            return IlvStatusTable[i].ErrorString;
        }
    }

    if( i == NbStatus)
    {
        return IlvStatusNotFound;
    }

    return NULL;
}
