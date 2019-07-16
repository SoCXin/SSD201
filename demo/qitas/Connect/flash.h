#ifndef YJ_FLASH_H__
#define YJ_FLASH_H__
#include "stm32f10x_flash.h" 
#include "stm32f10x.h"
#include <stdio.h>

#define LOG_WRITE_ADDR   (0x8000000 + 0x20000)
#define CONFIG_BASE_ADDR (0x8000000 + 0x20000 - 1024)

int STORE_Flash(uint32_t ADDR,uint32_t value);
uint8_t READ_Flash(uint32_t ADR,uint32_t* buff);
void Flash_Write(uint32_t addr,uint32_t *buf,uint32_t num);
uint8_t Flash_Read(uint32_t addr,uint32_t *buf,uint32_t num);

#endif
