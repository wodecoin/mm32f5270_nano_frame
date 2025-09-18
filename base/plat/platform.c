
/* Define to prevent recursive inclusion */
#define _PLATFORM_C_

/* Files include */

#include "platform.h"

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

void systick_init(void)
{
    RCC_ClocksTypeDef  RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
    {
        while (1)
        {
        }
    }

  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void delay_ms(uint32_t Millisecond)
{
  PLATFORM_DelayTick = Millisecond;

  while (0 != PLATFORM_DelayTick)
  {
  }
}

void console_init(uint32_t Baudrate)
{
#if (SHELL_UART_PORT == USE_UART1)
  uart_config(SHELL_UART,
              Baudrate,
              GPIOA,
              GPIO_Pin_9, GPIO_Pin_10,
              GPIO_PinSource9, GPIO_PinSource10,
              GPIO_AF_1,
              UART_IT,
              UART1_IRQn);
#elif (SHELL_UART_PORT == USE_UART2)
  uart_config(SHELL_UART,
              Baudrate,
              GPIOA,
              GPIO_Pin_2, GPIO_Pin_3,
              GPIO_PinSource2, GPIO_PinSource3,
              GPIO_AF_1,
              UART_IT,
              UART2_IRQn);
#else
#error "Unsupported SHELL_UART_PORT definition"
#endif
}

int fputc(int ch, FILE *f)
{
  UART_SendData(SHELL_UART, (uint8_t)ch);

  while (RESET == UART_GetFlagStatus(SHELL_UART, UART_FLAG_TXEPT))
  {
  }

  return (ch);
}

void platform_info_print(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  RCC_GetClocksFreq(&RCC_Clocks);

  printf("\r\n");
  printf("\r\nSYSCLK Frequency : %7.3f MHz", (double)RCC_Clocks.SYSCLK_Frequency / (double)1000000.0);
  printf("\r\nHCLK   Frequency : %7.3f MHz", (double)RCC_Clocks.HCLK_Frequency / (double)1000000.0);
  printf("\r\nPCLK1  Frequency : %7.3f MHz", (double)RCC_Clocks.PCLK1_Frequency / (double)1000000.0);
  printf("\r\nPCLK2  Frequency : %7.3f MHz", (double)RCC_Clocks.PCLK2_Frequency / (double)1000000.0);
  printf("\r\n\r\n");

  printf("\r\n--------------------------------------------\r\n");
  printf("software--> %d  or  %s\r\n", SOFTWARE_VERSION, SOFTWARE_VERSION_STR);
  printf("hardware--> %d  or  %s\r\n", HARDWARE_VERSION, HARDWARE_VERSION_STR);
  printf("\n%s\n", UPDATE_LOG);
  printf("\r\n--------------------------------------------\r\n");

  if (SET == RCC_GetFlagStatus(RCC_FLAG_PINRST))
  {
    printf("PIN Reset Flag.\r\n");
  }

  if (SET == RCC_GetFlagStatus(RCC_FLAG_PORRST))
  {
    printf("POR/PDR Reset Flag.\r\n");
  }

  if (SET == RCC_GetFlagStatus(RCC_FLAG_SFTRST))
  {
    printf("Software Reset Flag.\r\n");
  }

  if (SET == RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
  {
    printf("Independent Watchdog Reset Flag.\r\n");
  }

  if (SET == RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
  {
    printf("Window Watchdog Reset Flag.\r\n");
  }

  RCC_ClearFlag();
}
void platform_init(void)
{
  systick_init();
  platform_info_print();

  //	IWDG_Configure(10000);
}


// void IWDG_Configure(uint32_t time_ms)
// {
//   if (time_ms > 3000)
//     time_ms = 3000; //
//   uint16_t Reload = LSI_VALUE / 1000 * time_ms / 32;

//   RCC_APB1PeriphClockCmd(RCC_APB1ENR_IWDG, ENABLE);

//   RCC_LSICmd(ENABLE);

//   while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSIRDY))
//   {
//   }

//   PVU_CheckStatus();
//   IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//   IWDG_SetPrescaler(IWDG_Prescaler_32);
//   PVU_CheckStatus();

//   RVU_CheckStatus();
//   IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//   IWDG_SetReload(Reload);
//   RVU_CheckStatus();

//   IWDG_ReloadCounter();

//   IWDG_Enable();
// }
