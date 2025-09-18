/**
 * @file mm32_uart.c
 * @author yang
 * @brief
 * @version 0.1
 * @date 2025-09-18
 *
 * @copyright Copyright (c) 2025
 *
 *  @note 1 串口配置；2 串口发送；3 串口普通中断、空闲中断； 4 配置中断回调函数
 */

#include "platform.h"

uart_t uart1 = {
    .uart = UART1,
    .rx_len = 0,
    .rx_ok = 0,
    .callback = NULL};

uart_t uart2 = {
    .uart = UART2,
    .rx_len = 0,
    .rx_ok = 0,
};

uart_t uart3 = {
    .uart = UART3,
    .rx_len = 0,
    .rx_ok = 0,
};

/**
 * @brief  UART 初始化函数
 * @param  UARTx: 指向要配置的 UART 外设的指针
 * @param  Baudrate: 波特率
 * @param  GPIOx: 指向要配置的 GPIO 外设的指针
 * @param  TxPin: TX 引脚
 * @param  RxPin: RX 引脚
 * @param  TxPinSource: TX 引脚复用源
 * @param  RxPinSource: RX 引脚复用源
 * @param  GPIO_AF: GPIO 复用功能 -- 查数据手册
 * @param  IRQn: 中断向量号
 * @retval None
 */
void uart_config(UART_TypeDef *UARTx,
                 uint32_t Baudrate,
                 GPIO_TypeDef *GPIOx,
                 uint16_t TxPin,
                 uint16_t RxPin,
                 uint8_t TxPinSource,
                 uint8_t RxPinSource,
                 uint8_t GPIO_AF,
                 uint8_t it_mode,
                 IRQn_Type IRQn)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    UART_InitTypeDef UART_InitStruct;

    // 1. 启用 UART 和 GPIO 时钟
    if (UARTx == UART1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    else if (UARTx == UART2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE);
    else if (UARTx == UART3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART3, ENABLE);

    if (GPIOx == GPIOA)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    else if (GPIOx == GPIOC)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    // 2. 配置 UART 参数
    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.BaudRate = Baudrate;
    UART_InitStruct.WordLength = UART_WordLength_8b;
    UART_InitStruct.StopBits = UART_StopBits_1;
    UART_InitStruct.Parity = UART_Parity_No;
    UART_InitStruct.HWFlowControl = UART_HWFlowControl_None;
    UART_InitStruct.Mode = UART_Mode_Rx | UART_Mode_Tx;
    UART_Init(UARTx, &UART_InitStruct);

    // 3. 配置 GPIO 复用功能
    GPIO_PinAFConfig(GPIOx, TxPinSource, GPIO_AF);
    GPIO_PinAFConfig(GPIOx, RxPinSource, GPIO_AF);

    // 配置 TX 引脚：复用推挽输出
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = TxPin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOx, &GPIO_InitStruct);

    // 配置 RX 引脚：输入上拉
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = RxPin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOx, &GPIO_InitStruct);
    // 配置 UART 中断/空闲中断
    if (it_mode != IT_NULL)
    {
        UART_ITConfig(UARTx, UART_IT_RX, ENABLE);
        if (it_mode == UART_IDLE)
            UARTx->IER |= UART_IT_RXIDLE;
        // 4. 配置中断
        NVIC_InitStruct.NVIC_IRQChannel = IRQn;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
    }
    // 5. 启用 UART
    UART_Cmd(UARTx, ENABLE);
}

void UART_SendBytes(UART_TypeDef *uart, const uint8_t *data, uint32_t length)
{
    uint32_t i = 0;
    for (i = 0; i < length; i++)
    {
        while (RESET == UART_GetFlagStatus(uart, UART_FLAG_TXEPT))
            ;
        UART_SendData(uart, data[i]);
    }
}

void UART1_IRQHandler(void)
{
#if defined(USE_UART1)
    uart_t *huart = &uart1;

    // 接收中断
    if (UART_GetITStatus(huart->uart, UART_IT_RX) != RESET)
    {
        if (huart->callback)
        {
            huart->callback(UART_ReceiveData(huart->uart));
        }
        UART_ClearITPendingBit(huart->uart, UART_IT_RX);
    }

    // 空闲中断：一帧接收完成
    if (huart->uart->ISR & UART_IT_RXIDLE)
    {
        huart->uart->ICR = UART_IT_RXIDLE;
        huart->rx_ok = 1;
    }
#endif
}

/**
 * @brief  UART2 中断处理函数
 * @note   我把it.c 的中断函数 设为了弱函数
 * @note   用普通中断逐个缓存数据，用空闲中断判断“接收完成”并触发处理
 */
void UART2_IRQHandler(void)
{
#if defined(USE_UART2)
    uart_t *huart = &uart2;

    // 接收中断
    if (UART_GetITStatus(huart->uart, UART_IT_RX) != RESET)
    {
        if (huart->callback)
        {
            huart->callback(UART_ReceiveData(huart->uart));
        }
        UART_ClearITPendingBit(huart->uart, UART_IT_RX);
    }

    // 空闲中断：一帧接收完成
    if (huart->uart->ISR & UART_IT_RXIDLE)
    {
        huart->uart->ICR = UART_IT_RXIDLE;
        huart->rx_ok = 1;
    }
#endif
}

void UART3_IRQHandler(void)
{
#if defined(USE_UART3)
    uart_t *huart = &uart3;

    // 接收中断
    if (UART_GetITStatus(huart->uart, UART_IT_RX) != RESET)
    {
        if (huart->callback)
        {
            huart->callback(UART_ReceiveData(huart->uart));
        }
        UART_ClearITPendingBit(huart->uart, UART_IT_RX);
    }

    // 空闲中断：一帧接收完成
    if (huart->uart->ISR & UART_IT_RXIDLE)
    {
        huart->uart->ICR = UART_IT_RXIDLE;
        huart->rx_ok = 1;
    }
#endif
}

/* use demo
UART_Configure(UART2,
               115200,
               GPIOA,
               GPIO_Pin_2, GPIO_Pin_3,
               GPIO_PinSource2, GPIO_PinSource3,
               GPIO_AF_1,
               UART2_IRQn);
*/
// static struct rt_messagequeue mq;
// void shell_rx_callback(uint8_t ch)
// {
//     rt_mq_send(&mq, &ch, 1);
// }

// #define MSG_QUEUE_SIZE 32
// int uart_shell_module_init(void)
// {
//     shell_init();

//     static char mq_pool[MSG_QUEUE_SIZE];

//     rt_mq_init(&mq, "mq_uart",
//                mq_pool, 1, MSG_QUEUE_SIZE, RT_IPC_FLAG_FIFO);
// #if (SHELL_UART_PORT == USE_UART1)
//     uart1.callback = shell_rx_callback;
// #elif (SHELL_UART_PORT == USE_UART2)
//     uart2.callback = shell_rx_callback;
// #endif
//     return 0;
// }
// INIT_APP_EXPORT(uart_shell_module_init);