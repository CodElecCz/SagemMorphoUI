#include "Ilv_errors.h"
#include "Ilv_definitions.h"

#include <stdio.h>

// Error table struct
typedef struct
{
	uint8_t	ErrorCode;
	uint8_t  ErrorString[64];
} IVL_ERROR_TABLE;

// Error Table with string explanation
static IVL_ERROR_TABLE IlvErrorTable[] =   /*01234567890123456789012345678901234567890123456789*/
		{ 
			{ ILV_OK,						"Successful result"},								 		// 0x00
			{ ILVERR_ERROR,					"An error occurred" },										// 0xFF
			{ ILVERR_BADPARAMETER,			"Input parameters are not valid" }, 						// 0xFE
			{ ILVERR_INVALID_MINUTIAE,		"The minutiae is not valid" },      				 		// 0xFD
			{ ILVERR_INVALID_USER_ID,		"The record identifier does not exist in the dbs" },		// 0xFC
			{ ILVERR_INVALID_USER_DATA,		"The user data are not valid" },    			 			// 0xFB 
			{ ILVERR_TIMEOUT,				"No response after defined time" }, 						// 0xFA 
			{ ILVERR_INVALID_ID_PROTOCOL,	"The protocole used is not valid" },						// 0xF9
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
			{ ILVERR_CMDE_ABORTED,   		"Commanded has been aborted by the user" },					// 0xE5
			{ ILVERR_SAME_FINGER,   		"There are two templates of the same finger" },				// 0xE4
			{ ILVERR_NO_HIT,				"Presented finger does not match" },						// 0xE3
			{ ILVERR_FFD,					"False finger detected"},									// 0xDB 
			{ ILVERR_MOIST_FINGER,			"Too moist finger detected" },								// 0xDA
		};

static size_t	NbError = sizeof(IlvErrorTable)/sizeof(IVL_ERROR_TABLE);
static char IlvErrorNotFound[] = "Error not found";

/*****************************************************************************/
/*****************************************************************************/
const uint8_t* IlvConvertError(uint8_t error)
{
	int i;

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
