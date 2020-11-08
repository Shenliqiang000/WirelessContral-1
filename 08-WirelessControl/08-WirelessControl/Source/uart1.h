#include <ioCC2530.h>
#include <stdio.h>




void uart1_init(void);
void uart1_send_char(char ch);
void UartSendString(char *Data, int len);
char uart1_recv_char();
void teach_uart1_talking(void);