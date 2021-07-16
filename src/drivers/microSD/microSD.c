#include "../../config/default/device.h"
#include "../../config/default/peripheral/gpio/plib_gpio.h"
#include "../../config/default/peripheral/coretimer/plib_coretimer.h"
#include "../../utilities/crc.h"
#include "../spi/spi.h"
#include "../../library/Fatfs/ff.h"
#include "microSD.h"
#include <stdio.h>
#include <string.h>

/* Private macro -------------------------------------------------------------*/
//#define SD_DEBUG
//#define SD_TIME_DEBUG
/** 
 * @brief  SDIO Static flags, TimeOut, FIFO Address  
 */
#define SDIO_STATIC_FLAGS ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT ((uint32_t)0x00010000)

/** 
 * @brief  Mask for errors Card Status R1 (OCR Register) 
 */
#define SD_OCR_ADDR_OUT_OF_RANGE ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN ((uint32_t)0x00020000)
#define SD_OCR_CSD_OVERWRIETE ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS ((uint32_t)0xFDFFE008)

/** 
 * @brief  Masks for R6 Response 
 */
#define SD_R6_GENERAL_UNKNOWN_ERROR ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY ((uint32_t)0x40000000)
#define SD_STD_CAPACITY ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN ((uint32_t)0x000001AA)

#define SD_MAX_VOLT_TRIAL ((uint32_t)0x0000FFFF)
#define SD_ALLZERO ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT ((uint32_t)0x00010000)
#define SD_CARD_LOCKED ((uint32_t)0x02000000)

#define SD_DATATIMEOUT ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS ((uint32_t)0x000000FF)
#define SD_8TO15BITS ((uint32_t)0x0000FF00)
#define SD_16TO23BITS ((uint32_t)0x00FF0000)
#define SD_24TO31BITS ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES ((uint32_t)0x00000020)

/** 
 * @brief  Command Class Supported 
 */
#define SD_CCCC_LOCK_UNLOCK ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT ((uint32_t)0x00000040)
#define SD_CCCC_ERASE ((uint32_t)0x00000020)

/** 
 * @brief  Following commands are SD Card Specific commands.
 *         SDIO_APP_CMD should be sent before sending these commands. 
 */
#define SDIO_SEND_IF_COND ((uint32_t)0x00000008)

/* Private variables ---------------------------------------------------------*/

static uint32_t CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static uint32_t CSD_Tab[4], CID_Tab[4];
static uint8_t SDSTATUS_Tab[16];
volatile uint32_t StopCondition = 0;
volatile SD_Error TransferError = SD_OK;
volatile uint32_t TransferEnd = 0;
SD_CardInfo SDCardInfo;

//CMD
static uint8_t cmd0[6] = {SD_CMD_GO_IDLE_STATE, 0};
static uint8_t cmd8[6] = {SD_CMD_SEND_IF_COND, 0};
static uint8_t cmd9[6] = {SD_CMD_SEND_CSD, 0};
static uint8_t cmd10[6] = {SD_CMD_SEND_CID, 0};
static uint8_t cmd12[6] = {SD_CMD_STOP_TRANSMISSION, 0};
static uint8_t cmd13[6] = {SD_CMD_SEND_STATUS, 0};
static uint8_t cmd17[6] = {SD_CMD_READ_SINGLE_BLOCK, 0};
static uint8_t cmd18[6] = {SD_CMD_READ_MULTPLE_BLOCK, 0};
static uint8_t cmd24[6] = {SD_CMD_WRITE_SINGLE_BLOCK, 0};
static uint8_t cmd25[6] = {SD_CMD_WRITE_MULTPLE_BLOCK, 0};
static uint8_t cmd32[6] = {SD_CMD_ERASE_WR_BLK_START_ADDR, 0};
static uint8_t cmd33[6] = {SD_CMD_ERASE_WR_BLK_END_ADDR, 0};
static uint8_t cmd38[6] = {SD_CMD_ERASE, 0};
static uint8_t cmd55[6] = {SD_CMD_APP_CMD, 0};
static uint8_t cmd58[6] = {SD_CMD_READ_OCR, 0};

static uint8_t acmd13[6] = {SD_ACMD_SD_STAUS, 0};
static uint8_t acmd22[6] = {SD_ACMD_SD_SEND_NUM_WR_BLOCKS, 0};
static uint8_t acmd41[6] = {SD_ACMD_SD_SEND_OP_COND, 0};

//RECV
static uint8_t res0[1];
static uint8_t res1[1];
static uint8_t res2[2];
static uint8_t res3[5];

/* Private function prototypes -----------------------------------------------*/
static void SD_SendCommand(uint8_t *cmd, int len);
static void SD_SendData(uint8_t *data, int len);
static void SD_RecvData(uint8_t *data, int len);
static void SD_RecvResponse(uint8_t *rxData, int len);
static void SD_DummyRead(int len);
static void SD_StopTransfer(void);
static uint8_t SD_RecvDataHead();

//static SD_Error CmdError(void);
//static SD_Error CmdResp1Error(uint8_t cmd);
static SD_Error CmdResp3Error(void);
static SD_Error CmdResp7Error(void);

/*
static SD_Error CmdResp2Error(void);
static SD_Error CmdResp6Error(uint8_t cmd, uint16_t *prca);
static SD_Error SDEnWideBus(FunctionalState NewState);
static SD_Error IsCardProgramming(uint8_t *pstatus);
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr);

static void GPIO_Configuration(void);
static uint32_t SD_DMAEndOfTransferStatus(void);
static void SD_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
static void SD_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);*/

uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  SD card init
 * @param  None
 * @retval SD error code or SD_OK
 */
SD_Error SD_Init(void)
{
    SD_Error errorstatus = SD_OK;

    errorstatus = SD_PowerON();
    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    errorstatus = SD_InitializeCards();
    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    errorstatus = SD_GetCardInfo(&SDCardInfo);
    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    //Test
    //printf("SD_GetStatus: %x\n", SD_GetStatus());
    //uint32_t status;
    //SD_SendSDStatus(&status);
    /*
    uint8_t read[4][512];
    //SD_ReadBlock(read, 0, 512);
    uint8_t write[4][512];
    for (int i = 0; i < 512; i++)
      write[0][i] = i % 256;
    for (int i = 0; i < 512; i++)
      write[1][i] = (256 - i) % 256;
    for (int i = 0; i < 512; i++)
      write[2][i] = i % 256;
    for (int i = 0; i < 512; i++)
      write[3][i] = (256 - i) % 256;
    //SD_WriteBlock(write[0], 1 * 512, 512);
    //SD_WriteMultiBlocks((uint8_t *)write, 15 * 512, 512, 4);
    SD_Erase(17 * 512, 17 * 512);
    SD_ReadMultiBlocks((uint8_t *)read, 15 * 512, 512, 4);*/
#ifdef SD_TIME_DEBUG
    uint8_t read16[16][512];
    //SD_ReadMultiBlocks((uint8_t *) read16, 0 * 512, 512, 2);
    for (int i = 0; i < 16; i++)
        SD_ReadMultiBlocks((uint8_t *) read16, 0 * 512, 512, i + 1);
#endif
    return (errorstatus);
}

/**
 * @brief  Gets the cuurent sd card data transfer status.
 * @param  None
 * @retval Data Transfer state.
 */
SDTransferState SD_GetStatus(void)
{
#ifdef SD_DEBUG
    printf("----SD_GetStatus----\n");
#endif
    SDCardState cardstate = SD_CARD_TRANSFER;

    //cardstate = SD_GetState();

    if (cardstate == SD_CARD_TRANSFER)
    {
        return (SD_TRANSFER_OK);
    }
    else if (cardstate == SD_CARD_ERROR)
    {
        return (SD_TRANSFER_ERROR);
    }
    else
    {
        return (SD_TRANSFER_BUSY);
    }
}

/**
 * @brief  Returns the current card's state.
 * @param  None
 * @retval SDCardState: SD Card Error or SD Card Current State.
 */
SDCardState SD_GetState(void)
{
#ifdef SD_DEBUG
    printf("----SD_GetState----\n");
#endif
    uint32_t resp1 = 0;
    if (SD_SendStatus(&resp1) != SD_OK)
    {
        return SD_CARD_ERROR;
    }
    else
    {
        return (SDCardState) ((resp1 >> 9) & 0x0F);
    }
}

/**
 * @brief  SD power on
 * @param  None
 * @retval SD error code or SD_OK
 */
SD_Error SD_PowerON(void)
{
#ifdef SD_DEBUG
    printf("----SD_PowerOn----\n");
#endif
    SD_Error errorstatus = SD_OK;
    //uint32_t SDType = SD_STD_CAPACITY;
    /********************************************************************************************************/
    /* CMD0: GO_IDLE_STATE */
    SD_SendCommand(cmd0, 6);
    SD_RecvResponse(res0, 1);
    SD_DummyRead(10);
    if (res0[0] != 0x01)
        return SD_ERROR;
    /********************************************************************************************************/
    /* CMD8: SEND_IF_COND */
    cmd8[3] |= 0x01;
    cmd8[4] = 0xAA;
    SD_SendCommand(cmd8, 6);
    errorstatus = CmdResp7Error();
    if (errorstatus == SD_OK)
    {
        CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
        //SDType = SD_HIGH_CAPACITY;

        // CMD58: SD_CMD_READ_OCR
        SD_SendCommand(cmd58, 6);
        errorstatus = CmdResp3Error();
        if (errorstatus != SD_OK)
        {
            return (errorstatus);
        }
        // ACMD41
        do
        {
            SD_SendCommand(cmd55, 6);
            SD_RecvResponse(res1, 1);

            acmd41[1] = 0x40;
            SD_SendCommand(acmd41, 6);
            SD_RecvResponse(res1, 1);
        } while (res1[0] == 0x01);
        // CMD58: SD_CMD_READ_OCR
        SD_SendCommand(cmd58, 6);
        SD_RecvResponse(res3, 5);

        if (res3[1] &= 0x40)
        {
            CardType = SDIO_HIGH_CAPACITY_SD_CARD;
        }
    }
    else
    {
        errorstatus = SD_ERROR;
    }
    return (errorstatus);
}

/**
 * @brief  Initialize card
 * @param  None
 * @retval SD error code or SD_OK
 */
SD_Error SD_InitializeCards(void)
{
#ifdef SD_DEBUG
    printf("----SD_InitializeCards----\n");
#endif
    SD_Error errorstatus = SD_OK;
    //uint16_t rca = 0x01;

    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
    {
        /* Send CMD10 SEND_CID 
         */
        SD_SendCommand(cmd10, 6);
        SD_RecvResponse(res1, 1);

        /* ????CID?????? */
        SD_DummyRead(1); //0xFE
        SD_RecvResponse((uint8_t *) & CID_Tab[0], 4);
        SD_RecvResponse((uint8_t *) & CID_Tab[1], 4);
        SD_RecvResponse((uint8_t *) & CID_Tab[2], 4);
        SD_RecvResponse((uint8_t *) & CID_Tab[3], 4);
        uint16_t crc;
        SD_RecvResponse((uint8_t *) & crc, 2);
        crc = (crc >> 8) | (crc << 8);
        if (crc != crc16((unsigned char *) CID_Tab, 16))
        {
            /*printf("crc: %x\n", crc);
            printf("crc16: %x\n", crc16(CID_Tab, 16));*/
            errorstatus = SD_DATA_CRC_FAIL;
            return (errorstatus);
        }
        /********************************************************************************************************/
        //RCA register is not accessible in SPI mode
        /********************************************************************************************************/
        if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
        {
            // Send CMD9 SEND_CSD 
            SD_SendCommand(cmd9, 6);
            SD_RecvResponse(res1, 1);

            SD_DummyRead(1); //0xFE
            SD_RecvResponse((uint8_t *) & CSD_Tab[0], 4);
            SD_RecvResponse((uint8_t *) & CSD_Tab[1], 4);
            SD_RecvResponse((uint8_t *) & CSD_Tab[2], 4);
            SD_RecvResponse((uint8_t *) & CSD_Tab[3], 4);
            uint16_t crc;
            SD_RecvResponse((uint8_t *) & crc, 2);
            crc = (crc >> 8) | (crc << 8);
            if (crc != crc16((unsigned char *) CSD_Tab, 16))
            {
                errorstatus = SD_DATA_CRC_FAIL;
                return (errorstatus);
            }
        }
        /********************************************************************************************************/
        errorstatus = SD_OK;
        return (errorstatus);
    }
    errorstatus = SD_ERROR;
    return (errorstatus);
}

/**
 * @brief  Get card infomation
 * @param  cardinfo
 * @retval SD error code or SD_OK
 */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
    SD_Error errorstatus = SD_OK;
    uint8_t tmp = 0;

    cardinfo->CardType = (uint8_t) CardType;
    /*
    printf("CSD_Tab: \n");
    for (int i = 0; i < 4; i++)
      printf("%x\n", CSD_Tab[i]);
     */
    //Byte 0
    tmp = (uint8_t) (CSD_Tab[0] & 0xFF);
    cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;
    cardinfo->SD_csd.Reserved1 = 0; //Reserved
    //Byte 1
    tmp = (uint8_t) ((CSD_Tab[0] & 0xFF00) >> 8);
    cardinfo->SD_csd.TAAC = tmp;
    //Byte 2
    tmp = (uint8_t) ((CSD_Tab[0] & 0xFF0000) >> 16);
    cardinfo->SD_csd.NSAC = tmp;
    //Byte 3
    tmp = (uint8_t) ((CSD_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_csd.TRAN_SPEED = tmp;
    //Byte 4
    tmp = (uint8_t) (CSD_Tab[1] & 0xFF);
    cardinfo->SD_csd.CardComdClasses = tmp << 4;
    //Byte 5
    tmp = (uint8_t) ((CSD_Tab[1] & 0xFF00) >> 8);
    cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
    cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;
    //Byte 6
    tmp = (uint8_t) ((CSD_Tab[0] & 0xFF0000) >> 16);
    cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.Reserved2 = 0; //Reserved
    //Byte 7
    tmp = (uint8_t) ((CSD_Tab[1] & 0xFF000000) >> 24);
    cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 20;
    //Byte 8
    tmp = (uint8_t) (CSD_Tab[2] & 0xFF);
    cardinfo->SD_csd.DeviceSize |= tmp << 8;
    //Byte 9
    tmp = (uint8_t) ((CSD_Tab[2] & 0xFF00) >> 8);
    cardinfo->SD_csd.DeviceSize |= tmp;
    //Byte 10
    tmp = (uint8_t) ((CSD_Tab[2] & 0xFF0000) >> 16);
    cardinfo->SD_csd.Reserved3 = 0; //Reserved
    cardinfo->SD_csd.EraseBlkEn = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.EraseSectorSize = (tmp & 0x3F) << 1;
    //Byte 11
    tmp = (uint8_t) ((CSD_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_csd.EraseSectorSize |= (tmp & 0x80) >> 7;
    cardinfo->SD_csd.WrProtectGrSize = tmp & 0x3F;
    //Byte 12
    tmp = (uint8_t) (CSD_Tab[3] & 0xFF);
    cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.Reserved4 = 0; //Reserved
    cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;
    //Byte 13
    tmp = (uint8_t) ((CSD_Tab[3] & 0xFF00) >> 8);
    cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
    cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.Reserved5 = 0; //Reserved
    //Byte 14
    tmp = (uint8_t) ((CSD_Tab[3] & 0xFF0000) >> 16);
    cardinfo->SD_csd.FileFormatGroup = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
    cardinfo->SD_csd.Reserved6 = 0; //Reserved
    //Byte 15
    tmp = (uint8_t) ((CSD_Tab[3] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
    cardinfo->SD_csd.Reserved7 = 0; //Reserved
    //print CSD info
    /*
    printf("CSD structure: %x\n", cardinfo->SD_csd.CSDStruct);
    printf("Data read access-time 1: %x\n", cardinfo->SD_csd.TAAC);
    printf("Data read access-time 2 in CLK cycles: %x\n", cardinfo->SD_csd.NSAC);
    printf("Max data transfer rate: %x\n", cardinfo->SD_csd.TRAN_SPEED);
    printf("Card command classes: %x\n", cardinfo->SD_csd.CardComdClasses);
    printf("Max. read data block length: %x\n", cardinfo->SD_csd.RdBlockLen);
    printf("Partial blocks for read allowed: %x\n", cardinfo->SD_csd.PartBlockRead);
    printf("Write block misalignment: %x\n", cardinfo->SD_csd.WrBlockMisalign);
    printf("Read block misalignment: %x\n", cardinfo->SD_csd.RdBlockMisalign);
    printf("DSR implemented: %x\n", cardinfo->SD_csd.DSRImpl);
    printf("Device Size: %x\n", cardinfo->SD_csd.DeviceSize);
    printf("Erase single block enable: %x\n", cardinfo->SD_csd.EraseBlkEn);
    printf("Erase sector size: %x\n", cardinfo->SD_csd.EraseSectorSize);
    printf("Write protect group size: %x\n", cardinfo->SD_csd.WrProtectGrSize);
    printf("Write protect group enable: %x\n", cardinfo->SD_csd.WrProtectGrEnable);
    printf("Write speed factor: %x\n", cardinfo->SD_csd.WrSpeedFact);
    printf("Max. write data block length: %x\n", cardinfo->SD_csd.MaxWrBlockLen);
    printf("Partial blocks for write allowed: %x\n", cardinfo->SD_csd.WriteBlockPaPartial);
    printf("File format group: %x\n", cardinfo->SD_csd.FileFormatGroup);
    printf("Copy flag (OTP): %x\n", cardinfo->SD_csd.CopyFlag);
    printf("Permanent write protection: %x\n", cardinfo->SD_csd.PermWrProtect);
    printf("Temporary write protection: %x\n", cardinfo->SD_csd.TempWrProtect);
    printf("File Format: %x\n", cardinfo->SD_csd.FileFormat);
    printf("CSD CRC: %x\n", cardinfo->SD_csd.CSD_CRC);*/

    // Byte 0
    tmp = (uint8_t) (CID_Tab[0] & 0xFF);
    cardinfo->SD_cid.ManufacturerID = tmp;
    // Byte 1
    tmp = (uint8_t) ((CID_Tab[0] & 0xFF00) >> 8);
    cardinfo->SD_cid.OEM_AppliID = tmp << 8;
    // Byte 2
    tmp = (uint8_t) ((CID_Tab[0] & 0xFF0000) >> 16);
    cardinfo->SD_cid.OEM_AppliID |= tmp;
    // Byte 3
    tmp = (uint8_t) ((CID_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdName1 = tmp << 24;
    // Byte 4
    tmp = (uint8_t) (CID_Tab[1] & 0xFF);
    cardinfo->SD_cid.ProdName1 |= tmp << 16;
    // Byte 5
    tmp = (uint8_t) ((CID_Tab[1] & 0xFF00) >> 8);
    cardinfo->SD_cid.ProdName1 |= tmp << 8;
    // Byte 6
    tmp = (uint8_t) ((CID_Tab[1] & 0xFF0000) >> 16);
    cardinfo->SD_cid.ProdName1 |= tmp;
    // Byte 7
    tmp = (uint8_t) ((CID_Tab[1] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdName2 = tmp;
    // Byte 8
    tmp = (uint8_t) (CID_Tab[2] & 0xFF);
    cardinfo->SD_cid.ProdRev = tmp;
    // Byte 9
    tmp = (uint8_t) ((CID_Tab[2] & 0xFF00) >> 8);
    cardinfo->SD_cid.ProdSN = tmp << 24;
    // Byte 10
    tmp = (uint8_t) ((CID_Tab[2] & 0xFF0000) >> 16);
    cardinfo->SD_cid.ProdSN |= tmp << 16;
    // Byte 11
    tmp = (uint8_t) ((CID_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdSN |= tmp << 8;
    // Byte 12
    tmp = (uint8_t) (CID_Tab[3] & 0xFF);
    cardinfo->SD_cid.ProdSN |= tmp;
    // Byte 13
    tmp = (uint8_t) ((CID_Tab[3] & 0xFF00) >> 8);
    cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
    cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;
    // Byte 14
    tmp = (uint8_t) ((CID_Tab[3] & 0xFF0000) >> 16);
    cardinfo->SD_cid.ManufactDate |= tmp;
    // Byte 15
    tmp = (uint8_t) ((CID_Tab[3] & 0xFF000000) >> 24);
    cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
    cardinfo->SD_cid.Reserved2 = 1;
    //print CSD info
    /*
    printf("Manufacturer ID: %d\n", cardinfo->SD_cid.ManufacturerID);
    printf("OEM/Application ID: %c%c\n", (cardinfo->SD_cid.OEM_AppliID & 0xFF00) >> 8, cardinfo->SD_cid.OEM_AppliID & 0xFF);
    printf("Product name: %c%c%c%c%c\n", (cardinfo->SD_cid.ProdName1 & 0xFF000000) >> 24, (cardinfo->SD_cid.ProdName1 & 0xFF0000) >> 16,
           (cardinfo->SD_cid.ProdName1 & 0xFF00) >> 8, cardinfo->SD_cid.ProdName1 & 0xFF, cardinfo->SD_cid.ProdName2);
    printf("Product revision: %d.%d\n", (cardinfo->SD_cid.ProdRev & 0xF0) >> 4, cardinfo->SD_cid.ProdRev & 0x0F);
    printf("Product serial number: %02x%02x%02x%02x\n", (cardinfo->SD_cid.ProdSN & 0xFF000000) >> 24, (cardinfo->SD_cid.ProdSN & 0xFF0000) >> 16,
           (cardinfo->SD_cid.ProdSN & 0xFF00) >> 8, cardinfo->SD_cid.ProdSN & 0xFF);
    printf("Manufacturing date: %d.%02d\n", ((cardinfo->SD_cid.ManufactDate & 0xFF0) >> 4) + 2000, cardinfo->SD_cid.ManufactDate & 0xF);*/
    return (errorstatus);
}

/**
 * @brief  Enables wide bus opeartion for the requeseted card if supported by 
 *         card.
 * @param  cardstatus: ??????? 
 * @retval SD error code or SD_OK
 */
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus)
{
    SD_Error errorstatus = SD_OK;
    uint8_t tmp = 0;

    errorstatus = SD_SendSDStatus((uint32_t *) SDSTATUS_Tab);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    /*!< Byte 0 */
    tmp = (uint8_t) ((SDSTATUS_Tab[0] & 0xC0) >> 6);
    cardstatus->DAT_BUS_WIDTH = tmp;

    /*!< Byte 0 */
    tmp = (uint8_t) ((SDSTATUS_Tab[0] & 0x20) >> 5);
    cardstatus->SECURED_MODE = tmp;

    /*!< Byte 2 */
    tmp = (uint8_t) ((SDSTATUS_Tab[2] & 0xFF));
    cardstatus->SD_CARD_TYPE = tmp << 8;

    /*!< Byte 3 */
    tmp = (uint8_t) ((SDSTATUS_Tab[3] & 0xFF));
    cardstatus->SD_CARD_TYPE |= tmp;

    /*!< Byte 4 */
    tmp = (uint8_t) (SDSTATUS_Tab[4] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA = tmp << 24;

    /*!< Byte 5 */
    tmp = (uint8_t) (SDSTATUS_Tab[5] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 16;

    /*!< Byte 6 */
    tmp = (uint8_t) (SDSTATUS_Tab[6] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 8;

    /*!< Byte 7 */
    tmp = (uint8_t) (SDSTATUS_Tab[7] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA |= tmp;

    /*!< Byte 8 */
    tmp = (uint8_t) ((SDSTATUS_Tab[8] & 0xFF));
    cardstatus->SPEED_CLASS = tmp;

    /*!< Byte 9 */
    tmp = (uint8_t) ((SDSTATUS_Tab[9] & 0xFF));
    cardstatus->PERFORMANCE_MOVE = tmp;

    /*!< Byte 10 */
    tmp = (uint8_t) ((SDSTATUS_Tab[10] & 0xF0) >> 4);
    cardstatus->AU_SIZE = tmp;

    /*!< Byte 11 */
    tmp = (uint8_t) (SDSTATUS_Tab[11] & 0xFF);
    cardstatus->ERASE_SIZE = tmp << 8;

    /*!< Byte 12 */
    tmp = (uint8_t) (SDSTATUS_Tab[12] & 0xFF);
    cardstatus->ERASE_SIZE |= tmp;

    /*!< Byte 13 */
    tmp = (uint8_t) ((SDSTATUS_Tab[13] & 0xFC) >> 2);
    cardstatus->ERASE_TIMEOUT = tmp;

    /*!< Byte 13 */
    tmp = (uint8_t) ((SDSTATUS_Tab[13] & 0x3));
    cardstatus->ERASE_OFFSET = tmp;

    return (errorstatus);
}

/**
 * @brief  Allows to read one block from a specified address in a card. The Data
 *         transfer can be managed by DMA mode or Polling mode. 
 * @note   This operation should be followed by two functions to check if the 
 *         DMA Controller and SD Card status.
 *          - SD_ReadWaitOperation(): this function insure that the DMA
 *            controller has finished all data transfer.
 *          - SD_GetStatus(): to check that the SD Card has finished the 
 *            data transfer and it is ready for data.            
 * @param  readbuf: pointer to the buffer that will contain the received data
 * @param  ReadAddr: Address from where data are to be read.  
 * @param  BlockSize: the SD card Data block size. The Block size should be 512.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_ReadBlock(uint8_t *readbuf, uint64_t ReadAddr, uint16_t BlockSize)
{
#ifdef SD_DEBUG
    printf("----SD_ReadBlock----\n");
#endif
    SD_Error errorstatus = SD_OK;

    TransferError = SD_OK;
    TransferEnd = 0;
    StopCondition = 0;

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize = 512;
        ReadAddr /= 512;
    }
    /*********************************************************************************/
    // Send CMD17
    cmd17[1] = (ReadAddr & 0xFF000000) >> 24;
    cmd17[2] = (ReadAddr & 0xFF0000) >> 16;
    cmd17[3] = (ReadAddr & 0xFF00) >> 8;
    cmd17[4] = ReadAddr & 0xFF;
    SD_SendCommand(cmd17, 6);
    SD_RecvResponse(res1, 1);
    if (res1[0] != 0x00)
    {
        errorstatus = SD_ERROR;
        return (errorstatus);
    }

    uint8_t startToken;
    do
    {
        SD_RecvResponse(&startToken, 1);
    } while (startToken == 0xFF);
    if (startToken == 0xFE)
    {
        SD_RecvResponse(readbuf, 512);
        uint16_t crc;
        SD_RecvResponse((uint8_t *) & crc, 2);
        crc = (crc >> 8) | (crc << 8);
        if (crc != crc16(readbuf, 512))
            errorstatus = SD_DATA_CRC_FAIL;
    }
    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    /*
    SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);
    SDIO_DMACmd(ENABLE);
    SD_DMA_RxConfig((uint32_t *)readbuff, BlockSize);*/

    return (errorstatus);
}

/**
 * @brief  Allows to read blocks from a specified address  in a card.  The Data
 *         transfer can be managed by DMA mode or Polling mode.
 * @note   This operation should be followed by two functions to check if the 
 *         DMA Controller and SD Card status.
 *          - SD_ReadWaitOperation(): this function insure that the DMA
 *            controller has finished all data transfer. 
 *          - SD_GetStatus(): to check that the SD Card has finished the 
 *            data transfer and it is ready for data.   
 * @param  readbuf: pointer to the buffer that will contain the received data.
 * @param  ReadAddr: Address from where data are to be read.
 * @param  BlockSize: the SD card Data block size. The Block size should be 512.
 * @param  NumberOfBlocks: number of blocks to be read.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_ReadMultiBlocks(uint8_t *readbuf, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
#ifdef SD_TIME_DEBUG
    printf("NumberOfBlocks: %d\n", NumberOfBlocks);
    printf("Recv response begin: %d\n", CORETIMER_CounterGet());
#endif
    SD_Error errorstatus = SD_OK;
    TransferError = SD_OK;
    TransferEnd = 0;
    StopCondition = 1;

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize = 512;
        ReadAddr /= 512;
    }

    /*********************************************************************************/
    // Send CMD18
    cmd18[1] = (ReadAddr & 0xFF000000) >> 24;
    cmd18[2] = (ReadAddr & 0xFF0000) >> 16;
    cmd18[3] = (ReadAddr & 0xFF00) >> 8;
    cmd18[4] = ReadAddr & 0xFF;
    SD_SendCommand(cmd18, 6);
    SD_RecvResponse(res1, 1);

    if (res1[0] != 0x00)
    {
        errorstatus = SD_ERROR;
        return (errorstatus);
    }

    uint8_t startToken;
    for (int i = 0; i < NumberOfBlocks; i++)
    {
        /*
        do
        {
          SD_RecvResponse(&startToken, 1);
        } while (startToken == 0xFF);*/
        startToken = SD_RecvDataHead();
        if (startToken == 0xFE)
        {
            /*
            SD_RecvResponse(readbuf + 512 * i, 512);
            uint16_t crc;
            SD_RecvResponse(&crc, 2);
            crc = (crc >> 8) | (crc << 8);
            if (crc != crc16(readbuf + 512 * i, 512))
              errorstatus = SD_DATA_CRC_FAIL;
             */

            SD_RecvData(readbuf + 512 * i, 512);
            uint16_t crc;
            SD_RecvData((uint8_t *) & crc, 2);
            crc = (crc >> 8) | (crc << 8);
            //printf("crc: %x\n", crc);
            //printf("crc_cal: %x\n", crc16(readbuf + 512 * i, 512));
            /*
            if (crc != crc16(readbuf + 512 * i, 512))
            errorstatus = SD_DATA_CRC_FAIL;*/
#ifdef SD_DEBUG
            printf("Read block %d is ok!\n", i);
#endif
        }
        else
            break;
    }

    // Send CMD12
    SD_SendCommand(cmd12, 6);
    SD_RecvResponse(res1, 1);
    uint8_t busy;
    do
    {
        SD_RecvResponse(&busy, 1);
    } while (busy == 0x00);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

#ifdef SD_TIME_DEBUG
    printf("Recv response end: %d\n", CORETIMER_CounterGet());
#endif
    return (errorstatus);
}

/**
 * @brief  Allows to write one block starting from a specified address in a card.
 *         The Data transfer can be managed by DMA mode or Polling mode.
 * @note   This operation should be followed by two functions to check if the 
 *         DMA Controller and SD Card status.
 *          - SD_ReadWaitOperation(): this function insure that the DMA
 *            controller has finished all data transfer.
 *          - SD_GetStatus(): to check that the SD Card has finished the 
 *            data transfer and it is ready for data.      
 * @param  writebuf: pointer to the buffer that contain the data to be transferred.
 * @param  WriteAddr: Address from where data are to be read.   
 * @param  BlockSize: the SD card Data block size. The Block size should be 512.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_WriteBlock(uint8_t *writebuf, uint64_t WriteAddr, uint16_t BlockSize)
{
    SD_Error errorstatus = SD_OK;

    TransferError = SD_OK;
    TransferEnd = 0;
    StopCondition = 0;

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize = 512;
        WriteAddr /= 512;
    }
    /*********************************************************************************/
    // Send CMD24
    cmd24[1] = (WriteAddr & 0xFF000000) >> 24;
    cmd24[2] = (WriteAddr & 0xFF0000) >> 16;
    cmd24[3] = (WriteAddr & 0xFF00) >> 8;
    cmd24[4] = WriteAddr & 0xFF;
    SD_SendCommand(cmd24, 6);
    SD_RecvResponse(res1, 1);
    if (res1[0] != 0x00)
    {
        errorstatus = SD_ERROR;
        return (errorstatus);
    }

    SD_SendBlock block;
    block.startBlock = 0xFE;
    memcpy(block.data, writebuf, 512);
    uint16_t crc = crc16(block.data, 512);
    block.crc[0] = (crc & 0xFF00) >> 8;
    block.crc[1] = crc & 0xFF;

    SD_SendData((uint8_t *) & block, 515);
    uint8_t busy;
    do
    {
        SD_RecvResponse(&busy, 1);
    } while (busy == 0x00);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    return (errorstatus);
}

/**
 * @brief  Allows to write blocks starting from a specified address in a card.
 *         The Data transfer can be managed by DMA mode only. 
 * @note   This operation should be followed by two functions to check if the 
 *         DMA Controller and SD Card status.
 *          - SD_ReadWaitOperation(): this function insure that the DMA
 *            controller has finished all data transfer.
 *          - SD_GetStatus(): to check that the SD Card has finished the 
 *            data transfer and it is ready for data.     
 * @param  WriteAddr: Address from where data are to be read.
 * @param  writebuf: pointer to the buffer that contain the data to be transferred.
 * @param  BlockSize: the SD card Data block size. The Block size should be 512.
 * @param  NumberOfBlocks: number of blocks to be written.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_WriteMultiBlocks(uint8_t *writebuf, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
    SD_Error errorstatus = SD_OK;

    TransferError = SD_OK;
    TransferEnd = 0;
    StopCondition = 0;

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize = 512;
        WriteAddr /= 512;
    }
    /*********************************************************************************/
    // Send CMD25
    cmd25[1] = (WriteAddr & 0xFF000000) >> 24;
    cmd25[2] = (WriteAddr & 0xFF0000) >> 16;
    cmd25[3] = (WriteAddr & 0xFF00) >> 8;
    cmd25[4] = WriteAddr & 0xFF;
    SD_SendCommand(cmd25, 6);
    SD_RecvResponse(res1, 1);
    if (res1[0] != 0x00)
    {
        errorstatus = SD_ERROR;
        return (errorstatus);
    }

    uint8_t busy;
    for (int i = 0; i < NumberOfBlocks; i++)
    {
        SD_SendBlock block;
        block.startBlock = 0xFC;
        memcpy(block.data, writebuf + i * 512, 512);
        uint16_t crc = crc16(block.data, 512);
        block.crc[0] = (crc & 0xFF00) >> 8;
        block.crc[1] = crc & 0xFF;

        SD_SendData((uint8_t *) & block, 515);
        do
        {
            SD_RecvResponse(&busy, 1);
        } while (busy == 0x00);
    }
    //stop transmission
    SD_StopTransfer();
    do
    {
        SD_RecvResponse(&busy, 1);
    } while (busy == 0x00);
    //get the number of well written write blocks
    SD_SendCommand(cmd55, 6);
    SD_RecvResponse(res1, 1);

    SD_SendCommand(acmd22, 6);
    SD_RecvResponse(res1, 1);
    uint8_t startToken;
    do
    {
        SD_RecvResponse(&startToken, 1);
    } while (startToken == 0xFF);
    uint8_t writeLen[4];
    uint32_t len = 0;
    SD_RecvResponse(writeLen, 4);
    len += writeLen[0];
    len <<= 24;
    len += writeLen[1];
    len <<= 16;
    len += writeLen[2];
    len <<= 8;
    len += writeLen[3];
#ifdef SD_DEBUG
    printf("len: %d\n", len);
#endif

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    return (errorstatus);
}

/**
 * @brief  Gets the current data transfer state.
 * @param  None
 * @retval SDTransferState: Data Transfer state.
 *   This value can be: 
 *        - SD_TRANSFER_OK: No data transfer is acting
 *        - SD_TRANSFER_BUSY: Data transfer is acting
 */
SDTransferState SD_GetTransferState(void)
{
    return SD_TRANSFER_OK;
}

/**
 * @brief  Allows to erase memory area specified for the given card.
 * @param  startaddr: the start address.
 * @param  endaddr: the end address.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr)
{
#ifdef SD_DEBUG
    printf("----Erase----\n");
#endif
    SD_Error errorstatus = SD_OK;

    startaddr /= 512;
    endaddr /= 512;

    // Send CMD32
    cmd32[1] = (startaddr & 0xFF000000) >> 24;
    cmd32[2] = (startaddr & 0xFF0000) >> 16;
    cmd32[3] = (startaddr & 0xFF00) >> 8;
    cmd32[4] = startaddr & 0xFF;
    SD_SendCommand(cmd32, 6);
    SD_RecvResponse(res1, 1);

    if (res1[0] != 0x00)
        errorstatus = SD_ERROR;

    // Send CMD33
    cmd33[1] = (endaddr & 0xFF000000) >> 24;
    cmd33[2] = (endaddr & 0xFF0000) >> 16;
    cmd33[3] = (endaddr & 0xFF00) >> 8;
    cmd33[4] = endaddr & 0xFF;
    SD_SendCommand(cmd33, 6);
    SD_RecvResponse(res1, 1);

    if (res1[0] != 0x00)
        errorstatus = SD_ERROR;

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    // Send CMD38
    SD_SendCommand(cmd38, 6);
    SD_RecvResponse(res1, 1);

    if (res1[0] != 0x00)
        errorstatus = SD_ERROR;

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    uint8_t busy;
    do
    {
        SD_RecvResponse(&busy, 1);
    } while (busy == 0x00);

    return (errorstatus);
}

/**
 * @brief  Returns the current card's status.
 * @param  pcardstatus: pointer to the buffer that will contain the SD card 
 *         status (Card Status register).
 * @retval SD error code or SD_OK
 */
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
#ifdef SD_DEBUG
    printf("----SD_SendStatus----\n");
#endif
    SD_Error errorstatus = SD_OK;
    /* CMD13: SEND_STATUS */
    SD_SendCommand(cmd13, 6);
    SD_RecvResponse(res2, 2);
    SD_DummyRead(5);
    SD_DummyRead(4);
    SD_DummyRead(20);
    pcardstatus = (uint32_t *) res2;
    return (errorstatus);
}

/**
 * @brief  Returns the current SD card's status.
 * @param  psdstatus: pointer to the buffer that will contain the SD card status 
 *         (SD Status register).
 * @retval SD error code or SD_OK
 */
SD_Error SD_SendSDStatus(uint32_t *psdstatus)
{
#ifdef SD_DEBUG
    printf("----SD_SendSDStatus----\n");
#endif
    SD_Error errorstatus = SD_OK;
    //uint32_t count = 0;

    /* ACMD13: SEND_STATUS */
    SD_SendCommand(cmd55, 6);
    SD_RecvResponse(res1, 1);

    SD_SendCommand(acmd13, 6);
    SD_RecvResponse(res2, 2);
    SD_DummyRead(5);
    uint8_t recv[16];
    SD_RecvResponse(recv, 16);
    SD_DummyRead(48);
    SD_DummyRead(2);
    psdstatus = (uint32_t *) recv;
    return (errorstatus);
}

/**
 * @brief  SD_ProcessIRQSrc
 * @param  None
 * @retval SD error code or SD_OK
 */
SD_Error SD_ProcessIRQSrc(void)
{
    return SD_OK;
}

/**
 * @brief  This function waits until the SDIO DMA data transfer is finished. 
 *         This function should be called after SDIO_ReadMultiBlocks() function
 *         to insure that all data sent by the card are already transferred by 
 *         the DMA controller.        
 * @param  None.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_WaitReadOperation(void)
{
    return SD_OK;
}

/**
 * @brief  This function waits until the SDIO DMA data transfer is finished. 
 *         This function should be called after SDIO_WriteBlock() and
 *         SDIO_WriteMultiBlocks() function to insure that all data sent by the 
 *         card are already transferred by the DMA controller.        
 * @param  None.
 * @retval SD_Error: SD Card Error code.
 */
SD_Error SD_WaitWriteOperation(void)
{
    return SD_OK;
}

/**
 * @brief  SD_SendCommand
 * @param  cmd
 * @param  len
 * @retval None
 */
void SD_SendCommand(uint8_t *cmd, int len)
{
    uint8_t recv[7];
    cmd[0] |= 0x40;
    cmd[5] = (crc7(cmd, 5) << 1) | 1;
    cmd[6] = 0xFF; //N_CR
#ifdef SD_DEBUG
    printf("sendcmd: ");
    for (int i = 0; i < len + 1; i++)
        printf("%02x ", cmd[i]);
    printf("\n");
#endif
    SD_CS_Clear();
    SPI1_ReadWriteBytes(cmd, recv, 7);
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("recvcmd: ");
    for (int i = 0; i < len + 1; i++)
    {
        printf("%02x ", recv[i]);
    }
    printf("\n");
#endif
}

/**
 * @brief  SD_SendData
 * @param  data
 * @param  len
 * @retval None
 */
void SD_SendData(uint8_t *data, int len)
{
#ifdef SD_DEBUG
    uint8_t recv[10];
    uint8_t temp;
    printf("txData: ");
#endif
    SD_CS_Clear();
    for (int i = 0; i < len; i++)
    {
#ifdef SD_DEBUG
        temp = SPI1_ReadWriteByte(data[i]);
        //printf("%02x ", data[i]);
        if (i > 504)
            recv[i - 505] = temp;
#else
        SPI1_ReadWriteByte(data[i]);
#endif
    }
    SPI1_ReadWriteByte(0xFF); //N_CR
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("\n");
    printf("Last 10 bytes: ");
    for (int i = 0; i < 10; i++)
        printf("%02x ", recv[i]);
    printf("\n");
#endif
}

/**
 * @brief  SD_RecvData
 * @param  data
 * @param  len
 * @retval None
 */
void SD_RecvData(uint8_t *data, int len)
{
    //len = 1;
#ifdef SD_DEBUG
    /*
    printf("before: ");
    for (int i = 0; i < len; i++)
      printf("%02x ", data[i]);
    printf("\n");*/
    printf("rxData: ");
#endif
    SD_CS_Clear();
    /*
    for (int i = 0; i < len; i++)
    {
      rxData[i] = SPI1_ReadWriteByte(0xFF);
  #ifdef SD_DEBUG
      printf("%02x ", rxData[i]);
  #endif
    }*/
    SPI1_ReadBytes(data, len);
    SD_CS_Set();
#ifdef SD_DEBUG
    for (int i = 0; i < len; i++)
        printf("%02x ", data[i]);
    printf("\n");
#endif
}

/**
 * @brief  SD_RecvResponse
 * @param  rxData
 *         len
 * @retval None
 */
void SD_RecvResponse(uint8_t *rxData, int len)
{
#ifdef SD_DEBUG
    printf("rxData: ");
#endif
    SD_CS_Clear();
    for (int i = 0; i < len; i++)
    {
        rxData[i] = SPI1_ReadWriteByte(0xFF);
#ifdef SD_DEBUG
        printf("%02x ", rxData[i]);
#endif
    }
    SD_CS_Set();
#ifdef SD_DEBUG
    //SPI1_ReadWriteByte(0xFF);
    printf("\n");
#endif
}

/**
 * @brief  SD_DummyRead
 * @param  len
 * @retval None
 */
void SD_DummyRead(int len)
{
#ifdef SD_DEBUG
    printf("dummy read: ");
    uint8_t res;
#endif
    SD_CS_Clear();
    for (int i = 0; i < len; i++)
    {
#ifdef SD_DEBUG
        res = SPI1_ReadWriteByte(0xFF);
        printf("%02x ", res);
#else
        SPI1_ReadWriteByte(0xFF);
#endif
    }
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("\n");
#endif
}

/**
 * @brief  Aborts an ongoing data transfer.
 * @param  None
 * @retval None
 */
void SD_StopTransfer(void)
{
#ifdef SD_DEBUG
    uint8_t temp;
    printf("stopTran: ");
#endif
    SD_CS_Clear();
#ifdef SD_DEBUG
    temp = SPI1_ReadWriteByte(0xFD);
    printf("%02x ", temp);
#else
    SPI1_ReadWriteByte(0xFD);
#endif
    SPI1_ReadWriteByte(0xFF); //N_CR
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("\n");
#endif
}

/**
 * @brief  Receive data frame head 0xFE
 * @param  None
 * @retval None
 */
uint8_t SD_RecvDataHead()
{
    uint8_t startToken;
    SD_CS_Clear();
    do
    {
        startToken = SPI1_ReadWriteByte(0xFF);
#ifdef SD_DEBUG
        printf("%02x ", startToken);
#endif
    } while (startToken == 0xFF);
    SD_CS_Set();
    return startToken;
}

/**
 * @brief  CmdError
 * @param  None
 * @retval SD error code or SD_OK
 */
/*
SD_Error CmdError(void)
{
    SD_Error errorstatus = SD_OK;
    SD_CS_Clear();
    uint8_t res0 = SPI1_ReadWriteByte(0xFF);
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("%02x\n", res0);
#endif
    if (res0 != 0x01)
        errorstatus = SD_ERROR;
    return (errorstatus);
}
 */


/**
 * @brief  CmdResp1Error
 * @param  None
 * @retval SD error code or SD_OK
 */
/*
SD_Error CmdResp1Error(uint8_t cmd)
{
    SD_Error errorstatus = SD_OK;
    SD_CS_Clear();
    uint8_t res0 = SPI1_ReadWriteByte(0xFF);
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("%02x\n", res0);
#endif
    if (res0 != 0x01)
        errorstatus = SD_ERROR;
    return (errorstatus);
}
 */

/**
 * @brief  CmdResp3Error
 * @param  None
 * @retval SD error code or SD_OK
 */
SD_Error CmdResp3Error(void)
{
    SD_Error errorstatus = SD_OK;
    uint8_t res3[5];
    SD_CS_Clear();
    for (int i = 0; i < 5; i++)
    {
        res3[i] = SPI1_ReadWriteByte(0xFF);
#ifdef SD_DEBUG
        printf("%02x ", res3[i]);
#endif
    }
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("\n");
#endif
    if (res3[0] == 0x09)
        errorstatus = SD_CMD_CRC_FAIL;
    return (errorstatus);
}

/**
 * @brief  CmdResp7Error
 * @param  None
 * @retval SD error code or SD_OK
 */
SD_Error CmdResp7Error(void)
{
    SD_Error errorstatus = SD_OK;
    uint8_t res7[5];
    SD_CS_Clear();
    for (int i = 0; i < 5; i++)
    {
        res7[i] = SPI1_ReadWriteByte(0xFF);
#ifdef SD_DEBUG
        printf("%02x ", res7[i]);
#endif
    }
    SD_CS_Set();
#ifdef SD_DEBUG
    printf("\n");
#endif
    if (res7[0] == 0x09)
        errorstatus = SD_CMD_CRC_FAIL;
    else if (res7[3] == 0x00)
        errorstatus = SD_ERROR;
    return (errorstatus);
}

uint8_t ReadBuffer[1024];

void SD_Test(void)
{
    //SD_Init(); 
    FATFS fs;
    FRESULT res_sd;
    FIL file;
    UINT fnum;
    TCHAR fname[] = "0:SummerWind.wav";

    f_mount(&fs, "0:", 1);
    res_sd = f_open(&file, (TCHAR *) fname, FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK)
    {
        f_read(&file, ReadBuffer, 1024, &fnum);
        for (int i = 0; i < 1024; i++)
            printf("%02x ", ReadBuffer[i]);
        printf("\n");
    }
    else
    {
        printf("Open file failure!\n");
    }
    f_close(&file);
    f_mount(NULL, "0:", 1);
}