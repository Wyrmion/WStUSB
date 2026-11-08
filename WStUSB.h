/**
  ******************************************************************************
  * @file     WStUSB.h
  * @author   Kukushkin A.V.
  * @brief    Header file for HAL ST Driver use
  * @version  V1.0.4
  * @date     08 Nov 2026
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W_ST_USB_H__
#define __W_ST_USB_H__


#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stddef.h>





typedef struct    WStUSBHandle WStUSBHandle_t;



	
WStUSBHandle_t* 	WStUSB_ctor(void* ctx);
void 	            WStUSB_dtor(WStUSBHandle_t* husb);

void 							WStUSB_RxClbk(WStUSBHandle_t* husb,uint8_t* data,size_t len);
void 							WStUSB_TxClbk(WStUSBHandle_t* husb);




#ifdef __cplusplus
}
#endif

#endif
