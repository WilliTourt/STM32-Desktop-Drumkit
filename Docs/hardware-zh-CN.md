# 硬件设计

## PCB元器件清单

PCB所需元器件在[BOM文件夹](../Hardware/Schematic%20&%20PCB/PCB%20BOM)的表格中均已列出。PCB的设计文件和Gerber也在[原理图/PCB文件夹](../Hardware/Schematic%20&%20PCB)中。PCB包括两部分：主板和用户交互小板。

## PCB原理图说明

### 主板原理图
![主板原理图](../Hardware/Schematic%20&%20PCB/Images/MotherBoard_Schematic.png)

主板主要包括以下部分：
- MCU
- 电源
- 通信接口
- 压电片信号处理

信号输入输出流程：
1. 10路信号处理电路将外部压电片原信号进行偏置和限幅
2. 处理后的信号连接到MCU的ADC
3. MCU发送MIDI信息到CH345T USB-MIDI芯片
4. 发送调试信息到CH340K
5. 控制板载蜂鸣器
6. 通过6pin GH1.25接口：
   - 发送I2C数据到OLED
   - 控制LED
   - 接收按键输入

### 用户交互小板原理图
![用户交互小板原理图](../Hardware/Schematic%20&%20PCB/Images/UI_Schematic.png)

- 通过6pin接口接收来自MCU的数据
- MCU同时检测按键信号

## PCB布局

### 主板PCB布局
![主板PCB布局](../Hardware/Schematic%20&%20PCB/Images/MotherBoard_PCB_Top.png)

- 上方：三个接口
  - MIDI
  - UART_DEBUG
  - SWD
- 右侧：
  - 6pin用于连接UI小板的接口
  - 10pin输出控制接口
- 下方：10路独立的压电片信号处理电路
- 中部：
  - 电源选择电路
  - 开关机电路
  - DCDC降压器
  - MCU
  - 4个额外的IO

## PCB焊接注意事项

### 主板焊接注意事项
1. TLV62568的反馈电阻(100k/453k)切勿相互焊反
2. CH340K部分：
   - R18和C19焊接时请二选一
   - 推荐仅焊接R18以向V3引脚供3.3v电源（详细解释见CH340K数据手册）
3. 注意信号处理电路的二极管方向
4. 所有用户LED都是可选的：
   - TX/RX指示灯
   - MIDI IN/OUT指示灯
   - MIDI USB_RDY
   - 3v3电源指示灯
5. MCU的晶振电路，1M电阻可不焊接

### UI小板焊接注意事项
1. OLED引脚插入深度：
   - 不要全部插入
   - 建议方法：
     - 等外壳打印好后
     - 将小板放置好
     - 将OLED插入，引脚刚好触即外壳缺口底部即可
