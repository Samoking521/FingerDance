#ifndef MICROSD_H
#define MICROSD_H

#include <device.h>

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
    /** 
     * @brief  SDIO specific error defines  
     */
    SD_CMD_CRC_FAIL = (1), /*!< Command response received (but CRC check failed) */
    SD_DATA_CRC_FAIL = (2), /*!< Data bock sent/received (CRC check Failed) */
    SD_CMD_RSP_TIMEOUT = (3), /*!< Command response timeout */
    SD_DATA_TIMEOUT = (4), /*!< Data time out */
    SD_TX_UNDERRUN = (5), /*!< Transmit FIFO under-run */
    SD_RX_OVERRUN = (6), /*!< Receive FIFO over-run */
    SD_START_BIT_ERR = (7), /*!< Start bit not detected on all data signals in widE bus mode */
    SD_CMD_OUT_OF_RANGE = (8), /*!< CMD's argument was out of range.*/
    SD_ADDR_MISALIGNED = (9), /*!< Misaligned address */
    SD_BLOCK_LEN_ERR = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
    SD_ERASE_SEQ_ERR = (11), /*!< An error in the sequence of erase command occurs.*/
    SD_BAD_ERASE_PARAM = (12), /*!< An Invalid selection for erase groups */
    SD_WRITE_PROT_VIOLATION = (13), /*!< Attempt to program a write protect block */
    SD_LOCK_UNLOCK_FAILED = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
    SD_COM_CRC_FAILED = (15), /*!< CRC check of the previous command failed */
    SD_ILLEGAL_CMD = (16), /*!< Command is not legal for the card state */
    SD_CARD_ECC_FAILED = (17), /*!< Card internal ECC was applied but failed to correct the data */
    SD_CC_ERROR = (18), /*!< Internal card controller error */
    SD_GENERAL_UNKNOWN_ERROR = (19), /*!< General or Unknown error */
    SD_STREAM_READ_UNDERRUN = (20), /*!< The card could not sustain data transfer in stream read operation. */
    SD_STREAM_WRITE_OVERRUN = (21), /*!< The card could not sustain data programming in stream mode */
    SD_CID_CSD_OVERWRITE = (22), /*!< CID/CSD overwrite error */
    SD_WP_ERASE_SKIP = (23), /*!< only partial address space was erased */
    SD_CARD_ECC_DISABLED = (24), /*!< Command has been executed without using internal ECC */
    SD_ERASE_RESET = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
    SD_AKE_SEQ_ERROR = (26), /*!< Error in sequence of authentication. */
    SD_INVALID_VOLTRANGE = (27),
    SD_ADDR_OUT_OF_RANGE = (28),
    SD_SWITCH_ERROR = (29),
    SD_SDIO_DISABLED = (30),
    SD_SDIO_FUNCTION_BUSY = (31),
    SD_SDIO_FUNCTION_FAILED = (32),
    SD_SDIO_UNKNOWN_FUNCTION = (33),

    /** 
     * @brief  Standard error defines   
     */
    SD_INTERNAL_ERROR,
    SD_NOT_CONFIGURED,
    SD_REQUEST_PENDING,
    SD_REQUEST_NOT_APPLICABLE,
    SD_INVALID_PARAMETER,
    SD_UNSUPPORTED_FEATURE,
    SD_UNSUPPORTED_HW,
    SD_ERROR,
    SD_OK = 0
} SD_Error;

/** 
 * @brief  SDIO Transfer state  
 */
typedef enum {
    SD_TRANSFER_OK = 0,
    SD_TRANSFER_BUSY = 1,
    SD_TRANSFER_ERROR
} SDTransferState;

/** 
 * @brief  SD Card States 
 */
typedef enum {
    SD_CARD_READY = ((uint32_t) 0x00000001),
    SD_CARD_IDENTIFICATION = ((uint32_t) 0x00000002),
    SD_CARD_STANDBY = ((uint32_t) 0x00000003),
    SD_CARD_TRANSFER = ((uint32_t) 0x00000004),
    SD_CARD_SENDING = ((uint32_t) 0x00000005),
    SD_CARD_RECEIVING = ((uint32_t) 0x00000006),
    SD_CARD_PROGRAMMING = ((uint32_t) 0x00000007),
    SD_CARD_DISCONNECTED = ((uint32_t) 0x00000008),
    SD_CARD_ERROR = ((uint32_t) 0x000000FF)
} SDCardState;

/** 
 * @brief  Card Specific Data: CSD Register   
 */
typedef struct {
    volatile uint8_t CSDStruct : 2; /*!< CSD structure */
    volatile uint8_t Reserved1 : 6; /*!< Reserved */
    volatile uint8_t TAAC : 8; /*!< Data read access-time 1 */
    volatile uint8_t NSAC : 8; /*!< Data read access-time 2 in CLK cycles */
    volatile uint8_t TRAN_SPEED : 8; /*!< Max data transfer rate */
    volatile uint16_t CardComdClasses : 12; /*!< Card command classes */
    volatile uint8_t RdBlockLen : 4; /*!< Max. read data block length */
    volatile uint8_t PartBlockRead : 1; /*!< Partial blocks for read allowed */
    volatile uint8_t WrBlockMisalign : 1; /*!< Write block misalignment */
    volatile uint8_t RdBlockMisalign : 1; /*!< Read block misalignment */
    volatile uint8_t DSRImpl : 1; /*!< DSR implemented */
    volatile uint8_t Reserved2 : 6; /*!< Reserved */
    volatile uint32_t DeviceSize : 22; /*!< Device Size */
    volatile uint8_t Reserved3 : 1; /*!< Reserved */
    volatile uint8_t EraseBlkEn : 1; /*!< Erase single block enable */
    volatile uint8_t EraseSectorSize : 7; /*!< Erase sector size */
    volatile uint8_t WrProtectGrSize : 7; /*!< Write protect group size */
    volatile uint8_t WrProtectGrEnable : 1; /*!< Write protect group enable */
    volatile uint8_t Reserved4 : 2; /*!< Reserved */
    volatile uint8_t WrSpeedFact : 3; /*!< Write speed factor */
    volatile uint8_t MaxWrBlockLen : 4; /*!< Max. write data block length */
    volatile uint8_t WriteBlockPaPartial : 1; /*!< Partial blocks for write allowed */
    volatile uint8_t Reserved5 : 5; /*!< Reserded */
    volatile uint8_t FileFormatGroup : 1; /*!< File format group */
    volatile uint8_t CopyFlag : 1; /*!< Copy flag (OTP) */
    volatile uint8_t PermWrProtect : 1; /*!< Permanent write protection */
    volatile uint8_t TempWrProtect : 1; /*!< Temporary write protection */
    volatile uint8_t FileFormat : 2; /*!< File Format */
    volatile uint8_t Reserved6 : 2; /*!< Reserded */
    volatile uint8_t CSD_CRC : 7; /*!< CSD CRC */
    volatile uint8_t Reserved7 : 1; /*!< always 1*/
} SD_CSD;

/** 
 * @brief  Card Identification Data: CID Register   
 */
typedef struct {
    volatile uint8_t ManufacturerID; /*!< ManufacturerID */
    volatile uint16_t OEM_AppliID; /*!< OEM/Application ID */
    volatile uint32_t ProdName1; /*!< Product Name part1 */
    volatile uint8_t ProdName2; /*!< Product Name part2*/
    volatile uint8_t ProdRev; /*!< Product Revision */
    volatile uint32_t ProdSN; /*!< Product Serial Number */
    volatile uint8_t Reserved1 : 4; /*!< Reserved1 */
    volatile uint16_t ManufactDate : 12; /*!< Manufacturing Date */
    volatile uint8_t CID_CRC : 7; /*!< CID CRC */
    volatile uint8_t Reserved2 : 1; /*!< always 1 */
} SD_CID;

/** 
 * @brief SD Card Status 
 */
typedef struct {
    volatile uint8_t DAT_BUS_WIDTH;
    volatile uint8_t SECURED_MODE;
    volatile uint16_t SD_CARD_TYPE;
    volatile uint32_t SIZE_OF_PROTECTED_AREA;
    volatile uint8_t SPEED_CLASS;
    volatile uint8_t PERFORMANCE_MOVE;
    volatile uint8_t AU_SIZE;
    volatile uint16_t ERASE_SIZE;
    volatile uint8_t ERASE_TIMEOUT;
    volatile uint8_t ERASE_OFFSET;
} SD_CardStatus;

/** 
 * @brief SD Card information 
 */
typedef struct {
    SD_CSD SD_csd;
    SD_CID SD_cid;
    uint64_t CardCapacity; /*!< Card Capacity */
    uint32_t CardBlockSize; /*!< Card Block Size */
    uint16_t RCA;
    uint8_t CardType;
} SD_CardInfo;

/** 
 * @brief Send block uint
 */
typedef struct {
    uint8_t startBlock;
    uint8_t data[512];
    uint8_t crc[2];
} SD_SendBlock;

/** 
 * @brief SPI Commands Index 
 */
#define SD_CMD_GO_IDLE_STATE ((uint8_t)0)
#define SD_CMD_SEND_OP_COND ((uint8_t)1)
#define SD_CMD_SDIO_SEN_OP_COND ((uint8_t)5)
#define SD_CMD_SWITCH_FUNC ((uint8_t)6)
#define SD_CMD_SEND_IF_COND ((uint8_t)8)
#define SD_CMD_SEND_CSD ((uint8_t)9)
#define SD_CMD_SEND_CID ((uint8_t)10)
#define SD_CMD_STOP_TRANSMISSION ((uint8_t)12)
#define SD_CMD_SEND_STATUS ((uint8_t)13)
#define SD_CMD_SET_BLOCKLEN ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK ((uint8_t)17)
#define SD_CMD_READ_MULTPLE_BLOCK ((uint8_t)18)
#define SD_CMD_WRITE_SINGLE_BLOCK ((uint8_t)24)
#define SD_CMD_WRITE_MULTPLE_BLOCK ((uint8_t)25)
#define SD_CMD_PROGRAM_CSD ((uint8_t)27)
#define SD_CMD_SET_WRITE_PROT ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PROT ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PROT ((uint8_t)30)
#define SD_CMD_ERASE_WR_BLK_START_ADDR ((uint8_t)32) // To set the address of the first write block to be erased. (For SD card only)
#define SD_CMD_ERASE_WR_BLK_END_ADDR ((uint8_t)33)   // To set the address of the last write block of the continuous range to be erased. (For SD card only)
#define SD_CMD_ERASE ((uint8_t)38)
#define SD_CMD_LOCK_UNLOCK ((uint8_t)42)
#define SD_CMD_APP_CMD ((uint8_t)55)
#define SD_CMD_GEN_CMD ((uint8_t)56)
#define SD_CMD_READ_OCR ((uint8_t)58)
#define SD_CMD_CRC_ON_OFF ((uint8_t)59)

/** 
 * @brief Following commands are SD Card Specific commands.
 *        SD_APP_CMD ?CMD55 should be sent before sending these commands. 
 */
#define SD_ACMD_SD_STAUS ((uint8_t)13)               /*!< For SD Card only */
#define SD_ACMD_SD_SEND_NUM_WR_BLOCKS ((uint8_t)22)  /*!< For SD Card only */
#define SD_ACMD_SET_WR_BLK_ERASE_COUNT ((uint8_t)23) /*!< For SD Card only */
#define SD_ACMD_SD_SEND_OP_COND ((uint8_t)41)        /*!< For SD Card only */
#define SD_ACMD_SET_CLR_CARD_DETECT ((uint8_t)42)    /*!< For SD Card only */
#define SD_ACMD_SEND_SCR ((uint8_t)51)               /*!< For SD Card only */

/** 
 * @brief Following commands are SD Card Specific security commands.
 *        SDIO_APP_CMD should be sent before sending these commands. 
 */
#define SD_CMD_SD_APP_GET_MKB ((uint8_t)43)                     /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID ((uint8_t)44)                     /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1 ((uint8_t)45)                 /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2 ((uint8_t)46)                 /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2 ((uint8_t)47)                /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1 ((uint8_t)48)                /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK ((uint8_t)18)  /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK ((uint8_t)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE ((uint8_t)38)                /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA ((uint8_t)49)          /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB ((uint8_t)48)            /*!< For SD Card only */

/* Uncomment the following line to select the SDIO Data transfer mode */
#define SD_DMA_MODE ((uint32_t)0x00000000)
/*#define SD_POLLING_MODE                            ((uint32_t)0x00000002)*/

/**
 * @brief  SD detection on its memory slot
 */
#define SD_PRESENT ((uint8_t)0x01)
#define SD_NOT_PRESENT ((uint8_t)0x00)

/** 
 * @brief Supported SD Memory Cards 
 */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1 ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0 ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD ((uint32_t)0x00000007)

/* Exported functions ------------------------------------------------------- */
void SD_DeInit(void);
SD_Error SD_Init(void);
SDTransferState SD_GetStatus(void);
SDCardState SD_GetState(void);
SD_Error SD_PowerON(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus);
SD_Error SD_ReadBlock(uint8_t *readbuf, uint64_t ReadAddr, uint16_t BlockSize);
SD_Error SD_ReadMultiBlocks(uint8_t *readbuf, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
SD_Error SD_WriteBlock(uint8_t *writebuf, uint64_t WriteAddr, uint16_t BlockSize);
SD_Error SD_WriteMultiBlocks(uint8_t *writebuf, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks);
SDTransferState SD_GetTransferState(void);
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr);
SD_Error SD_SendStatus(uint32_t *pcardstatus);
SD_Error SD_SendSDStatus(uint32_t *psdstatus);
SD_Error SD_ProcessIRQSrc(void);
SD_Error SD_WaitReadOperation(void);
SD_Error SD_WaitWriteOperation(void);
void SD_Test(void);

#endif /* MICROSD_H */
