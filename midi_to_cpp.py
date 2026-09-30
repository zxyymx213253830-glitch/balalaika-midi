import mido
import sys
import os

def convert(midi_path, output_path):
    try:
        mid = mido.MidiFile(midi_path)
        with open(output_path, 'w') as f:
            f.write("#pragma once\n\n// MIDI 自动转 C++ 数据\n")
            f.write("struct NoteEvent { int delay_ms; int note; int velocity; };\n\n")
            
            # 使用文件名（去除后缀）作为数组变量名
            base_name = os.path.basename(midi_path).split('.')[0].replace('-', '_').replace(' ', '_')
            f.write(f"const NoteEvent {base_name}_track[] = {{\n")
            
            # 模拟播放逻辑来提取真实时间间隔
            for msg in mid.play():
                if msg.type in ['note_on', 'note_off']:
                    delay = int(msg.time * 1000)
                    # 如果是 note_off 或者 velocity 为 0，统一记为松开按键 (0)
                    vel = msg.velocity if msg.type == 'note_on' else 0
                    f.write(f"    {{{delay}, {msg.note}, {vel}}},\n")
            
            f.write("    {-1, -1, -1} // 结束标志\n};\n")
        print(f"✅ 成功！头文件已生成: {output_path}")
    except Exception as e:
        print(f"❌ 发生错误: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("用法: python3 midi_to_cpp.py <输入.mid> <输出.h>")
    else:
        convert(sys.argv[1], sys.argv[2])
