# STM32 Desktop Drumkit - 硬件设计

> 此文档将会介绍STM32 Desktop Drumkit的硬件设计相关，以及PCB焊接和3D件打印相关说明。

![主板PCB图](../Hardware/Schematic%20&%20PCB/Images/MotherBoard.jpg)
![UI小板PCB图](../Hardware/Schematic%20&%20PCB/Images/UI%20Board.jpg)
![3D外壳图](../Hardware/3D%20Files/ImageSW.png)

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

板上线路和STM32CubeMX配置相对应。使用CubeMX查看外设和引脚配置。

## PCB焊接注意事项

### 主板焊接注意事项
1. TLV62568的反馈电阻(100k/453k)切勿相互焊反
2. CH340K部分：
   - R18和C19焊接时请二选一
   - 推荐仅焊接R18以向V3引脚供3.3v电源（详细解释见CH340K数据手册）
   ![CH340K](../Hardware/Schematic%20&%20PCB/Images/CH340K.jpg)
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
     - 将OLED插入，引脚刚好触即外壳缺口底部即可（稍后的[组装指南](Docs/partsassemble-zh-CN.md)中有提及）

## 3D 打印

所有相关的3D文件均在[3D Files文件夹](../Hardware/3D%20Files)中。一共需要打印这些部分：
- 外壳 ([Desktop Drumkit V1.0.step](../Hardware/3D%20Files/step%20&%203mf/Desktop%20Drumkit%20V1.0.STEP))
- 顶板 ([Drumkit Cover.step](../Hardware/3D%20Files/step%20&%203mf/Drumkit%20Cover.STEP))
- 叮叮镲支架 ([DrumpadStand_Ride.step](../Hardware/3D%20Files/step%20&%203mf/DrumpadStand_Ride.STEP))
- 7种不同大小的鼓皮紧固环 ([Fasteners 文件夹](../Hardware/3D%20Files/step%20&%203mf/Fasteners))，其中踩镲和高嗵鼓的环需要打印两个。

### 3D打印设置

如果你使用Bambu Studio，则可以直接使用我上传的3mf项目文件，切片选项已全部配置好（但注意切换打印机和打印板选项）。但外壳主题的3mf未能上传（因文件大小大于100M）

如果你使用其他3D打印软件，则需主要关注以下设置：
- 外壳：
  - 支撑：底部槽口无需添加支撑，**上方紧固圈部分需全部添加支撑！!**
  - 顶部壳体厚度：至少**1.6mm**，越厚越好。需要保证之后组装时用力按压紧固环时的结构完整性。
  - 稀疏填充：请不要低于12%
  - 速度：建议稍慢一些

- 紧固环
  - 墙层数：请直接塞满整个环

- 顶板
  - 顶板模型含有很薄的墙，注意放慢速度！！同时可以尝试Arachne墙生成

- 叮叮镲支架
  - 正常打印即可，无需特殊设置。
