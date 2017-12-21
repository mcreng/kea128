/*
 * sys_tick.h
 *
 * Author: Peter Tse (mcreng)
 * Copyright (c) 2014-2017 HKUST SmartCar Team
 * Refer to LICENSE for details
 */
#include "libbase/cmsis/SKEAZ1284.h"
#pragma once

namespace libbase {
class Systick {
public:

  /**
   * @param Number of clock cycle per tick
   */
  static void Init(uint32_t);
  static void StartCount(uint32_t = 0x00);
  static void DelayCycle(uint32_t);
  static void DelayMs(uint32_t);
  static uint32_t GetTime();

  static volatile uint32_t m_cur_ticks;
private:
  static uint32_t m_period_per_tick;  // tick per cycle period


}; // namespace Systick
} // namespace libbase
