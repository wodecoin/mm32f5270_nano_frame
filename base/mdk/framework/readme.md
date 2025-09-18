一| mm32 平台操作函数
1 adc
2 gpio
3 uart

二、关于rtthread nano

1 移植 rtthread nano 4.1.1 ,使用的是M33内核,使用context_gcc.S 汇编文件

2 基本文件目录树为
├─ core
│  ├─ app
│  │   ├─ main.c
│  │   └─ main.h
│  ├─ bsp
│  │   ├─ mm32f5270_it.c
│  │   ├─ mm32f5270_it.h
│  │   └─ system_mm32f5270.c
│  └─ startup
│      └─ startup_mm32f5270.s
│
├─ library
│  ├─ CMSIS
│  │   └─ Core
│  │       └─ Include
│  │           ├─ cmsis_armclang.h
│  │           ├─ cmsis_compiler.h
│  │           ├─ cmsis_iccarm.h
│  │           ├─ cmsis_version.h
│  │           ├─ core_starmc1.h
│  │           └─ mpu_armv8.h
│  └─ MM32F5270
│      ├─ HAL_Lib
│      │   ├─ Inc
│      │   └─ Src
│      └─ Drivers
│
├─ project
│  └─ keil
│      └─ framework.uvprojx
│
│─ rtthread-nano-4.1.1
│
├─ docs
└─ tools

3 关于rtos的心跳  1ms一次

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

void SysTick_Handler(void)
{
  if (0 != PLATFORM_DelayTick)
  {
    PLATFORM_DelayTick--;
  }
  rt_os_tick_callback();
}

4 关于finsh的移植  注意加入

#define RT_USING_FINSH
#define FINSH_USING_HISTORY 
#define FINSH_USING_SYMTAB

5 如何修改任务栈空间

#define RT_HEAP_SIZE (5*1024)