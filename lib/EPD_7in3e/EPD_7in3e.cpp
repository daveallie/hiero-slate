#include <Arduino.h>
#include <SPI.h>
#include "EPD_7in3e.h"
#include <Logger.h>

static const char* TAG = "EPD";

EPD_7in3e::EPD_7in3e() {
  reset_pin = 2;
  dc_pin = 15;
  cs_pin = 4;
  busy_pin = 5;
  width = 800;
  height = 480;

  // Setup pins
  pinMode(cs_pin, OUTPUT);
  pinMode(dc_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);
  pinMode(busy_pin, INPUT);

  // Init outputs
  digitalWrite(cs_pin, LOW); // Leave CS low to ensure display is always selected
  digitalWrite(dc_pin, HIGH); // Assume data
  digitalWrite(reset_pin, HIGH); // Leave reset pin high
}

void EPD_7in3e::Init() const {
  Logger::Log(TAG, "Initializing display");

  // Setup SPI
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  digitalWrite(reset_pin, LOW);
  delay(2);
  digitalWrite(reset_pin, HIGH);
  delay(20);
  WaitUntilNotBusy();

  // Initialize the registers
  // https://github.com/A223D/colourEpaperWaveshare
  // https://www.waveshare.com/w/upload/b/bf/SPD1656_1.1.pdf

  // CMDH
  SendCommand(EPD_CMD_UNKNOWN01);
  SendData(0x49);
  SendData(0x55);
  SendData(0x20);
  SendData(0x08);
  SendData(0x09);
  SendData(0x18);

  SendCommand(EPD_CMD_PWR);
  SendData(0x3F);

  SendCommand(EPD_CMD_PSR);
  SendData(0x5F);
  SendData(0x69);

  SendCommand(EPD_CMD_PFS);
  SendData(0x00);
  SendData(0x54);
  SendData(0x00);
  SendData(0x44);

  SendCommand(EPD_CMD_UNKNOWN02);
  SendData(0x40);
  SendData(0x1F);
  SendData(0x1F);
  SendData(0x2C);

  SendCommand(EPD_CMD_BTST);
  SendData(0x6F);
  SendData(0x1F);
  SendData(0x17);
  SendData(0x49);

  SendCommand(EPD_CMD_UNKNOWN03);
  SendData(0x6F);
  SendData(0x1F);
  SendData(0x1F);
  SendData(0x22);

  SendCommand(EPD_CMD_PLL);
  SendData(0x03);

  SendCommand(EPD_CMD_CDI);
  SendData(0x3F);

  SendCommand(EPD_CMD_TCON);
  SendData(0x02);
  SendData(0x00);

  SendCommand(EPD_CMD_TRES);
  SendData(0x03);
  SendData(0x20);
  SendData(0x01);
  SendData(0xE0);

  SendCommand(EPD_CMD_UNKNOWN04);
  SendData(0x01);

  SendCommand(EPD_CMD_PWS);
  SendData(0x2F);

  SendCommand(EPD_CMD_PON);
  WaitUntilNotBusy();
}

void EPD_7in3e::SendCommand(const u8 command) const {
  digitalWrite(dc_pin, LOW);
  // digitalWrite(cs_pin, LOW);
  SPI.transfer(command);
  // digitalWrite(cs_pin, HIGH);
  digitalWrite(dc_pin, HIGH);
}

void EPD_7in3e::SendData(const u8 data) const {
  // digitalWrite(cs_pin, LOW);
  SPI.write(data);
  // digitalWrite(cs_pin, HIGH);
}

void EPD_7in3e::WaitUntilNotBusy(void (*delayFn)(unsigned long)) const {
  while(!digitalRead(busy_pin)) {
    delayFn(1);
  }
}

void EPD_7in3e::WaitUntilNotBusy() const {
  WaitUntilNotBusy(delay);
}

void EPD_7in3e::RefreshDisplay(void (*refreshingFn)(unsigned long)) const {
  Logger::Log(TAG, "Refreshing display");

  SendCommand(EPD_CMD_PON); // POWER_ON
  WaitUntilNotBusy();

  // Second setting
  SendCommand(EPD_CMD_BTST);
  SendData(0x6F);
  SendData(0x1F);
  SendData(0x17);
  SendData(0x49);

  SendCommand(EPD_CMD_DRF);
  SendData(0x00);
  WaitUntilNotBusy(refreshingFn);

  SendCommand(EPD_CMD_POF);
  SendData(0x00);
  WaitUntilNotBusy();
}

void EPD_7in3e::RefreshDisplay() const {
  RefreshDisplay(delay);
}

void EPD_7in3e::StartRenderChunks() const {
  SendCommand(EPD_CMD_DTM1);
  // digitalWrite(cs_pin, LOW);
}

void EPD_7in3e::SendRenderChunk(const u8 image[], const size_t size) const {
  SPI.writeBytes(image, size);
}

void EPD_7in3e::Clear(const u8 color) const {
  SendCommand(EPD_CMD_DTM1);
  for (unsigned long i = 0; i < height; i++) {
    for (unsigned long j = 0; j < width / 2; j++) {
      SendData(color<<4 | color);
    }
    // It seems to crash without a delay
    delayMicroseconds(100);
  }

  RefreshDisplay();
}

void EPD_7in3e::Sleep() const {
  SendCommand(EPD_CMD_POF);
  SendData(0x00);
  WaitUntilNotBusy();

  SendCommand(EPD_CMD_DSLP);
  SendData(0xA5);
  delay(200);
}
