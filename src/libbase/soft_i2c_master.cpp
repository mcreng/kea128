/*
 * soft_i2c_master.h
 *
 * Author: Peter Tse (mcreng)
 * Copyright (c) 2014-2017 HKUST SmartCar Team
 * Refer to LICENSE for details
 */
#include "libbase/soft_i2c_master.h"
#include "libbase/gpio.h"
namespace libbase {

SoftI2cMaster::SoftI2cMaster(libbase::Pin::Name sda, libbase::Pin::Name scl) :
  m_sda(libbase::Gpo(sda, true)), m_scl(libbase::Gpo(scl, true)) {
  Pin::PullEnable(sda, true);
//  Pin::PullEnable(scl, true);
}

SoftI2cMaster::~SoftI2cMaster() {
  Pin::PullEnable((Pin::Name)m_sda.GetPin(), false);
  Pin::PullEnable((Pin::Name)m_scl.GetPin(), false);
}

void SoftI2cMaster::SendStart() {
    SDA1();
    SCL1();
    Delay();
    SDA0();
    Delay();
    SCL0();
}

void SoftI2cMaster::SendStop() {
    SDA0();
    SCL0();
    Delay();
    SCL1();
    Delay();
    SDA1();
    Delay();
}

void SoftI2cMaster::SendACK(bool isACK) {
  SCL0();
  Delay();
  if(isACK) SDA0();
  else      SDA1();
  SCL1();
  Delay();
  SCL0();
  Delay();
}

bool SoftI2cMaster::WaitACK() {
  SCL0();
//  SDA_in();
//  Gpi m_sda_i = m_sda.ToGpi();


  // Set SDA to GPI
  Pin::Name p = (Pin::Name)m_sda.GetPin();
  uint8_t ptx = Pin::getPTX(p);
  uint8_t ptn = Pin::getPTN(p);
  RESET_BIT(MEM_MAPS[ptx]->PIDR, ptn);
  RESET_BIT(MEM_MAPS[ptx]->PDDR, ptn);

  Delay();
  SCL1();
  Delay();
  bool state = ((MEM_MAPS[ptx]->PDIR) >> ptn) & 0x1;
//  if(state)
//  {
////      SDA_out();
//      m_sda = m_sda_i.ToGpo(false);
//      SCL0();
//      Delay();
//      return false;
//  }
//  SDA_out();
  // Set SDA to GPO
  SET_BIT(MEM_MAPS[ptx]->PIDR, ptn);
  SET_BIT(MEM_MAPS[ptx]->PDDR, ptn);

  SCL0();
  Delay();
  return !state;
}

void SoftI2cMaster::SendByte(uint8_t b) {
  uint8_t i = 8;
  while(i--)
  {
      if(b & 0x80)    SDA1();
      else            SDA0();
      b <<= 1;
      Delay();
      SCL1();
      Delay();
      SCL0();
  }
  WaitACK();
}

uint8_t SoftI2cMaster::ReadByte(bool needACK) {
  uint8_t c = 0;
  SCL0();
  Delay();
  SDA1();
//  SDA_in();
//  Gpi m_sda_i = m_sda.ToGpi();

  Pin::Name p = (Pin::Name)m_sda.GetPin();
  uint8_t ptx = Pin::getPTX(p);
  uint8_t ptn = Pin::getPTN(p);
  RESET_BIT(MEM_MAPS[ptx]->PIDR, ptn);
  RESET_BIT(MEM_MAPS[ptx]->PDDR, ptn);

  for(int i=0;i<8;i++) {
      Delay();
      SCL0();
      Delay();
      SCL1();
      Delay();
      c<<=1;
      if(((MEM_MAPS[ptx]->PDIR) >> ptn) & 0x1) c+=1;
  }
//  SDA_out();
//  m_sda = Gpo(m_sda_i.GetPin(), false);
  SET_BIT(MEM_MAPS[ptx]->PIDR, ptn);
  SET_BIT(MEM_MAPS[ptx]->PDDR, ptn);
  SCL0();
  Delay();
  SendACK(needACK);

  return c;
}


void SoftI2cMaster::WriteReg(uint8_t ID, uint8_t reg, uint8_t data) {
  SendStart();
  SendByte( (ID<<1) | 0x00 );
  Delay();
  SendByte(reg);
  Delay();
  SendByte(data);
  Delay();
  SendStop();
  Delay();
}

uint8_t SoftI2cMaster::ReadReg(uint8_t ID, uint8_t reg) {
  uint8_t data;
  SendStart();
  SendByte( (ID<<1) | 0x00 );
  SendByte(reg);

  SendStart();
  SendByte( (ID<<1) | 0x01 );
  data = ReadByte(false);
  SendStop();

  return data;
}

void SoftI2cMaster::ReadRegs(uint8_t ID, uint8_t first_reg, uint8_t count, uint8_t* buf) {
  SendStart();
  SendByte( (ID<<1) | 0x00 );
  SendByte(first_reg);

  SendStart();
  SendByte( (ID<<1) | 0x01 );
  while(--count) {
      *buf = ReadByte(true);
      buf++;
  }
  *buf = ReadByte(false);
  SendStop();
}
}
