/**
  ******************************************************************************
  * @file     MyStUSB.h
  * @author   Kukushkin A.V.
  * @brief    Header file for HAL ST Driver use
  * @version  V1.0.1
  * @date     15 Oct 2026.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_ST_USB_H__
#define __MY_ST_USB_H__


#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stddef.h>





typedef struct MyStUSBHandle MyStUSBHandle_t;



	
MyStUSBHandle_t* 	MyStUSB_ctor(void* ctx);
void 	            MyStUSB_dtor(MyStUSBHandle_t* husb);

void 							MyStUsb_RxClbk(MyStUSBHandle_t* husb,uint8_t* data,size_t len);
void 							MyStUsb_TxClbk(MyStUSBHandle_t* husb);




#ifdef __cplusplus
}
#endif

#endif
