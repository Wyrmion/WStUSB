/**
  ******************************************************************************
  * @file     MyStUSB.h
  * @author   Kukushkin A.V.
  * @brief    Header file for HAL ST Driver use
  * @version  V1.0.0
  * @date     28 March 2024.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_ST_USB_H__
#define __MY_ST_USB_H__


#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include "InterfacePrivate.h"





typedef struct MyStUSBHandle MyStUSBHandle_t;


// #ifdef My_USB__NO_MALLOC_CNT
//   CircBuf_Handle_t* CircBuf_ctor(InterfaceCmdData_t *payloadarr,uint32_t dept);
// #else
//   CircBuf_Handle_t* CircBuf_ctor(uint32_t dept);
// #endif 

	
MyStUSBHandle_t* 	MyStUSB_ctor(void);
void 	            MyStUSB_dtor(MyStUSBHandle_t* husb);
void 							MyStUsb_RxClbk(MyStUSBHandle_t* husb,uint8_t* data,size_t len);




#ifdef __cplusplus
}
#endif

#endif