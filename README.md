# Balalaika MIDI —— 从音频到 C++ 数组，让电脑自己把曲子"弹"出来

一个把《巴拉莱卡》等曲子的音频转成 MIDI、再转成 C++ 音符数组、最后用 Windows MIDI 接口播放出来的小工具链。

整个链路只有四步，每步都留了中间产物：

```
音频 (.mp3/.m4a)
   │  basic-pitch（Spotify 开源的音频转 MIDI 模型）
   ▼
MIDI (.mid)
   │  midi_to_cpp.py
   ▼
C++ 头文件 (.h)   struct NoteEvent { delay_ms, note, velocity }
   │  player/main.cpp + winmm
   ▼
Windows 系统合成器播放
```

## 目录结构

| 路径 | 说明 |
|---|---|
| `midi_to_cpp.py` | 转换脚本：MIDI → C++ 音符数组 |
| `midi/` | basic-pitch 生成的 MIDI 文件（巴拉莱卡 + 三次录音） |
| `headers/` | 生成的 C++ 头文件：`巴拉莱卡.h`、`flower_dance.h`、`music_new_1.h`、`music_new_2.h` |
| `player/` | Visual Studio 播放器工程：`main.cpp` + `balalaika.h` + sln/vcxproj |

## 用法

### 1. 生成 MIDI

```bash
pip install basic-pitch
basic-pitch 你的音频.mp3        # 输出 xxx_basic_pitch.mid
```

### 2. MIDI 转 C++ 数组

```bash
pip install mido
python midi_to_cpp.py midi/巴拉莱卡_basic_pitch.mid headers/巴拉莱卡.h
```

生成的头文件长这样：

```cpp
#pragma once

struct NoteEvent { int delay_ms; int note; int velocity; };

const NoteEvent 巴拉莱卡_basic_pitch_track[] = {
    {1243, 69, 94},
    {370, 81, 59},
    {127, 69, 0},     // velocity = 0 表示松键
    ...
    {-1, -1, -1}      // 结束标志
};
```

脚本用 `mido` 的 `mid.play()` 回放事件流，把每个音符的**真实时间间隔**换算成毫秒，因此延时是演奏时序而非文件内部 tick。

### 3. 播放

用 Visual Studio 打开 `player/MidiPlayer.sln` 编译运行即可（Windows + winmm，链接 `winmm.lib`）。程序打开系统默认 MIDI 合成器，按数组里的延时逐条发送 Note On / Note Off。

## 说明

- 原始音频（3 个 `.m4a` 录音 + 1 个 `.mp3`，约 19 MB）未纳入仓库，仓库里保留的是生成的 `.mid` 与 `.h`，足以复现播放效果。
- 录音转 MIDI 用的是 basic-pitch 默认参数，polyphony 高的段落会有错音，`headers/` 里的版本是人工挑过的。
- `player/balalaika.h` 是给 VS 工程用的精简版（数组名 `balalaika_track`），`headers/巴拉莱卡.h` 是完整转写版。
