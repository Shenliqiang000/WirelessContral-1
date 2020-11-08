#include "uart1.h"

unsigned int recvCnt;
char recvBuf[];

void uart1_init(void){

  
  PERCFG = 0x00;          //设置UART0的备用位置为P0口，即UART0的RX-P0_2，TX-P0_3
  P0SEL  = 0x3c;          //设置P0_2，P0_3为外设功能
  P2DIR &= ~0xc0;         //配置P0端口优先作为UART0
  
  /*设置USART0的方式*/
  U0CSR |= 0x80;          //SPI同步模式  
  
  /*设置USART0的波特率，115200*/
  U0GCR |= 11;            
  U0BAUD |= 216;
  
  /*将UART0中断标志位清0*/
  UTX0IF = 0;             //清除UART0发送中断标志位
  URX0IF = 0;             //清除UART0接收中断标志位
  
  /*开总中断，以及使能串口0接收中断*/
  IEN0 |= 0x80;           //开总中断
  IEN0 |= 0x04;           //使能串口0接收中断
  
  /*运行串口0接收*/
  U0CSR |= 0x40;

}

void uart0_send_char(char ch){
  U0DBUF = ch;
  while(UTX0IF == 0);
  UTX0IF = 0;
}
void uart0_send_string(char *data){
  while(*data != '\0'){
    uart0_send_char(*data++);
  }
}
void UartSendString(char *Data, int len)
{
  int j;
  for (j = 0; j < len; j++)
  {
    U0DBUF = *Data++;
    for (; 0 == UTX0IF ;);
    UTX0IF = 0;
  }
}
char uart1_recv_char(){
  char ch;
  while(URX0IF == 0);
  ch = U0DBUF;
  URX0IF = 0;
  return ch;
}
