#include "MIDIUSB.h"

int activeChannel = -1;
byte channel;
byte note;
byte velocity;

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void setup() {
}

void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    // check only ON and OFF note events
    if (rx.header != 0 && (rx.header == 0x09 || rx.header == 0x08)) {
      channel = rx.header == 0x09 ? 0x90 ^ rx.byte1 : 0x80 ^ rx.byte1; // channel: 0 - 15
      note = rx.byte2; // note: 0 - 255
      velocity = rx.byte3; // velocity: 0 - 127
      if (rx.header == 0x09) {
        // set active channel as first event's channel
        if (activeChannel < 0) activeChannel = channel;
        if (channel == activeChannel) {
          // send midi ON event to selected channel/s
          noteOn(1, note, velocity);  
        }   
      } else if (rx.header == 0x08) {
        if (channel == activeChannel) {
          // reset active channel
          activeChannel = -1;
          // send midi OFF event to selected channel/s
          noteOff(1, note, velocity);
        }
      }
      MidiUSB.flush();
    }
  } while (rx.header != 0);
}
