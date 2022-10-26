/* Refer to documentation for the meaning of commands */
#ifndef __ILV_DEFINITIONS_H
#define __ILV_DEFINITIONS_H

/* ------------------------------------------------------------
 * General commands
 */
#define ILV_GET_DESCRIPTOR          0x05

/* ------------------------------------------------------------
 * Biometric commands
 */
#define ILV_AUTHENTICATE            0x20
#define ILV_ENROLL                  0x21
#define ILV_IDENTIFY                0x22
#define ILV_VERIFY_MATCH            0x23
#define ILV_IDENTIFY_MATCH          0x24

/* ------------------------------------------------------------
 * DataBase commands
 */
#define ILV_GET_BASE_CONFIG         0x07
#define ILV_CREATE_DB               0x30
#define ILV_ERASE_BASE              0x32
#define ILV_ADD_RECORD				0x35
#define ILV_REMOVE_RECORD           0x36
#define ILV_DESTROY_DB              0x3B
#define ILV_LIST_PUBLIC_FIELDS      0x3E
#define ILV_GET_DATA                0x3F

#define ID_NO_CHECK_ON_TEMPLATE	    0x60	//96

/* ------------------------------------------------------------
 * Special ILV : answer to an invalid command
 */
#define ILV_INVALID                 0x50

/* ------------------------------------------------------------
 * Asynchronous messages
 */
#define ILV_CANCEL                  0x70
#define ILV_ASYNC_MESSAGE           0x71

/* ------------------------------------------------------------
 * Security
 */
#define ILV_SECU_GET_CONFIG        0x80

/* ------------------------------------------------------------
 * Special ILV
 */
#define ILV_CONFIG_UART            0xEE
#define ILV_EXTENDED               0xFE
#define ILV_ESCAPE                 0xFF

/* ------------------------------------------------------------
 * Asyn Message
 */
#define ID_MESSAGE_COMMAND_CMD          0x01
#define ID_MESSAGE_IMAGE_CMD            0x02
#define ID_MESSAGE_ENROLLMENT_CMD       0x04
#define ID_MESSAGE_IMAGE_FULL_RES_CMD   0x08
#define ID_MESSAGE_CODE_QUALITY_CMD     0x40
#define ID_MESSAGE_DETECT_QUALITY_CMD   0x80

/* ------------------------------------------------------------
 * Optional ILV
 */
#define ID_USER_ID                  4             //0x04
#define ID_USER_DATA                5             //0x05
#define ID_COM1                     6             //0x06
#define ID_PULBIC_FIELD             15            //0x0F
#define ID_PUC_DATA                 20            //0x14
#define ID_DESC_PRODUCT             41            //0x29
#define ID_DESC_SOFTWARE            42            //0x2A
#define ID_DESC_SENSOR              43            //0x2B
#define	ID_PRIVATE_FIELD            49            //0x31
#define	ID_FIELD_CONTENT            50            //0x32
#define ID_ASYNCHRONOUS_EVENT       52            //0x34
#define	ID_USER_INDEX               54            //0x36
#define ID_BIO_ALGO_PARAM           56            //0x38
#define ID_EXPORT_IMAGE             61            //0x3D
#define	ID_COMPRESSION              62            //0x3E
#define	ID_PKM1                     63            //0x3F
#define	ID_PKM1_PARAM               64            //0x40
#define	ID_PKM1_DATA                65            //0x41
#define	ID_X984_PARAM               84            //0x54
#define	ID_X984                     85            //0x55
#define ID_MATCHING_SCORE           86            //0x56
#define	ID_TKB                      87            //0x57
#define ID_ALIVE_MESSAGE_TIME       153           //0x99

//Conversion de PK normalise
#define ID_PK_COMP					2
#define ID_ISO_PK                   ID_PKM1        //0x3F
#define ID_ISO_PK_PARAM             ID_PKM1_PARAM  //0x40
#define ID_ISO_PK_DATA_ANSI_378     ID_PKM1_DATA   //0x41
#define ID_ISO_PK_DATA_ILO_FMR      107            //0x6B
#define ID_ISO_PK_DATA_ISO_FMC_CS   108            //0x6C
#define ID_ISO_PK_DATA_ISO_FMC_NS   109            //0x6D
#define ID_ISO_PK_DATA_ISO_FMR      110            //0x6E
#define ID_ISO_PK_DATA_MINEX_A      111            //0x6F


#define ID_COMPRESSION_NULL      44      ///< No image compression
#define ID_COMPRESSION_HUFFMAN   51      ///< Huffman compression
#define ID_COMPRESSION_V1        60      ///< Huffman compression V1
#define ID_COMPRESSION_WSQ		156 	 ///< WSQ compresion

/* Format for MSO_Get_Descriptor */
#define ID_FORMAT_TEXT                  47      ///< Text format for Get_Descriptor function
#define ID_FORMAT_BIN                   48      ///< Binary format for Get_Descriptor function

/*!
*  Status code return by function: o_puc_ILV_Status.
*
*/
#define ILV_OK                          0x00    ///< Successful result
#define ILVERR_ERROR                    0xFF    ///< An error occurred
#define ILVERR_BADPARAMETER             0xFE    ///< Input parameters are not valid
#define ILVERR_INVALID_MINUTIAE         0xFD    ///< The minutiae is not valid
#define ILVERR_INVALID_USER_ID          0xFC    ///< The record identifier does not exist in the database
#define ILVERR_INVALID_USER_DATA        0xFB    ///< The user data are not valid
#define ILVERR_TIMEOUT                  0xFA    ///< No response after defined time.
#define ILVERR_INVALID_ID_PROTOCOL      0xF9    ///< The protocole used is not valid
#define ILVERR_ALREADY_ENROLLED         0xF8    ///< The person is already in the base
#define ILVERR_BASE_NOT_FOUND           0xF7    ///< The specified base does not exist
#define ILVERR_BASE_ALREADY_EXISTS      0xF6    ///< The specified base already exist
#define ILVERR_BIO_IN_PROGRESS          0xF5    ///< Command received during biometric processing
#define ILVERR_CMD_INPROGRESS           0xF4    ///< Command received while another command is running
#define ILVERR_FLASH_INVALID            0xF3    ///< Flash type invalid
#define ILVERR_NO_SPACE_LEFT            0xF2    ///< Not Enough memory for the creation of a database
#define ILVERR_FIELD_NOT_FOUND          0xE9    ///< Field does not exist.
#define ILVERR_FIELD_INVALID            0xE8    ///< Field size or field name is invalid.
#define ILVERR_SECURITY_MODE            0xE7    ///< The request is not compatible with security mode.
#define ILVERR_USER_NOT_FOUND           0xE6    ///< The searched user is not found.
#define ILVERR_CMDE_ABORTED             0xE5    ///< Commanded has been aborted by the user.
#define ILVERR_SAME_FINGER              0xE4    ///< There are two templates of the same finger
#define ILVERR_NO_HIT                   0xE3    ///< Presented finger does not match
#define ILVERR_FFD                      0xDB    ///< False finger detected
#define ILVERR_MOIST_FINGER             0xDA    ///< Too moist finger detected
#define ILVERR_NOT_IMPLEMENTED          0x9D    ///< The request is not yet implemented.

/*!
* Status Codes definition :
*/
#define ILVSTS_OK                       0x00    ///< Successful
#define ILVSTS_HIT                      0x01    ///< Authentication or Identification succeed
#define ILVSTS_NO_HIT                   0x02    ///< Authentication or Identification failed
#define ILVSTS_LATENT                   0x03    ///< Security Protection Triggered
#define ILVSTS_DB_FULL                  0x04    ///< The database is full
#define ILVSTS_DB_EMPTY                 0x05    ///< The database is empty
#define ILVSTS_BAD_QUALITY              0x06    ///< Bad finger and/or enroll quality
#define ILVSTS_DB_OK                    0x07    ///< The database is right
#define ILVSTS_ACTIVATED                0x08    ///< The MorphoModule is activated
#define ILVSTS_NOTACTIVATED             0x09    ///< The MorphoModule is not activated
#define ILVSTS_DB_KO                    0x0A	///< The flash can not be accessed
#define ILVSTS_FFD                      0x22    ///< False finger detected
#define ILVSTS_MOIST_FINGER             0x23    ///< Too moist finger detected
#define ILVSTS_MOVED_FINGER 			0x24
#define ILVSTS_SATURATED_FINGER			0x25
#define ILVSTS_INVALID_FINGER 			0x26

#endif /*__ILV_DEFINITIONS_H*/

