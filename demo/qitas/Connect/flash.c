#include "flash.h"


/**
 * @brief STORE_Flash flash写入一个值 
 *
 * @param ADDR 地址
 * @param value 值
 *
 * @return 是否写入成功 
 */
int STORE_Flash(uint32_t ADDR,uint32_t value)
{
    uint32_t Flash_buff;
    FLASH_Unlock();
    FLASH_ErasePage(ADDR);    
    FLASH_Lock();    
    FLASH_Unlock();        
    FLASH_ProgramWord(ADDR,value); 
    FLASH_Lock();
    
    Flash_buff=*(vu32*)(ADDR);
    if(Flash_buff==value) return 1;
    else return 0;
}

/**
 * @brief READ_Flash Flash里读一个值
 *
 * @param ADR
 * @param buff
 *
 * @return 
 */
uint8_t READ_Flash(uint32_t ADR,uint32_t* buff)
{
    if(*(vu32*)(ADR)==0xffffffff)
    {
        *buff=0;
        return 0;
    }
    else 
    {
        *buff=*(vu32*)(ADR);
        return 1;
    }
}

/**
 * @brief Flash_Write Flash里写一串值
 *
 * @param addr
 * @param buf
 * @param num
 */
void Flash_Write(uint32_t addr,uint32_t *buf,uint32_t num)
{
    int len;
    FLASH_Unlock();
    FLASH_ErasePage(addr);    
    FLASH_Lock();    
    FLASH_Unlock();        
    for (len = 0; len < num; ++len)
    {
        FLASH_ProgramWord(addr,buf[len]); 
        addr += 4;
    }
    FLASH_Lock();    
}


/**
 * @brief Flash_Read flash里读一串值
 *
 * @param addr
 * @param buf
 * @param num
 *
 * @return 
 */
uint8_t Flash_Read(uint32_t addr,uint32_t *buf,uint32_t num)
{
    uint8_t err=0;
    while(num--)
    {
        if(READ_Flash(addr,buf))
        {
            err++;
        }
        
        buf++;
        addr+=4;
    }
    if(err)
    return 1;
    else 
    return 0;    
}

