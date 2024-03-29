#ifndef __STM32F4XX_HAL_PCD_H
#define __STM32F4XX_HAL_PCD_H


#include "stm32f4xx_ll_usb.h"

typedef enum 
{
    HAL_PCD_STATE_RESET   = 0x00U,
    HAL_PCD_STATE_READY   = 0x01U,
    HAL_PCD_STATE_ERROR   = 0x02U,
    HAL_PCD_STATE_BUSY    = 0x03U,
    HAL_PCD_STATE_TIMEOUT = 0x04U
} PCD_StateTypeDef;

#ifdef USB_OTG_GLPMCFG_LPMEN
/* Device LPM suspend state */
typedef enum  
{
    LPM_L0 = 0x00U, /* on */
    LPM_L1 = 0x01U, /* LPM L1 sleep */
    LPM_L2 = 0x02U, /* suspend */
    LPM_L3 = 0x03U  /* off */
} PCD_LPM_StateTypeDef;
#endif /* USB_OTG_GLPMCFG_LPMEN */

typedef USB_OTG_GlobalTypeDef  PCD_TypeDef;
typedef USB_OTG_CfgTypeDef     PCD_InitTypeDef;
typedef USB_OTG_EPTypeDef      PCD_EPTypeDef;

/** 
  * @brief  PCD Handle Structure definition  
  */ 
typedef struct
{
    PCD_TypeDef             *Instance;    /*!< Register base address              */
    PCD_InitTypeDef         Init;         /*!< PCD required parameters            */
    PCD_EPTypeDef           IN_ep[16U];   /*!< IN endpoint parameters             */
    PCD_EPTypeDef           OUT_ep[16U];  /*!< OUT endpoint parameters            */
    __IO PCD_StateTypeDef   State;        /*!< PCD communication state            */
    uint32_t                Setup[12U];   /*!< Setup packet buffer                */
#ifdef USB_OTG_GLPMCFG_LPMEN
    PCD_LPM_StateTypeDef    LPM_State;    /*!< LPM State                          */
    uint32_t                BESL;
    uint32_t                lpm_active;   /*!< Enable or disable the Link Power Management .
                                            This parameter can be set to ENABLE or DISABLE */
#endif /* USB_OTG_GLPMCFG_LPMEN */
#ifdef USB_OTG_GCCFG_BCDEN
    uint32_t battery_charging_active;     /*!< Enable or disable Battery charging.
                                        This parameter can be set to ENABLE or DISABLE */
#endif /* USB_OTG_GCCFG_BCDEN */
    void                    *pData;       /*!< Pointer to upper stack Handler */
} PCD_HandleTypeDef;

#include "stm32f4xx_hal_pcd_ex.h"

/* Exported constants --------------------------------------------------------*/
/** @defgroup PCD_Exported_Constants PCD Exported Constants
  * @{
  */

/** @defgroup PCD_Speed PCD Speed
  * @{
  */
#define PCD_SPEED_HIGH               0U
#define PCD_SPEED_HIGH_IN_FULL       1U
#define PCD_SPEED_FULL               2U
/**
  * @}
  */
  
/** @defgroup PCD_PHY_Module PCD PHY Module
  * @{
  */
#define PCD_PHY_ULPI                 1U
#define PCD_PHY_EMBEDDED             2U
/**
  * @}
  */
  
/** @defgroup PCD_Turnaround_Timeout Turnaround Timeout Value
  * @{
  */
#ifndef USBD_HS_TRDT_VALUE
 #define USBD_HS_TRDT_VALUE           9U
#endif /* USBD_HS_TRDT_VALUE */
#ifndef USBD_FS_TRDT_VALUE
 #define USBD_FS_TRDT_VALUE           5U
#endif /* USBD_FS_TRDT_VALUE */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup PCD_Exported_Macros PCD Exported Macros
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
#define __HAL_PCD_ENABLE(__HANDLE__)                   USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_PCD_DISABLE(__HANDLE__)                  USB_DisableGlobalInt ((__HANDLE__)->Instance)

#define __HAL_PCD_GET_FLAG(__HANDLE__, __INTERRUPT__)      ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) == (__INTERRUPT__))
#define __HAL_PCD_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    (((__HANDLE__)->Instance->GINTSTS) &= (__INTERRUPT__))
#define __HAL_PCD_IS_INVALID_INTERRUPT(__HANDLE__)         (USB_ReadInterrupts((__HANDLE__)->Instance) == 0U)

#define __HAL_PCD_UNGATE_PHYCLOCK(__HANDLE__)             *(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE) &= \
                                                           ~(USB_OTG_PCGCCTL_STOPCLK)

#define __HAL_PCD_GATE_PHYCLOCK(__HANDLE__)               *(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE) |= USB_OTG_PCGCCTL_STOPCLK

#define __HAL_PCD_IS_PHY_SUSPENDED(__HANDLE__)            ((*(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE))&0x10U)

#define USB_OTG_FS_WAKEUP_EXTI_RISING_EDGE                0x08U 
#define USB_OTG_FS_WAKEUP_EXTI_FALLING_EDGE               0x0CU 
#define USB_OTG_FS_WAKEUP_EXTI_RISING_FALLING_EDGE        0x10U 

#define USB_OTG_HS_WAKEUP_EXTI_RISING_EDGE                0x08U 
#define USB_OTG_HS_WAKEUP_EXTI_FALLING_EDGE               0x0CU 
#define USB_OTG_HS_WAKEUP_EXTI_RISING_FALLING_EDGE        0x10U 

#define USB_OTG_HS_WAKEUP_EXTI_LINE              0x00100000U  /*!< External interrupt line 20 Connected to the USB HS EXTI Line */
#define USB_OTG_FS_WAKEUP_EXTI_LINE              0x00040000U  /*!< External interrupt line 18 Connected to the USB FS EXTI Line */

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_IT()    EXTI->IMR |= (USB_OTG_HS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_HS_WAKEUP_EXTI_DISABLE_IT()   EXTI->IMR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_HS_WAKEUP_EXTI_GET_FLAG()     EXTI->PR & (USB_OTG_HS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG()   EXTI->PR = (USB_OTG_HS_WAKEUP_EXTI_LINE)

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_RISING_EDGE() do{EXTI->FTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                             EXTI->RTSR |= USB_OTG_HS_WAKEUP_EXTI_LINE;\
                                                            }while(0U)

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_FALLING_EDGE() do{EXTI->FTSR |= (USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                              EXTI->RTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                             }while(0U) 

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_RISING_FALLING_EDGE() do{EXTI->RTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                                     EXTI->FTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                                     EXTI->RTSR |= USB_OTG_HS_WAKEUP_EXTI_LINE;\
                                                                     EXTI->FTSR |= USB_OTG_HS_WAKEUP_EXTI_LINE;\
                                                                    }while(0U)

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_GENERATE_SWIT()   (EXTI->SWIER |= USB_OTG_FS_WAKEUP_EXTI_LINE) 

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_IT()    EXTI->IMR |= USB_OTG_FS_WAKEUP_EXTI_LINE
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_DISABLE_IT()   EXTI->IMR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_GET_FLAG()     EXTI->PR & (USB_OTG_FS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG()   EXTI->PR = USB_OTG_FS_WAKEUP_EXTI_LINE

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_EDGE() do{EXTI->FTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                             EXTI->RTSR |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                            }while(0U)  

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_FALLING_EDGE()  do{EXTI->FTSR |= (USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                               EXTI->RTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                              }while(0U)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_FALLING_EDGE()  do{EXTI->RTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                                      EXTI->FTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                                      EXTI->RTSR |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                                      EXTI->FTSR |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                                     }while(0U) 

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_GENERATE_SWIT()  (EXTI->SWIER |= USB_OTG_FS_WAKEUP_EXTI_LINE)
/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @addtogroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */

/* Initialization/de-initialization functions  ********************************/
/** @addtogroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);
/**
  * @}
  */

/* I/O operation functions  ***************************************************/
/* Non-Blocking mode: Interrupt */
/** @addtogroup PCD_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);
/**
  * @}
  */

/* Peripheral Control functions  **********************************************/
/** @addtogroup PCD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type);
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
uint16_t          HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
/**
  * @}
  */

/* Peripheral State functions  ************************************************/
/** @addtogroup PCD_Exported_Functions_Group4 Peripheral State functions
  * @{
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd);

#endif

