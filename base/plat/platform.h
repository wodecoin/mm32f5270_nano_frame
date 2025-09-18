/* Define to prevent recursive inclusion */
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_conf.h"
#include "rtthread.h"
#include <stdio.h>
/**************************** shell color  *************************************/

  /**************************** device enum  *************************************/
  enum MCU_BASE
  {
    UART_IDLE = 0,
    UART_IT,
    IT_NULL
  };

  /********************* IO && Device config ************************************/

#define USE_UART1 1
#define USE_UART2 2
#define USE_UART3 3

#define SHELL_UART UART2
#define SHELL_UART_PORT USE_UART2

#define UART_RX_BUF_SIZE 30
  typedef void (*uart_rx_callback_t)(uint8_t ch);                       // 接收字节回调函数类型
  typedef void (*uart_idle_rx_callback_t)(uint8_t *pBuf, uint16_t len); // 空闲接收回调函数类型
  typedef struct
  {
    UART_TypeDef *uart;

    uint8_t rx_buf[UART_RX_BUF_SIZE]; // 接收缓冲区
    volatile uint16_t rx_len;         // 当前接收长度
    volatile uint8_t rx_ok;           // 接收完成标志（IDLE触发后置1）
    uart_rx_callback_t callback;
    uart_idle_rx_callback_t idle_callback;
  } uart_t;
#ifdef USE_UART1
  extern uart_t uart1;
#endif
#ifdef USE_UART2
  extern uart_t uart2;
#endif
#ifdef USE_UART3
  extern uart_t uart3;
#endif
  typedef struct
  {
    GPIO_TypeDef *port; // GPIO 端口，例如 GPIOA
    uint16_t pin;       // GPIO 引脚，例如 GPIO_Pin_1

    uint32_t freq_hz; // 模拟PWM频率
    uint8_t duty;     // 占空比百分比 0~100

    uint32_t period_ticks; // 一个PWM周期包含多少个tick
    uint32_t on_ticks;     // 高电平持续多少tick
    uint32_t tick_counter; // 当前tick计数
  } soft_pwm_t;

/********************* MCU 相关配置信息 ***************************************/
#define MM32_ROM_START ((uint32_t)0x08000000)
#define MM32_ROM_SIZE (64 * 1024)
#define MM32_ROM_END ((uint32_t)(STM32_ROM_START + STM32_ROM_SIZE))

/********************* 版本说明(尽量使用字符串类型的版本) **********************/
#define HARDWARE_VERSION 1
#define SOFTWARE_VERSION 1
#define HARDWARE_VERSION_STR "V1 2025/5/21"
#define SOFTWARE_VERSION_STR "V1 2025/03/22"

/********************* 更新日志 **********************************************/
#define UPDATE_LOG "更新日志 2025/05/16 新建工程框架\n "
#undef EXTERN

#ifdef _PLATFORM_C_
#define EXTERN
#else
#define EXTERN extern
#endif

  EXTERN volatile uint32_t PLATFORM_DelayTick;
  /* Public platform typedef ********************************************************************************************/

  /* Exported functions *************************************************************************************************/
  void app(void);
	void platform_init(void);
  void IWDG_Configure(uint32_t time_ms);
  void soft_pwm_update(soft_pwm_t *pwm);
  // pwm_config
  void soft_pwm_init(soft_pwm_t *pwm,
                     GPIO_TypeDef *port,
                     uint16_t pin,
                     uint16_t freq_hz,
                     uint8_t duty,
                     uint32_t timer_tick_hz);
  void soft_pwm_set_duty(soft_pwm_t *pwm, uint8_t duty);
  void tim2_pwm_channel_config(uint32_t pwm_freq_hz,
                               uint8_t channel,
                               uint8_t duty_percent,
                               GPIO_TypeDef *gpio_port,
                               uint16_t gpio_pin,
                               uint8_t pin_source,
                               uint8_t af);
  // flash_config
  uint8_t flash_write(const uint8_t *bytes, uint32_t length);
  // usart_config
	void console_init(uint32_t Baudrate);
  void uart_config(UART_TypeDef *UARTx,
                   uint32_t Baudrate,
                   GPIO_TypeDef *GPIOx,
                   uint16_t TxPin,
                   uint16_t RxPin,
                   uint8_t TxPinSource,
                   uint8_t RxPinSource,
                   uint8_t GPIO_AF,
                   uint8_t it_mode,
                   IRQn_Type IRQn);
  void UART_SendBytes(UART_TypeDef *uart, const uint8_t *data, uint32_t length);
  void gpio_config(GPIO_TypeDef *GPIOx, uint16_t pin, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
  void gpio_toggle(GPIO_TypeDef *GPIOx, uint16_t pin);
  // adc_config
  void adc_config_single(ADC_TypeDef *hadc);
  uint16_t adc_read_single(ADC_TypeDef *hadc, uint8_t channel);
  // uilts

#define F(str) (str)

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORM_H_ */

/********************************************** (C) Copyright MindMotion **********************************************/
