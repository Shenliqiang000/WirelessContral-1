
#include <iocc2530.h>
#include <stdio.h>


void hal_led_init(void)
{
    P1SEL &= ~0x0D;          //设置P1.0 P1.2 P1.3为普通 I/O 口
    P1DIR |= 0x0D;           //设置P1.0 P1.2 P1.3 为输出

    P2SEL &= ~0x01;
    P2DIR |= 0x01;
    
    P1 |= 0x0D;
    P2_0 |= 0x01;
}

void hal_led_off(int leds)
{
  if (leds & 0x01) {
    P1_0 |= 0x01;
  }
  if (leds & 0x02) {
    P1_2 |= 0x04;
  }
  if (leds & 0x04) {
    P1_3 |= 0x08;
  }
  if (leds & 0x08) {
    P2_0 |= 0x01;
  }
}
void hal_led_on(int leds)
{
    if (leds & 0x01) {
    P1_0 &= ~0x01;
  }
  if (leds & 0x02) {
    P1_2 &= ~0x04;
  }
  if (leds & 0x04) {
    P1_3 &= ~0x08;
  }
  if (leds & 0x08) {
    P2_0 &= ~0x01;
  }
}



/****************************************************************
*函数功能 ：初始化串口1										
*入口参数 ：无												
*返 回 值 ：无							
*说    明 ：19200-8-n-1						
****************************************************************/
void hal_uart_init(void)
{
  P1SEL |=  0xC0;                                               //初始化UART1端口
  PERCFG |= 0x02;                                               //选择UART1为可选位置一
  P2SEL &= ~0x20;                                               //P1优先作为串口1
  U1CSR = 0xC0;                                                 //设置为UART模式,而且使能接受器
   
  U1GCR = 10;                  
  U1BAUD = 59;                                                  //波特率设置为38400
  
  U1UCR |= 0|0;                                     //设置停止位与奇偶校验
}

__near_func int putchar(int ch)
{
    U1DBUF = ch;
    while(UTX1IF == 0);
    UTX1IF = 0;
    return ch;
}




