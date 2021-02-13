#ifndef __STM32F1xx_HAL_DEF
#define __STM32F1xx_HAL_DEF
#include <wk/kernel.h>
#include "stm32f103_xe.h"

typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

#endif
