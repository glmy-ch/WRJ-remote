# WRJ-Remote · STM32 遥控器

配合 [WRJ-Flight](https://github.com/glmy-ch/WRJ-flight) 四轴飞控使用的 2.4G 遥控器固件：**STM32F103C8T6** + STM32CubeMX + FreeRTOS + Keil MDK，摇杆走 ADC + DMA 采样，按键做微调与飞行模式切换，SI24R1（nRF24L01 兼容）以 6ms 周期发送 17 字节遥控数据包。

## 硬件

| 功能 | 器件 | 引脚 / 接口 |
| --- | --- | --- |
| 主控 | STM32F103C8T6 @72MHz | — |
| 2.4G 无线 | SI24R1 | SPI1：PB3 SCK、PB4 MISO、PB5 MOSI、PA15 NSS，PB7 = `SI_EN` |
| 摇杆（4 通道） | 电位器 | ADC1 + DMA 循环：PA1、PA2、PA3、PA6（通道 1/2/3/6） |
| 按键 | 6 键 | PB2 `KEY_LEFT_X`、PB10 `KEY_RIGHT-X`、PB11 `KEY_UP`、PB12 `KEY_RIGHT`、PB13 `KEY_LEFT`、PB14 `KEY_DOWN` |
| 电源管理 | TP4336 | PB15 `power-key` |
| 调试串口 | USART1 | PA9 TX、PA10 RX |

## 软件结构

```
WRJ-remote.ioc        CubeMX 工程
Core/                 外设初始化：GPIO / ADC1+DMA / SPI1 / USART1
MDK-ARM/APP/          应用层
  APP_freeRTOS.c        任务创建与调度
  APP_process_data.c    摇杆校准、按键处理、数据归一化
  APP_transmit.c        组包并通过 SI24R1 发送
  joystick.c            ADC → 四通道摇杆值
  key.c                 按键扫描与长按识别
  SI24R1.c / tp4336.c
commom/               Com_tool（限幅）等小工具
MDK-ARM/freeRTOS/     FreeRTOS 内核与移植层
```

### 任务划分

| 任务 | 优先级 | 周期 | 职责 |
| --- | --- | --- | --- |
| `task_power_key` | 4 | 10 s | TP4336 电源维持 |
| `task_SI24R1` | 3 | 6 ms | 组包 + 无线发送 |
| `task_key` | 2 | 20 ms | 按键扫描，映射为微调 / 模式切换 |
| `task_joystick` | 2 | 20 ms | ADC 采样 + 归一化 |

### 数据流

1. ADC（0–4095）→ 换算成 **0–1000**：`1000 - raw * 1000 / 4095`。
2. 减去上电校准得到的零位偏移（`joystick_calibrate()` 采样 10 次取平均），再加上按键微调量（每次 ±10）。
3. 限幅到 0–1000 后写入 `remote.pit / rol / thr / yaw`。
4. `APP_transmit_Data()` 把 4 个通道 + 关机 / 定高标志打包成 17 字节，经 SI24R1 发出。

### 按键功能

| 按键 | 功能 |
| --- | --- |
| UP / DOWN | 俯仰微调 ∓10 |
| LEFT / RIGHT | 横滚微调 ∓10 |
| LEFT_X | 发送关机指令（飞控收到后断电） |
| RIGHT_X 短按 | 切换定高模式标志 |
| RIGHT_X 长按 | 重新校准摇杆零位 |

## 编译

1. Keil MDK 打开 `MDK-ARM/WRJ-remote.uvprojx` 编译烧录。
2. 改外设用 STM32CubeMX 打开 `WRJ-remote.ioc` 重新生成。

## 已知限制 / TODO

- `task_power_key` 里写的是局部函数声明 `void tp4336_power_key(void);`，实际没有调用，电源维持在真机上需确认。
- 摇杆校准是累加式（多次长按会累积偏移），实际使用建议只在冷启动时校准一次。
- 遥控数据没有做校验和 / 应答确认，丢包时飞控侧按超时进入保护逻辑。
