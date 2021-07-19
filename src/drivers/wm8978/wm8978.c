#include "../../config/default/device.h"
#include "wm8978.h"
#include "../i2c/i2c.h"
#include "../i2s/i2s.h"
#include <stdio.h>
#include <string.h>

//#define WM8978_DEBUG

/*------------------------   I2C control WM8978 config  -------------------------------------------*/
static uint8_t regBuf[2] = {0};

static uint8_t wm8978_i2c_WriteReg(uint8_t regAddr, uint16_t regValue);
static uint16_t wm8978_ReadReg(uint8_t regAddr);
static uint8_t wm8978_WriteReg(uint8_t regAddr, uint16_t regValue);

/*
  wm8978 register buffer
  because wm8978 can not read register data by I2C interface, cache them in MCU memory
  update buffer synchronously when write register, return value directly in memory when read register
  register address is 7bit, register data is 9bit
 */
static uint16_t wm8978_RegBuf[] = {
    0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
    0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
    0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
    0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
    0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
    0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
    0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
    0x001, 0x001
};

/**
 * @brief  Writes a Byte to a given register into the audio codec through the 
            control interface (I2C)
 * @param  regAddr: The address (location) of the register to be written.
 * @param  regValue: the Byte value to be written into destination register.
 * @retval 1 if correct communication and 0 if wrong communication
 */
uint8_t wm8978_i2c_WriteReg(uint8_t regAddr, uint16_t regValue)
{
    regBuf[0] = ((regAddr << 1) & 0xFE) | ((regValue >> 8) & 0x1);
    regBuf[1] = regValue & 0xFF;
    if (I2C2_WriteBytes(WM8978_ADDRESS, regBuf, 2))
        return 1;
    else
        return 0;
}

/**
 * @brief  Read register value from buffer
 * @param  regAddr: The address (location) of the register.
 * @retval register value
 */
uint16_t wm8978_ReadReg(uint8_t regAddr)
{
    return wm8978_RegBuf[regAddr];
}

/**
 * @brief  Writes value into wm8978
 * @param  regAddr: The address (location) of the register to be written.
 * @param  regValue: the value to be written into destination register.
 * @retval 1 if success and 0 if failure
 */
uint8_t wm8978_WriteReg(uint8_t regAddr, uint16_t regValue)
{
    uint8_t res;
    res = wm8978_i2c_WriteReg(regAddr, regValue);
    if (res)
        wm8978_RegBuf[regAddr] = regValue;
    return res;
}

/**
 * @brief  WM8978 init
 * @param  None
 * @retval 1: initialize successful
 *         0: initialize failure
 */
uint8_t wm8978_Init(void)
{
#ifdef WM8978_DEBUG
    printf("----wm8978_Init----\n");
#endif
    uint8_t res;
    res = wm8978_Reset(); // hardware reset WM8978 all register to default value
#ifdef WM8978_DEBUG
    printf("res: %d\n", res);
#endif
    return res;
    /*
    wm8978_CfgAudioPath(DAC_ON, SPK_ON | EAR_LEFT_ON | EAR_RIGHT_ON);
    wm8978_SetOUT1Volume(40);
    wm8978_CfgAudioIF(I2S_STANDARD_PHILLIPS, 16);*/
}

/**
 * @brief  reset WM8978,  all register renew to default value
 * @param  None
 * @retval 1: reset successful
 *         0: reset failure
 */
uint8_t wm8978_Reset(void)
{
    // wm8978 register default value
    const uint16_t reg_default[] = {
        0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
        0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
        0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
        0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
        0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
        0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
        0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
        0x001, 0x001
    };
    memcpy(wm8978_RegBuf, reg_default, sizeof (wm8978_RegBuf));

    uint8_t res;
    res = wm8978_WriteReg(0x00, 0x000);
    return res;
}

/**
 * @brief  configure WM8978 audio interface(I2S)
 * @param  interface standard: I2S_Standard_Phillips, I2S_Standard_MSB or I2S_Standard_LSB
 * @param  wordLen: 16, 24 or 32
 * @retval None
 */
void wm8978_CfgAudioIF(uint16_t standard, uint8_t wordLen)
{
    uint16_t usReg = 0;
    /*
      REG R4, audio interface control register
          B8	BCP	 = X, BCLK polarity 0-normal 1-phase reversal
          B7	LRCP = x, LRC clock polarity 0-normal 1-phase reversal
          B6:5	WL = x, word length 00=16bit, 01=20bit, 10=24bit, 11=32bit
          B4:3	FMT = x, audio interface data format, 00=right justified 01=left justified 10=I2S format 11=PCM
          B2	DACLRSWAP = x, control whether DAC data appears in 'right' or 'left' phases of LRC clock 0-left 1-right
          B1 	ADCLRSWAP = x, control whether ADC data appears in 'right' or 'left' phases of LRC clock 0-left 1-right
          B0	MONO	= x, select between stereo and mono device operation 0-stereo 1-mono
     */
    if (standard == I2S_STANDARD_PHILLIPS) // I2S phillips standard
    {
        usReg |= (2 << 3);
    }
    else if (standard == I2S_STANDARD_MSB) // MSB justified standard(left justified)
    {
        usReg |= (1 << 3);
    }
    else if (standard == I2S_STANDARD_LSB) // LSB justified standard(right justified)
    {
        usReg |= (0 << 3);
    }
    else // PCM standard
    {
        usReg |= (3 << 3);
    }

    if (wordLen == 24)
    {
        usReg |= (2 << 5);
    }
    else if (wordLen == 32)
    {
        usReg |= (3 << 5);
    }
    else
    {
        usReg |= (0 << 5); // 16bit
    }
    wm8978_WriteReg(4, usReg);
    /*
          R6, clock generation control register
          MS = 0,  BCLK and LRC clock are inputs
     */
    wm8978_WriteReg(6, 0x000);
    /*
          R10, DAC control register
          SOFTMUTE = 0, softmute disables
          DACOSR = 1, DAC oversampling rate is 128x(best performance)
     */
    wm8978_WriteReg(10, 1 << 3); //R10,SOFTMUTE??,128x??,??SNR
}

/**
 * @brief  output mute
 * @param  mute: mode select
 *         @arg 1: mute
 *         @arg 0: cancel mute
 * @retval None
 */
void wm8978_OutMute(uint8_t mute)
{
    uint16_t usRegValue;
    if (mute == 1) // mute
    {
        usRegValue = wm8978_ReadReg(52); /* Left Mixer Control */
        usRegValue |= (1u << 6);
        wm8978_WriteReg(52, usRegValue);

        usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
        usRegValue |= (1u << 6);
        wm8978_WriteReg(53, usRegValue);

        usRegValue = wm8978_ReadReg(54); /* Right Mixer Control */
        usRegValue |= (1u << 6);
        wm8978_WriteReg(54, usRegValue);

        usRegValue = wm8978_ReadReg(55); /* Right Mixer Control */
        usRegValue |= (1u << 6);
        wm8978_WriteReg(55, usRegValue);
    }
    else // cancel mute
    {
        usRegValue = wm8978_ReadReg(52);
        usRegValue &= ~(1u << 6);
        wm8978_WriteReg(52, usRegValue);

        usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
        usRegValue &= ~(1u << 6);
        wm8978_WriteReg(53, usRegValue);

        usRegValue = wm8978_ReadReg(54);
        usRegValue &= ~(1u << 6);
        wm8978_WriteReg(54, usRegValue);

        usRegValue = wm8978_ReadReg(55); /* Left Mixer Control */
        usRegValue &= ~(1u << 6);
        wm8978_WriteReg(55, usRegValue);
    }
}

/**
 * @brief  close WM8978
 * @param  None
 * @retval None
 */
void wm8978_PowerDown(void)
{
    wm8978_Reset(); // hardware reset WM8978 all register to default value
}

/**
 * @brief  config wm8978 audio channel
 * @param  inPath: audio input channel
 * @param  outPath: audio output channel
 * @retval None
 */
void wm8978_CfgAudioPath(uint16_t inPath, uint16_t outPath)
{
    uint16_t usReg;
    if ((inPath == IN_PATH_OFF) && (outPath == OUT_PATH_OFF))
    {
        wm8978_PowerDown();
        return;
    }
    /*
          R1,  Power manage 1
          Bit8    BUFDCOPEN, Output stage 1.5xAVDD/2 driver enable
          Bit7    OUT4MIXEN, OUT4 mixer enable
          Bit6    OUT3MIXEN, OUT3 mixer enable
          Bit5    PLLEN, unused
          Bit4    MICBEN, Microphone Bias Enable (MIC bias enable)
          Bit3    BIASEN, Analogue amplifier bias control, must be set 1
          Bit2    BUFIOEN, Unused input/output tie off buffer enable
          Bit1:0  VMIDSEL, must be set non-zero value
     */
    usReg = (1 << 3) | (3 << 0);
    if (outPath & OUT3_4_ON) // OUT3 and OUT4 enable output to GSM module
    {
        usReg |= ((1 << 7) | (1 << 6));
    }
    if ((inPath & MIC_LEFT_ON) || (inPath & MIC_RIGHT_ON))
    {
        usReg |= (1 << 4);
    }
    wm8978_WriteReg(1, usReg);
    /*
          R2,  Power manage 2
          Bit8	ROUT1EN,	ROUT1 output enable
          Bit7	LOUT1EN,	LOUT1 output enable
          Bit6	SLEEP, 		0 = Normal device operation   1 = Residual current reduced in device standby mode
          Bit5	BOOSTENR,	Right channel Input BOOST enable
          Bit4	BOOSTENL,	Left channel Input BOOST enable
          Bit3	INPGAENR,	Right channel input PGA enable
          Bit2	INPGAENL,	Left channel input PGA enable
          Bit1	ADCENR,		Enable ADC right channel
          Bit0	ADCENL,		Enable ADC left channel
     */
    usReg = 0;
    if (outPath & EAR_LEFT_ON)
    {
        usReg |= (1 << 7);
    }
    if (outPath & EAR_RIGHT_ON)
    {
        usReg |= (1 << 8);
    }
    if (inPath & MIC_LEFT_ON)
    {
        usReg |= ((1 << 4) | (1 << 2));
    }
    if (inPath & MIC_RIGHT_ON)
    {
        usReg |= ((1 << 5) | (1 << 3));
    }
    if (inPath & LINE_ON)
    {
        usReg |= ((1 << 4) | (1 << 5));
    }
    if (inPath & ADC_ON)
    {
        usReg |= ((1 << 1) | (1 << 0));
    }
    wm8978_WriteReg(2, usReg);

    /*
          R3,  Power manage 3
          Bit8	OUT4EN,		OUT4 enable
          Bit7	OUT3EN,		OUT3 enable
          Bit6	LOUT2EN,	LOUT2 output enable
          Bit5	ROUT2EN,	ROUT2 output enable
          Bit4	0,
          Bit3	RMIXEN,		Right mixer enable
          Bit2	LMIXEN,		Left mixer enable
          Bit1	DACENR,		Right channel DAC enable
          Bit0	DACENL,		Left channel DAC enable
     */
    usReg = 0;
    if (outPath & OUT3_4_ON)
    {
        usReg |= ((1 << 8) | (1 << 7));
    }
    if (outPath & SPK_ON)
    {
        usReg |= ((1 << 6) | (1 << 5));
    }
    if (outPath != OUT_PATH_OFF)
    {
        usReg |= ((1 << 3) | (1 << 2));
    }
    if (inPath & DAC_ON)
    {
        usReg |= ((1 << 1) | (1 << 0));
    }
    wm8978_WriteReg(3, usReg);

    /*
          R44, Input ctrl
          Bit8	MBVSEL, 		Microphone Bias Voltage Control   0 = 0.9 * AVDD   1 = 0.6 * AVDD
          Bit7	0
          Bit6	R2_2INPPGA,		Connect R2 pin to right channel input PGA positive terminal
          Bit5	RIN2INPPGA,		Connect RIN pin to right channel input PGA negative terminal
          Bit4	RIP2INPPGA,		Connect RIP pin to right channel input PGA amplifier positive terminal
          Bit3	0
          Bit2	L2_2INPPGA,		Connect L2 pin to left channel input PGA positive terminal
          Bit1	LIN2INPPGA,		Connect LIN pin to left channel input PGA negative terminal
          Bit0	LIP2INPPGA,		Connect LIP pin to left channel input PGA amplifier positive terminal
     */
    usReg = 0 << 8;
    if (inPath & LINE_ON)
    {
        usReg |= ((1 << 6) | (1 << 2));
    }
    if (inPath & MIC_RIGHT_ON)
    {
        usReg |= ((1 << 5) | (1 << 4));
    }
    if (inPath & MIC_LEFT_ON)
    {
        usReg |= ((1 << 1) | (1 << 0));
    }
    wm8978_WriteReg(44, usReg);
    /*
          R14, ADC Control
          Bit8 	HPFEN,	High Pass Filter Enable
          BIt7 	HPFAPP,	Select audio mode or application mode
          Bit6:4	HPFCUT?Application mode cut-off frequency
          Bit3 	ADCOSR,	ADC oversample rate select: 0=64x (lower power) 1=128x (best performance)
          Bit2   	0
          Bit1 	ADC right channel polarity adjust:  0=normal  1=inverted
          Bit0 	ADC left channel polarity adjust:  0=normal 1=inverted
     */
    if (inPath & ADC_ON)
    {
        usReg = (1 << 3) | (0 << 8) | (4 << 0); // disable ADC high pass filter, set cut-off frequency
    }
    else
    {
        usReg = 0;
    }
    wm8978_WriteReg(14, usReg);

    /* 
          set notch filter
          R27, R28, R29, R30
          R7 Bit7 NFEN
     */
    if (inPath & ADC_ON)
    {
        usReg = (0 << 7);
        wm8978_WriteReg(27, usReg);
        usReg = 0;
        wm8978_WriteReg(28, usReg);
        wm8978_WriteReg(29, usReg);
        wm8978_WriteReg(30, usReg);
    }
    /*  ALC, R32  - 34 */
    {
        usReg = 0;
        wm8978_WriteReg(32, usReg);
        wm8978_WriteReg(33, usReg);
        wm8978_WriteReg(34, usReg);
    }
    /*  
      R35  ALC Noise Gate Control
          Bit3	NGATEN, Noise gate function enable
          Bit2:0	Noise gate threshold:
     */
    usReg = (3 << 1) | (7 << 0); // disable noise gate function
    wm8978_WriteReg(35, usReg);

    /*
          Mic input channel BOOST control by PGABOOSTL and PGABOOSTR
          Aux input channel BOOST control by AUXL2BOOSTVO[2:0] and AUXR2BOOSTVO[2:0]
          Line input channel BOOST control by LIP2BOOSTVOL[2:0] and RIP2BOOSTVOL[2:0]
     */
    /*
      R47 (left channel), R48 (right channel), MIC
          R47 (the same as R48)
          B8		PGABOOSTL	= 1,
          B7		= 0, res
          B6:4	L2_2BOOSTVOL = x, 0: disable 1-7: -12dB ~ +6dB
          B3		= 0, res
          B2:0`	AUXL2BOOSTVOL = x, 0: disable 1-7: -12dB ~ +6dB
     */
    usReg = 0;
    if ((inPath & MIC_LEFT_ON) || (inPath & MIC_RIGHT_ON))
    {
        usReg |= (1 << 8); // MIC gain +20dB
    }
    if (inPath & AUX_ON)
    {
        usReg |= (3 << 0); // Aux gain fix 3
    }
    if (inPath & LINE_ON)
    {
        usReg |= (3 << 4); // Line gain fix 3
    }
    wm8978_WriteReg(47, usReg);
    wm8978_WriteReg(48, usReg);
    /* digital ADC input volumn control
          R15 control left channel, R16 control right channel
          Bit8 	ADCVU = 1
          Bit7:0 gain select 0000 0000 = mute
                             0000 0001 = -127dB
                             0000 0010 = -12.5dB  0.5dB step size
                             1111 1111 = 0dB
     */
    usReg = 0xFF;
    wm8978_WriteReg(15, usReg);
    usReg = 0x1FF;
    wm8978_WriteReg(16, usReg);

    // set mic PGA gain by wm8978_SetMicGain
    /*	
      R43  AUXR ? ROUT2 BEEP Mixer Function
          B8:6 = 0
          B5	 MUTERPGA2INV,	Mute input to INVROUT2 mixer
          B4	 INVROUT2,  Invert ROUT2 output
          B3:1 BEEPVOL = 7;	AUXR input to ROUT2 inverter gain
          B0	 BEEPEN = 1;	Enable AUXR beep input
     */
    usReg = 0;
    if (outPath & SPK_ON)
    {
        usReg |= (1 << 4); // ROUT2 invert phase
    }
    if (inPath & AUX_ON)
    {
        usReg |= ((7 << 1) | (1 << 0));
    }
    wm8978_WriteReg(43, usReg);

    /* 
      R49  Output ctrl
          B8:7	0
          B6		DACL2RMIX,	Left DAC output to right output mixer
          B5		DACR2LMIX,	Right DAC output to left output
          B4		OUT4BOOST,	0 = OUT4 output gain = -1; DC = AVDD / 2; 1 = OUT4 output gain = +1.5; DC = 1.5 x AVDD / 2
          B3		OUT3BOOST,	0 = OUT3 output gain = -1; DC = AVDD / 2; 1 = OUT3 output gain = +1.5; DC = 1.5 x AVDD / 2
          B2		SPKBOOST,	0 = Speaker gain = -1;  DC = AVDD / 2 ; 1 = Speaker gain = +1.5; DC = 1.5 x AVDD / 2
          B1		TSDEN,   Thermal Shutdown Enable
          B0		VROI,	Disabled Outputs to VREF Resistance
     */
    usReg = 0;
    if (inPath & DAC_ON)
    {
        usReg |= ((1 << 6) | (1 << 5));
    }
    if (outPath & SPK_ON)
    {
        usReg |= ((1 << 2) | (1 << 1)); // SPK 1.5x gain, thermal shutdown enable
    }
    if (outPath & OUT3_4_ON)
    {
        usReg |= ((1 << 4) | (1 << 3)); // BOOT3  BOOT4  1.5x gain
    }
    wm8978_WriteReg(49, usReg);

    /*	
      REG 50 (50-left 51-right)
          B8:6	AUXLMIXVOL = 111	
          B5		AUXL2LMIX = 1	Left Auxilliary input to left channel
          B4:2      BYPLMIXVOL		volume
          B1		BYPL2LMIX = 0;	Left bypass path (from the left channel input boost output) to left output mixer
          B0		DACL2LMIX = 1;	Left DAC output to left output mixer
     */
    usReg = 0;
    if (inPath & AUX_ON)
    {
        usReg |= ((7 << 6) | (1 << 5));
    }
    if ((inPath & LINE_ON) || (inPath & MIC_LEFT_ON) || (inPath & MIC_RIGHT_ON))
    {
        usReg |= ((7 << 2) | (1 << 1));
    }
    if (inPath & DAC_ON)
    {
        usReg |= (1 << 0);
    }
    wm8978_WriteReg(50, usReg);
    wm8978_WriteReg(51, usReg);

    /*	
      R56   OUT3 mixer ctrl
          B8:7	0
          B6		OUT3MUTE,  	0 = Output stage outputs OUT3 mixer;  1 = Output stage muted ? drives out VMID.
          B5:4	0
          B3		BYPL2OUT3,	OUT4 mixer output to OUT3
          B4		0
          B2		LMIX2OUT3,	Left ADC input to OUT3
          B1		LDAC2OUT3,	Left DAC mixer to OUT3
          B0		LDAC2OUT3,	Left DAC output to OUT3
     */
    usReg = 0;
    if (outPath & OUT3_4_ON)
    {
        usReg |= (1 << 3);
    }
    wm8978_WriteReg(56, usReg);

    /* 
      R57		OUT4 (MONO) mixer ctrl
          B8:7	0
          B6		OUT4MUTE,	0 = Output stage outputs OUT4 mixer  1 = Output stage muted ? drives outVMID.
          B5		HALFSIG,	0 = OUT4 normal output	1 = OUT4 attenuated by 6dB
          B4		LMIX2OUT4,	Left DAC mixer to OUT4
          B3		LDAC2UT4,	Left DAC to OUT4
          B2		BYPR2OUT4,	Right ADC input to OUT4
          B1		RMIX2OUT4,	Right DAC mixer to OUT4
          B0		RDAC2OUT4,	Right DAC output to OUT4
     */
    usReg = 0;
    if (outPath & OUT3_4_ON)
    {
        usReg |= ((1 << 4) | (1 << 1));
    }
    wm8978_WriteReg(57, usReg);

    /* 
      R11, 12   DAC digital volume
          R11		Left DAC Digital Volume
          R12		Right DAC Digital Volume
     */
    if (inPath & DAC_ON)
    {
        wm8978_WriteReg(11, 255);
        wm8978_WriteReg(12, 255 | 0x100);
    }
    else
    {
        wm8978_WriteReg(11, 0);
        wm8978_WriteReg(12, 0 | 0x100);
    }

    /*	
      R10 DAC Control
          B8	0
          B7	0
          B6	SOFTMUTE,	Softmute enable:
          B5	0
          B4	0
          B3	DACOSR128,	DAC oversampling rate: 0=64x (lowest power) 1=128x (best performance)
          B2	AMUTE,		Automute enable
          B1	DACPOLR,	Right DAC output polarity
          B0	DACPOLL,	Left DAC output polarity:
     */
    if (inPath & DAC_ON)
    {
        wm8978_WriteReg(10, 0);
    }
}

/**
 * @brief  set mic gain
 * @param  gain: 0-63
 * @retval None
 */
void wm8978_SetMicGain(uint8_t gain)
{
    if (gain > GAIN_MAX)
    {
        gain = GAIN_MAX;
    }
    /* 
      PGA volume control  R45, R46 
          Bit8	INPPGAUPDATE
          Bit7	INPPGAZCL		
          Bit6	INPPGAMUTEL	
          Bit5:0 gain value
     */
    wm8978_WriteReg(45, gain);
    wm8978_WriteReg(46, gain | (1 << 8));
}

/**
 * @brief  set line gain
 * @param  gain: 0-7
 * @retval None
 */
void wm8978_SetLineGain(uint8_t gain)
{
    uint16_t usRegValue;
    if (gain > 7)
    {
        gain = 7;
    }
    /*
          Mic input channel BOOST control by PGABOOSTL and PGABOOSTR
          Aux input channel BOOST control by AUXL2BOOSTVO[2:0] and AUXR2BOOSTVO[2:0]
          Line input channel BOOST control by LIP2BOOSTVOL[2:0] and RIP2BOOSTVOL[2:0]
     */
    /*
      R47 (left channel), R48 (right channel), MIC
          R47 (the same as R48)
          B8		PGABOOSTL	= 1,
          B7		= 0, res
          B6:4	L2_2BOOSTVOL = x, 0: disable 1-7: -12dB ~ +6dB
          B3		= 0, res
          B2:0`	AUXL2BOOSTVOL = x, 0: disable 1-7: -12dB ~ +6dB
     */
    usRegValue = wm8978_ReadReg(47);
    usRegValue &= 0x8F; // clear Bit6:4   1000 1111
    usRegValue |= (gain << 4);
    wm8978_WriteReg(47, usRegValue);

    usRegValue = wm8978_ReadReg(48);
    usRegValue &= 0x8F; // clear Bit6:4   1000 1111
    usRegValue |= (gain << 4);
    wm8978_WriteReg(48, usRegValue);
}

/**
 * @brief  modificate output channel 2 volume
 * @param  volume: 0-63
 * @retval None
 */
void wm8978_SetOUT2Volume(uint8_t volume)
{
    uint16_t regL;
    uint16_t regR;

    if (volume > VOLUME_MAX)
    {
        volume = VOLUME_MAX;
    }

    regL = volume;
    regR = volume;

    /*
          R54	LOUT2 (SPK) Volume control
          R55	ROUT2 (SPK) Volume control
     */
    wm8978_WriteReg(54, regL | 0x00);
    wm8978_WriteReg(55, regR | 0x100);
}

/**
 * @brief  modificate output channel 1 volume
 * @param  volume: 0-63
 * @retval None
 */
void wm8978_SetOUT1Volume(uint8_t volume)
{
    uint16_t regL;
    uint16_t regR;

    if (volume > VOLUME_MAX)
    {
        volume = VOLUME_MAX;
    }

    regL = volume;
    regR = volume;

    /*
          R52	LOUT1 Volume control
          R53	ROUT1 Volume control
     */
    wm8978_WriteReg(52, regL | 0x00);
    wm8978_WriteReg(53, regR | 0x100);
}

/**
 * @brief  read output channel 1 volume
 * @param  None
 * @retval volume
 */
uint8_t wm8978_ReadOUT1Volume(void)
{
    return (uint8_t) (wm8978_ReadReg(52) & 0x3F);
}

/**
 * @brief  read output channel 2 volume
 * @param  None
 * @retval volume
 */
uint8_t wm8978_ReadOUT2Volume(void)
{
    return (uint8_t) (wm8978_ReadReg(54) & 0x3F);
}

/**
 * @brief  set notch filter
 * @param  NFA0[13:0] and NFA1[13:0]
 * @retval None
 */
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1)
{
    uint16_t usReg;
    /*
          A programmable notch filter is provided. This filter has a variable centre frequency and bandwidth,
          programmable via two coefficients, a0 and a1. a0 and a1 are represented by the register bits
          NFA0[13:0] and NFA1[13:0]. Because these coefficient values require four register writes to setup
          there is an NFU (Notch Filter Update) flag which should be set only when all four registers are setup.
     */
    usReg = (1 << 7) | (_NFA0 & 0x3F);
    wm8978_WriteReg(27, usReg);

    usReg = ((_NFA0 >> 7) & 0x3F);
    wm8978_WriteReg(28, usReg);

    usReg = (_NFA1 & 0x3F);
    wm8978_WriteReg(29, usReg);

    usReg = (1 << 8) | ((_NFA1 >> 7) & 0x3F);
    wm8978_WriteReg(30, usReg);
}