#include <windows.h>
#include <iostream>
#include "balalaika.h"
#pragma comment(lib, "winmm.lib")

// MIDI output handle
HMIDIOUT midiOut;

// Send MIDI message
void sendMidiMessage(BYTE status, BYTE data1, BYTE data2) {
    DWORD msg = (DWORD)(status | (data1 << 8) | (data2 << 16));
    midiOutShortMsg(midiOut, msg);
}

// Play or stop a note
void playNoteEvent(const NoteEvent& event) {
    if (event.velocity > 0) {
        // Note On: status 0x90 (channel 0), note, velocity
        sendMidiMessage(0x90, (BYTE)event.note, (BYTE)event.velocity);
    }
    else {
        // Note Off: status 0x80 (channel 0), note, velocity 0
        sendMidiMessage(0x80, (BYTE)event.note, 0);
    }
}

int main() {
    // Open MIDI output device (default synthesizer)
    MMRESULT result = midiOutOpen(&midiOut, MIDI_MAPPER, 0, 0, 0);
    if (result != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open MIDI device! Error code: " << result << std::endl;
        system("pause");
        return 1;
    }

    int noteCount = sizeof(balalaika_track) / sizeof(NoteEvent);
    std::cout << "========================================" << std::endl;
    std::cout << "  MIDI Player - Balalaika" << std::endl;
    std::cout << "  Total note events: " << noteCount << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Playing... Press Ctrl+C to stop" << std::endl;

    // Iterate through note event array
    for (int i = 0;; i++) {
        const NoteEvent& event = balalaika_track[i];

        // Stop at end marker {-1, -1, -1}
        if (event.delay_ms == -1 && event.note == -1 && event.velocity == -1) {
            break;
        }

        // Delay (milliseconds)
        if (event.delay_ms > 0) {
            Sleep(event.delay_ms);
        }

        // Play / stop note
        playNoteEvent(event);
    }

    // Close MIDI device
    midiOutClose(midiOut);
    std::cout << "Playback finished!" << std::endl;
    system("pause");
    return 0;
}
