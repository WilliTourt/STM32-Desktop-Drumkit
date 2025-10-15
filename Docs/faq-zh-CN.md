# STM32 Desktop Drumkit - 常见问题 FAQ

## 鼓垫敲击后，电脑发不出声音？

1. 检查屏幕上是否提示 "MIDI: READY!", 如果没有, 请检查是否正确连接了MIDI接口。
2. 检查对应压电片的正负极是否正确连接。参考[组装指南](../Docs/partsassemble-zh-CN.md)中的`压电传感器制作 & 安装`部分的第四点。
3. 检查DAW中的MIDI连接设置，以及MIDI映射。
    ![MIDI设置](../Images/Debug/FL%20Screenshot.png)
4. 重新调试压电片，确保它的波形是正常的。

## 力度输出不准？

1. 重新微调参数，详情请见[调试与参数标定指南](../Docs/howtodebug-zh-CN.md)。
2. 这还有一部分采样率的影响，如果压电片峰值波形十分短，STM32可能无法捕捉到最准确的峰值。

## 有串扰/有误触发？

1. 检查你的`hit_threshold`和`HIT_THRESHOLD_OFFSET`参数。详情请见[调试与参数标定指南](../Docs/howtodebug-zh-CN.md)。

2. 如果你怎么调参数都还是有串扰，请检查含串扰问题的压电片波形是否在敲击瞬间有很强的跳变，尝试更换压电片。也可尝试对信号处理电路的滤波电容换用更高的电容值。
