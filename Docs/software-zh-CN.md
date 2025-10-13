# 固件 API 说明

## 资源正在整理中，将会很快更新文档。此文档内容不完全。

## 主模块与调用关系

- `cpp_main.cpp` 主循环入口，负责 Pad/MIDI/UI 调度
- `pad.cpp/h` 鼓垫采集与力度映射
    - `Pad::detectHit()` 采集并判断击打
    - `Pad::measureForce()` 测量并映射力度
    - `Pad::setForceCurve()` 设置力度曲线
- `midi.cpp/h` MIDI信号处理
    - `Midi::sendNoteOn()` 发送MIDI Note On
    - `Midi::sendNoteOff()` 发送MIDI Note Off
    - `Midi::autoNoteOff()` 自动管理 Note Off
    - `Midi::isConnected()` 检查 MIDI 连接状态
- `oled.cpp/h` OLED显示驱动
    - `OLED::printText()` 显示文本
    - `OLED::printVar()` 显示变量
    - `OLED::clear()` 清屏
    - `OLED::power()` 控制屏幕电源
- `OneButtonTiny.cpp/h` 按钮输入
    - `OneButtonTiny::tick()` 轮询检测
    - `attachClick/DoubleClick/MultiClick/LongPressStart()` 回调绑定
- `ui.cpp/h` 用户界面与菜单系统
    - `UI::update()` 刷新界面
    - `UI::updatePadStats()` 更新击打统计
    - `UI::updateMidiStats()` 更新 MIDI 统计
    - `UI::setMode()/setPage()` 切换显示/菜单状态

## 参数说明

- 按钮定时参数（去抖、单击、长按、多击）可在代码和菜单中设置
- 每个Pad的参数（阈值/上限/曲线）可在菜单或代码中调节
- 所有统计信息在UI类中自动更新

## 扩展接口

- 支持自定义菜单项和页面
- 支持增减鼓垫数量和类型
- 支持Debug串口输出

---
