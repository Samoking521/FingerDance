#ifndef CRC_H
#define CRC_H

#define crc7pm 0x89
#define crc16pm 0x11021

unsigned int crc_byte(unsigned int cwd, unsigned int plnm, unsigned int dgt);
unsigned int crc_common(unsigned char *dat, unsigned int cnt, unsigned int plnm, unsigned int dgt);
unsigned char crc7(unsigned char *dat, unsigned int cnt);
unsigned int crc16(unsigned char *dat, unsigned int cnt);
unsigned int crc7_word(unsigned int cwd);
unsigned char crc7_ori(unsigned char *dat, unsigned int cnt);
unsigned int crc16_dword(unsigned int cwd);
unsigned int crc16_ori(unsigned char *dat, unsigned int cnt);

#endif /* CRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
