 /**
 ****************************************************************************
 * @file     WStUSB.c
 * @author   Wyrm
 * @brief    this file provide code for stm32 usb CDC shell 
 * @version  V1.0.4
 * @date     08 Nov 2026
 * @note     plz correct usbd_cdc_if.h:
 *
 *           USER CODE BEGIN INCLUDE 
 *           #include "WStUSB.h"
 *           USER CODE END INCLUDE 
 *
 *           USER CODE BEGIN EXPORTED_VARIABLES 
 *           extern WStUSBHandle_t* husb;
 *           USER CODE END EXPORTED_VARIABLES 
 *        
 *           USER CODE BEGIN EXPORTED_FUNCTIONS 
 *           bool CDC_IsFree(void);
 *           USER CODE END EXPORTED_FUNCTIONS 
 *
 *           usbd_cdc_if.c:
 *            
 *           static int8_t CDC_Receive_HS(uint8_t* Buf, uint32_t *Len);
 *
 *           replace user block 11
 *
 *           USER CODE BEGIN 11 
 *            //USBD_CDC_SetRxBuffer(&hUsbDeviceHS, &Buf[0]);
 *            WStUSB_RxClbk(husb,UserRxBufferHS,*Len);
 *            USBD_CDC_ReceivePacket(&hUsbDeviceHS);
 *            return (USBD_OK);
 *           USER CODE END 11 
 *           
 *           uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len)
 *                   
 *           replace user block 12
 *           
 *           USER CODE BEGIN 12
 *           USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceHS.pClassData;
 *           if (hcdc->TxState != 0){
 *             return USBD_BUSY;
 *           }
 *           USBD_CDC_SetTxBuffer(&hUsbDeviceHS, Buf, Len);
 *           result = USBD_CDC_TransmitPacket(&hUsbDeviceHS);
 *           USER CODE END 12 
 *
 *
 *
 *           USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION 
 *           bool CDC_IsFree(void)
 *           {
 *             USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceHS.pClassData;
 *             if (hcdc->TxState != 0) 
 *               return false;
 *           
 *             return true;
 *           }
 *           USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION 
 *************************************************************************
 */
 
#include "WStUSB.h"

#include "usbd_cdc_if.h"

#include "wheap.h"
#include "InterfacePrivate.h"


//#define MYUSB_W8_FREE


#ifdef W_USE_RTOS
  #include "cmsis_os2.h"
#endif

struct WStUSBHandle
{
  HWInterface_t parent;
  uint8_t*  RxBuff;
  size_t    RxBuff_len;

  uint8_t*  TxBuff;
  size_t    TxBuff_len;

  size_t    RxLeng;
  bool      drdy;

  void*     ctx;
};
#ifdef NO_MY_MALLOC
static WStUSBHandle_t hdev;
#endif

static bool ReadRxBuff( void* this_ptr,uint8_t* data,   size_t* len,size_t  max_len);
static void SetRxBuff(  void* this_ptr,uint8_t* RxBuff, size_t  max_len);
static void SetTxBuff(  void* this_ptr,uint8_t* RxBuff, size_t  max_len);
static bool SendData(   void* this_ptr,const uint8_t* data,size_t len);
static void Process(    void* this_ptr); 
static bool IsFree(     void* this_ptr);

static void ResetDrdy(WStUSBHandle_t* husb);

static const HwInterface_vtable_t vtable = {
    .ReadRxBuff = ReadRxBuff,
    .SetRxBuff = SetRxBuff,
    .SetTxBuff = SetTxBuff,
    .SendData = SendData,
    .IsFree = IsFree,
    .Process = Process,
};

static void SetRxBuff(void* this_ptr,uint8_t* /*data*/RxBuff,size_t /*max len*/max_len)
{
  WStUSBHandle_t* husb = (WStUSBHandle_t*)this_ptr;

  husb->RxBuff = RxBuff;
  husb->RxBuff_len = max_len;
}
static void SetTxBuff(void* this_ptr,uint8_t* /*data*/TxBuff,size_t /*max len*/max_len)
{
  WStUSBHandle_t* husb = (WStUSBHandle_t*)this_ptr;

  husb->TxBuff = TxBuff;
  husb->TxBuff_len = max_len;
}

static bool ReadRxBuff(void* this_ptr,uint8_t* data,size_t* len,size_t max_len)
{
  WStUSBHandle_t* husb = (WStUSBHandle_t*)this_ptr;
  
  if(husb->drdy == true)
  {
    ResetDrdy(husb);
    *len = husb->RxLeng;
    return true;
  }
  return false;
}

static void ResetDrdy(WStUSBHandle_t* husb)
{
  __disable_irq();
    husb->drdy = false;
  __enable_irq();

  
}

static bool SendData(void* this_ptr,const uint8_t* data,size_t len)
{
  [[maybe_unused]]WStUSBHandle_t* cthis = (WStUSBHandle_t*)this_ptr;
  
  #ifdef MY_ST_USB_FS
    if(CDC_Transmit_FS(data,len) == USBD_BUSY) return false;
  #else 
    if(CDC_Transmit_HS((uint8_t*)data,len) == USBD_BUSY) return false;
  #endif
  return true;
}

static bool IsFree(void* this_ptr)
{
  return CDC_IsFree();
}

void WStUSB_RxClbk(WStUSBHandle_t* husb,uint8_t* data,size_t len)
{
  if(husb == NULL) 
    return;

  if(len>husb->RxBuff_len) 
    while(1);//bug catch;
  else 
    memcpy(husb->RxBuff,data,len);
  
  husb->RxLeng = len;
  husb->drdy = true;

  #ifdef W_USE_RTOS
    osThreadFlagsSet(husb->ctx,0b111);
  #endif
}

void WStUSB_TxClbk(WStUSBHandle_t* husb)
{
  #ifdef W_USE_RTOS
    osThreadFlagsSet(husb->ctx,0b111);
  #endif
}

static void Process(void* this_ptr)
{ 
  UNUSED(this_ptr);
}

WStUSBHandle_t* WStUSB_ctor(void* ctx)
{
  WStUSBHandle_t *phUsb = NULL;

  
  phUsb = heap_malloc_cast(WStUSBHandle_t);
  
  if(phUsb == NULL) return NULL;

  phUsb->parent.vtable = &vtable;
  phUsb->drdy   = false;
  
  phUsb->RxBuff = NULL;
  phUsb->RxBuff_len = 0;

  phUsb->TxBuff = NULL;
  phUsb->TxBuff_len = 0;
  
  phUsb->ctx = ctx;

  return phUsb;
}

void WStUSB_dtor(WStUSBHandle_t *husb)
{
  #ifndef NO_MY_MALLOC
    free(husb);
  #endif

  husb = NULL;
}
