 /**
 ****************************************************************************
 * @file     MyStUSB.c
 * @author   Kukushkin A.V.
 * @brief    this file provide code for stm32 usb CDC shell 
 * @version  V1.0.1
 * @date     15 Oct 2026.
 *************************************************************************
 */
 
#include "MyStUSB.h"
#include "usbd_cdc_if.h"

#include "wheap.h"
#include "InterfacePrivate.h"

#include "stdbool.h"
//#define MYUSB_W8_FREE


#ifdef W_USE_RTOS
  #include "cmsis_os2.h"
#endif

struct MyStUSBHandle
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
static MyStUSBHandle_t hdev;
#endif

static bool ReadRxBuff( void* this_ptr,uint8_t* data,   size_t* len,size_t  max_len);
static void SetRxBuff(  void* this_ptr,uint8_t* RxBuff, size_t  max_len);
static void SetTxBuff(  void* this_ptr,uint8_t* RxBuff, size_t  max_len);
static bool SendData(   void* this_ptr,uint8_t* data,size_t len);
static void Process(    void* this_ptr); 
static bool IsFree(     void* this_ptr);

static void ResetDrdy(MyStUSBHandle_t* husb);

static HwInterface_vtable_t vtable = {
    .ReadRxBuff = ReadRxBuff,
    .SetRxBuff = SetRxBuff,
    .SetTxBuff = SetTxBuff,
    .SendData = SendData,
    .IsFree = IsFree,
    .Process = Process,
};

static void SetRxBuff(void* this_ptr,uint8_t* /*data*/RxBuff,size_t /*max len*/max_len)
{
  MyStUSBHandle_t* husb = (MyStUSBHandle_t*)this_ptr;

  husb->RxBuff = RxBuff;
  husb->RxBuff_len = max_len;
}
static void SetTxBuff(void* this_ptr,uint8_t* /*data*/TxBuff,size_t /*max len*/max_len)
{
  MyStUSBHandle_t* husb = (MyStUSBHandle_t*)this_ptr;

  husb->TxBuff = TxBuff;
  husb->TxBuff_len = max_len;
}

static bool ReadRxBuff(void* this_ptr,uint8_t* data,size_t* len,size_t max_len)
{
  MyStUSBHandle_t* husb = (MyStUSBHandle_t*)this_ptr;
  
  if(husb->drdy == true)
  {
    ResetDrdy(husb);
    *len = husb->RxLeng;
    return true;
  }
  return false;
}

static void ResetDrdy(MyStUSBHandle_t* husb)
{
  __disable_irq();
    husb->drdy = false;
  __enable_irq();

  
}

static bool SendData(void* this_ptr,uint8_t* data,size_t len)
{
  MyStUSBHandle_t* cthis = (MyStUSBHandle_t*)this_ptr;
  
  #ifdef MY_ST_USB_FS
    if(CDC_Transmit_FS(data,len) == USBD_BUSY) return false;
  #else 
    if(CDC_Transmit_HS(data,len) == USBD_BUSY) return false;
  #endif
  return true;
}

static bool IsFree(void* this_ptr)
{
  return CDC_IsFree();
}

void MyStUsb_RxClbk(MyStUSBHandle_t* husb,uint8_t* data,size_t len)
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

void MyStUsb_TxClbk(MyStUSBHandle_t* husb)
{
  #ifdef W_USE_RTOS
    osThreadFlagsSet(husb->ctx,0b111);
  #endif
}

static void Process(void* this_ptr)
{ 
  UNUSED(this_ptr);
}

MyStUSBHandle_t* MyStUSB_ctor(void* ctx)
{
  MyStUSBHandle_t *phUsb = NULL;

  
  phUsb = heap_malloc_cast(MyStUSBHandle_t);
  
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

void MyStUSB_dtor(MyStUSBHandle_t *husb)
{
  #ifndef NO_MY_MALLOC
    free(husb);
  #endif

  husb = NULL;
}
