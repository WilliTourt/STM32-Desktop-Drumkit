# STM32 Desktop Drumkit - 软件/固件/API 文档

## 系统概述

STM32 Desktop Drumkit是一个将鼓垫敲击转换为 MIDI 消息的控制器，具有以下特点：

- 10 个带力度感应的鼓垫输入
- OLED 显示屏用于状态显示和配置
- USB MIDI 输出
- 可配置的鼓垫设置

## 硬件接口

### 引脚配置

MIDI 音符配置可以在 midi.h 中轻松修改。详见[调试与参数标定](Docs/howtodebug-zh-CN.md)。

| 鼓垫名称    | ADC 组 | ADC 通道 | 输出端口/引脚 | MIDI 音符 |
|-----------|--------|----------|-------------|----------|
| 开踩镲     | ADC1   | 0        | GPIOA/P4    | 46       |
| 闭踩镲     | ADC1   | 1        | GPIOA/P5    | 42       |
| 吊镲       | ADC1   | 2        | GPIOA/P6    | 49       |
| 叮叮镲     | ADC1   | 3        | GPIOA/P7    | 51       |
| 边击       | ADC2   | 0        | GPIOC/P0    | 37       |
| 底鼓       | ADC2   | 1        | GPIOC/P1    | 35       |
| 军鼓       | ADC2   | 2        | GPIOC/P2    | 38       |
| 中通鼓     | ADC3   | 0        | GPIOA/P0    | 48       |
| 低通鼓     | ADC3   | 1        | GPIOA/P1    | 45       |
| 高通鼓     | ADC3   | 2        | GPIOC/P3    | 50       |

### 外设使用

- **ADC1-3**: 鼓垫输入检测(使用 DMA)
- **I2C1**: OLED 显示屏通信
- **USART1**: 调试输出
- **USART2**: MIDI 输出
- **GPIO**: 鼓垫输出触发、按钮、LED

## 软件架构

本项目采用模块化设计，使用 C++ 而非 C 以便更好地处理多个类实例。主入口点为 `cpp_main.cpp`。
本项目使用VSCode + EIDE插件编写，如果你也使用EIDE，可以直接导入workspace。

### 主要组件

1. **Pad 类** (`pad.h/cpp`)
   - 处理鼓垫输入
   - 敲击检测和力度测量
   - 力度映射曲线

2. **UI 类** (`ui.h/cpp`)
   - OLED 显示管理
   - 菜单导航
   - 按钮输入处理

3. **Midi 类** (`midi.h/cpp`)
   - MIDI 消息构造
   - Note On/Off 处理
   - 通道状态管理

4. **主应用** (`cpp_main.cpp`)
   - 系统初始化
   - 主处理循环
   - 模块协调

### 数据流

1. 通过 ADC/DMA 检测鼓垫敲击
2. 测量力度值并映射为速度
3. 发送 MIDI Note On 消息
4. 更新 UI 显示鼓垫活动
5. 一定时间后自动发送 MIDI Note Off

## API 参考

### Pad 类

```cpp
class Pad {
public:
    // ADC 组(1-3)
    enum ADCGroup { ADC_1, ADC_2, ADC_3 };
    
    // 鼓垫标识(共10个鼓垫)
    enum PadID { OpenHiHat, CloseHiHat, Crash, Ride, SideStick, 
                Kick, Snare, MidTom, LowTom, HighTom, PAD_NUM };
    
    // 力度映射曲线(线性/对数/指数)
    enum ForceMappingCurve { CURVE_LINEAR, CURVE_LOG, CURVE_EXP };
    
    // 构造函数: 配置鼓垫参数
    // 参数说明: ADC组、ADC序号、输出端口/引脚、鼓垫ID、触发阈值、力度上限、映射曲线类型
    Pad(ADCGroup group, uint8_t adc_index, GPIO_TypeDef* port, uint16_t pin, 
        PadID id, uint16_t threshold, uint16_t upper_limit, ForceMappingCurve curve);
        
    // 核心功能:
    void detectHit();      // 检测新敲击(首先调用)
    bool isTriggered();    // 检查是否有新敲击
    void measureForce();   // 触发后计算力度
    uint8_t getForce();    // 获取力度值(0-127)
    
    // 实用功能:
    PadID getID();        // 获取鼓垫标识
    void setForceCurve(ForceMappingCurve curve); // 更改映射曲线
    uint16_t getADCVal_DBG(); // 获取原始ADC值(仅调试)
};
```

### UI 类

```cpp 
class UI {
public:
    // 显示模式(实时页面/菜单导航)
    enum class DisplayMode { PAGE, MENU };
    
    // 可用页面(主页面/测试页/设置页/统计页)
    enum class Page { MAIN, PAD_TEST, PAD_SETTING, STATS };
    
    // 初始化UI和按钮时序参数
    void init();
    void buttonInit(uint16_t debounce, uint16_t click_ms, uint16_t press_ms, uint16_t multi_ms);
    
    // 运行时功能(在主循环中调用):
    void buttonTick();    // 处理按钮输入
    void update();        // 刷新显示
    
    // 状态更新:
    void updatePadStats(Pad::PadID id, uint32_t hits); // 更新敲击计数
    void updateMidiStats();  // 更新MIDI统计
    void updateMidiConn(bool connected); // 更新连接状态
    
    // 显示控制:
    void setMode(DisplayMode mode); // 切换PAGE/MENU模式
    void setPage(Page page);       // 更改显示页面
    DisplayMode getMode();         // 获取当前模式
    Page getPage();                // 获取当前页面
};
```

### Midi 类

```cpp
class Midi {
public:
    // 发送MIDI消息:
    bool sendNoteOn(Pad::PadID padID, uint8_t velocity, uint8_t channel = 10);
    void sendNoteOff(Pad::PadID padID); // 自动使用鼓垫的音符/通道
    void sendNoteOff(uint8_t note, uint8_t channel = 10); // 手动音符关闭
    
    // 处理自动Note Off(在主循环中调用)
    void autoNoteOff();
    
    // 检查连接状态
    bool isConnected();
};
```

## 固件

可以使用 STM32CubeProgrammer 或者 STM32 Utility 工具烧写固件。
固件文件已编译好，在 Project folder/build 目录下。
