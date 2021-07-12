#include "crc.h"

unsigned int crc_byte(unsigned int cwd, unsigned int plnm, unsigned int dgt)
{
  unsigned int crcc;
  unsigned int crcp;
  unsigned int mxw;

  mxw = (0x1 << dgt) - 1;

  crcp = plnm << 8;
  crcc = 0x1 << (dgt + 8);
  while (cwd > mxw)
  {
    while ((cwd & crcc) == 0)
    {
      crcc >>= 1;
      crcp >>= 1;
    }
    cwd ^= crcp;
  }

  return cwd;
}

unsigned int crc_common(unsigned char *dat, unsigned int cnt, unsigned int plnm, unsigned int dgt)
{
  unsigned int cbr = 0;
  unsigned int cc = 0;

  while (cc < cnt)
  {
    cbr = crc_byte((cbr << 8) | dat[cc], plnm, dgt);
    cc++;
  }
  cc = dgt;
  while (cc > 8)
  {
    cbr = crc_byte(cbr << 8, plnm, dgt);
    cc -= 8;
  }
  cbr = crc_byte(cbr << cc, plnm, dgt);

  return cbr;
}

unsigned char crc7(unsigned char *dat, unsigned int cnt)
{
  return crc_common(dat, cnt, crc7pm, 7);
}

unsigned int crc16(unsigned char *dat, unsigned int cnt)
{
  return crc_common(dat, cnt, crc16pm, 16);
}

unsigned int crc7_word(unsigned int cwd)
{
  unsigned int crcc;
  unsigned int crcp;

  while (cwd > 0x7F)
  {
    crcp = crc7pm << 8;
    crcc = 0x8000;
    while ((cwd & crcc) == 0)
    {
      crcc >>= 1;
      crcp >>= 1;
    }
    cwd ^= crcp;
  }

  return cwd;
}

unsigned char crc7_ori(unsigned char *dat, unsigned int cnt)
{
  unsigned int cbr = 0;
  unsigned int cc = 0;

  while (cc < cnt)
  {
    cbr = crc7_word((cbr << 8) | dat[cc]);
    cc++;
  }
  cbr = crc7_word(cbr << 7);
  return cbr;
}

unsigned int crc16_dword(unsigned int cwd)
{
  unsigned int crcc;
  unsigned int crcp;

  while (cwd > 0xFFFF)
  {
    crcp = crc16pm << 8;
    crcc = 0x1000000;
    while ((cwd & crcc) == 0)
    {
      crcc >>= 1;
      crcp >>= 1;
    }
    cwd ^= crcp;
  }

  return cwd;
}

unsigned int crc16_ori(unsigned char *dat, unsigned int cnt)
{
  unsigned int cbr = 0;
  unsigned int cc = 0;

  while (cc < cnt)
  {
    cbr = crc16_dword((cbr << 8) | dat[cc]);
    cc++;
  }
  cbr = crc16_dword(cbr << 8);
  cbr = crc16_dword(cbr << 8);
  return cbr;
}
