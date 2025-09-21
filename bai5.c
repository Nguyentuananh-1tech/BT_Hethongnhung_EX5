#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <string.h>

#define LED_PIN         GPIO_Pin_0
#define LED_PORT        GPIOA

#define USART2_TX_PIN   GPIO_Pin_2
#define USART2_RX_PIN   GPIO_Pin_3
#define USART2_PORT     GPIOA

void GPIO_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = LED_PIN;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &gpio);

    GPIO_ResetBits(LED_PORT, LED_PIN);  
}


void USART2_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitTypeDef gpio;

   
    gpio.GPIO_Pin = USART2_TX_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART2_PORT, &gpio);

    
    gpio.GPIO_Pin = USART2_RX_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART2_PORT, &gpio);

    USART_InitTypeDef usart;
    usart.USART_BaudRate = 9600;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    USART_Cmd(USART2, ENABLE);
}


void USART2_SendChar(char c) {
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, (uint16_t)c);
}


void USART2_SendString(char *str) {
    while (*str) USART2_SendChar(*str++);
}


char USART2_ReadChar(void) {
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART2);
}


int main(void) {
    char buffer[10];
    int index = 0;

    GPIO_Config();
    USART2_Init();

    USART2_SendString("Hello from STM32!\r\n");

    while (1) {
        char c = USART2_ReadChar();   
        USART2_SendChar(c);           

        if (c == '\r' || c == '\n') { 
            buffer[index] = '\0';    

            if (strcmp(buffer, "ON") == 0) {
                GPIO_SetBits(LED_PORT, LED_PIN);
                USART2_SendString("\r\nLED ON\r\n");
            }
            else if (strcmp(buffer, "OFF") == 0) {
                GPIO_ResetBits(LED_PORT, LED_PIN);
                USART2_SendString("\r\nLED OFF\r\n");
            }
            index = 0; 
        } else {
            buffer[index++] = c;
            if (index >= (int)sizeof(buffer)) index = 0; 
        }
    }
}
