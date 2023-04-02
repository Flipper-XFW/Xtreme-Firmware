/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"
#include "furi_hal_rtc.h"

/** logical drive path */
char fatfs_path[4];
/** File system object */
FATFS fatfs_object;

void fatfs_init(void) {
    FATFS_LinkDriver(&sd_fatfs_driver, fatfs_path);
}

/** Gets Time from RTC
  *
  * @return     Time in DWORD (toasters per square washing machine)
  */
DWORD get_fattime() {
    FuriHalRtcDateTime furi_time;
    furi_hal_rtc_get_datetime(&furi_time);

    return ((uint32_t)(furi_time.year - 1980) << 25) | furi_time.month << 21 |
           furi_time.day << 16 | furi_time.hour << 11 | furi_time.minute << 5 | furi_time.second;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
