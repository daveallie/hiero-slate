#ifndef EPD_7IN3E_H
#define EPD_7IN3E_H

// Colors
#define EPD_7IN3E_BLACK   0x0   /// 000
#define EPD_7IN3E_WHITE   0x1   /// 001
#define EPD_7IN3E_YELLOW  0x2   /// 010
#define EPD_7IN3E_RED     0x3   /// 011
#define EPD_7IN3E_BLUE    0x5   /// 101
#define EPD_7IN3E_GREEN   0x6   /// 110

// Commands
#define EPD_CMD_UNKNOWN01 0xAA
#define EPD_CMD_UNKNOWN02 0x05
#define EPD_CMD_UNKNOWN03 0x08
#define EPD_CMD_UNKNOWN04 0x84
#define EPD_CMD_PSR 0x00
#define EPD_CMD_PWR 0x01
#define EPD_CMD_POF 0x02
#define EPD_CMD_PFS 0x03
#define EPD_CMD_PON 0x04
#define EPD_CMD_BTST 0x06
#define EPD_CMD_DSLP 0x07
#define EPD_CMD_DTM1 0x10
#define EPD_CMD_DSP 0x11
#define EPD_CMD_DRF 0x12
#define EPD_CMD_LUTC 0x20
#define EPD_CMD_LUTB 0x21
#define EPD_CMD_LUTW 0x22
#define EPD_CMD_LUTG1 0x23
#define EPD_CMD_LUTG2 0x24
#define EPD_CMD_LUTR0 0x25
#define EPD_CMD_LUTR1 0x26
#define EPD_CMD_LUTR2 0x27
#define EPD_CMD_LUTR3 0x28
#define EPD_CMD_LUTXON 0x29
#define EPD_CMD_PLL 0x30
#define EPD_CMD_TSC 0x40
#define EPD_CMD_TSE 0x41
#define EPD_CMD_TSW 0x42
#define EPD_CMD_TSR 0x43
#define EPD_CMD_CDI 0x50
#define EPD_CMD_LPD 0x51
#define EPD_CMD_TCON 0x60
#define EPD_CMD_TRES 0x61
#define EPD_CMD_DAM 0x65
#define EPD_CMD_FLG 0x71
#define EPD_CMD_AMV 0x80
#define EPD_CMD_VV 0x81
#define EPD_CMD_VDCS 0x82
#define EPD_CMD_CCSET 0xE0
#define EPD_CMD_PWS 0xE3

class EPD_7in3e {
public:
  EPD_7in3e();
  void Init() const;
  void StartRenderChunks() const;
  void SendRenderChunk(const u8 image[], size_t size) const;
  void RefreshDisplay() const;
  void RefreshDisplay(void (*refreshingFn)(unsigned long)) const;
  void Sleep() const;
  void Clear(u8 color) const;

private:
  unsigned int reset_pin;
  unsigned int dc_pin;
  unsigned int cs_pin;
  unsigned int busy_pin;
  unsigned long width;
  unsigned long height;

  void WaitUntilNotBusy() const;
  void WaitUntilNotBusy(void (*delayFn)(unsigned long)) const;
  void Reset() const;
  void SendCommand(u8 command) const;
  void SendData(u8 data) const;
};

#endif
