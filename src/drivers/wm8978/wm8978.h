#ifndef WM8978_H
#define WM8978_H

/*------------------------   I2C control WM8978 config  -------------------------------------------*/

/* WM8978 audio input channel select, can select multi channels at the same time. etc, MIC_LEFT_ON | LINE_ON */
typedef enum {
    IN_PATH_OFF = 0x00, // no input
    MIC_LEFT_ON = 0x01, // LIN, LIP foot, MIC left channel
    MIC_RIGHT_ON = 0x02, // RIN, RIP foot, MIC right channel
    LINE_ON = 0x04, // L2, R2 stereo input
    AUX_ON = 0x08, // AUXL, AUXR stereo
    DAC_ON = 0x10, // I2S data DAC(CPU generate audio signal)
    WM_ADC_ON = 0x20 // input audio flow to WM8978 internal ADC(I2S record)
} IN_PATH_E;

/* WM8978 audio output channel select, can select multi channels at the same time. */
typedef enum {
    OUT_PATH_OFF = 0x00, // no output
    EAR_LEFT_ON = 0x01, // LOUT1 earphone left channel
    EAR_RIGHT_ON = 0x02, // ROUT1 earphone right channel
    SPK_ON = 0x04, // LOUT2 and ROUT2 convert phase output single channel
    OUT3_4_ON = 0x08, // OUT3 and OUT4 output single channel
} OUT_PATH_E;

/* define max volume */
#define VOLUME_MAX 63 // max volumn
#define VOLUME_STEP 1 // adjust volumn step size

/* define max MIC gain */
#define GAIN_MAX 63 // max MIC gain
#define GAIN_STEP 1 // adjust MIC gain step size

/* WM8978 I2C address */
#define WM8978_ADDRESS 0x1A

uint8_t wm8978_Init(void);
uint8_t wm8978_Reset(void);
void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void wm8978_OutMute(uint8_t _ucMute);
void wm8978_PowerDown(void);
void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void wm8978_SetMicGain(uint8_t _ucGain);
void wm8978_SetLineGain(uint8_t _ucGain);
void wm8978_SetOUT2Volume(uint8_t _ucVolume);
void wm8978_SetOUT1Volume(uint8_t _ucVolume);
uint8_t wm8978_ReadOUT1Volume(void);
uint8_t wm8978_ReadOUT2Volume(void);
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);

/*------------------------   I2S control data transfer  ---------------------------------------------*/
/**
 * WM8978_LRC    -> RA3/I2S2_WS
 * WM8978_BCLK   -> RB15/I2S2_CK
 * WM8978_DACDAT -> RA4/I2S2_SD
 * WM8978_MCLK   -> RA2/I2S2_MCK
 */

#endif
