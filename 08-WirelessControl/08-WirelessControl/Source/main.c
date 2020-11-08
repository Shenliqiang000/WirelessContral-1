#include <iocc2530.h>
#include "hal_mcu.h"
#include "hal_assert.h"
#include "hal_board.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include <stdio.h>
#include "info.h"
#include "uart1.h"
#include "clock.h"
#include "htu21d.h"
#include "iic.h"

#define RF_CHANNEL            19                                // 2.4 GHz RF channel
#define PAN_ID                0x2002
#define SEND_ADDR             0x2530
#define RECV_ADDR             0x2522
#define COMMAND               0x10

#define NODE_TYPE             1                                 //0:接收节点，！0：发送节点

char buffer='1';

void io_init(void)
{
  P2SEL &= ~0x01;
  P2DIR &= ~0x01;
}

static basicRfCfg_t basicRfConfig;
int ledstatus = 0;
uint8 pRxData[128];

void rfSendData(void)
{
  
  int rlen;
  uint8 pTxData[] = 'a'; 
  char tx[]="请输入g获取温度......\n"; 
  int i;
  
  basicRfReceiveOff();
  while (TRUE) {
    
    hal_led_on(1);
    UartSendString(tx,sizeof tx);
    halMcuWaitMs(500);
    if(buffer=='1')
    {
      buffer= uart1_recv_char();
    }
    pTxData[0]= buffer;
    basicRfSendPacket(0xffff, (uint8*)pTxData, sizeof pTxData);//广播发送数据包   
     
    halMcuWaitMs(500);
    basicRfReceiveOn();

    halMcuWaitMs(500);
    while(!basicRfPacketIsReady());
    rlen =basicRfReceive(pRxData, sizeof pRxData, NULL);
   
    if(rlen>0)
    {
      hal_led_off(1);
      uart0_send_string(pRxData);
    }
     
    basicRfReceiveOff();
    halMcuWaitMs(500);
    
  }
}

void rfRecvData(void)
{
  uint8 pRxData[128];
  uint8 txData[128]='1';
  int rlen;

  while (TRUE) {
    basicRfReceiveOn();
    while(!basicRfPacketIsReady());
    rlen = basicRfReceive(pRxData, sizeof pRxData, NULL);
    
    if(rlen > 0 && pRxData[0] == 'g')
    {
      basicRfReceiveOff();
      hal_led_on(1);   
      
      sprintf(txData,"Temp:%.2f  sa:2",htu21d_get_data(TEMPERATURE)/100.0);     //将温度数据以字符串的形式回传到汇集节点
      printf("Temp:");
      printf(txData);
      printf("°");
      
      halMcuWaitMs(500);
      hal_led_off(1);
      halMcuWaitMs(2000);       //如果是node1，注释此行，如果为node2则不用
      halMcuWaitMs(500);
      
      basicRfSendPacket(0xffff, (uint8*)txData, sizeof txData);         //广播发送温度数据，只有NODE_TYPE=1时才能收到
      halMcuWaitMs(500);     
      
      basicRfReceiveOn();
    }
  }
}

void main(void)
{
    halMcuInit();

    io_init();
    uart1_init();
    hal_led_init();
    iic_init();
    htu21d_init();
    
    hal_uart_init();
    
    
    //lcd_dis();
    
    if (FAILED == halRfInit()) {
        HAL_ASSERT(FALSE);
    }
    // Config basicRF
    basicRfConfig.panId = PAN_ID;
    basicRfConfig.channel = RF_CHANNEL;
    basicRfConfig.ackRequest = TRUE;
#ifdef SECURITY_CCM
    basicRfConfig.securityKey = key;
#endif

    
    // Initialize BasicRF
#if NODE_TYPE
    basicRfConfig.myAddr = SEND_ADDR;
#else
    basicRfConfig.myAddr = RECV_ADDR; 
#endif
    
    if(basicRfInit(&basicRfConfig)==FAILED) {
      HAL_ASSERT(FALSE);
    }
#if NODE_TYPE
  rfSendData();
#else
  rfRecvData();   
#endif
}


#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
  URX0IF = 0;       //清中断标志
  //temp = U0DBUF;    //将串口接收缓冲的数据存放到temp中,temp是char类型，即8位数据位
  
}
