#include "MIDIUSB.h"

byte activeChannel;
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
  Serial.begin(115200);
}


void loop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0 && (rx.header == 0x09 || rx.header == 0x08)) {
      channel = rx.header == 0x09 ? 0x90 ^ rx.byte1 : 0x80 ^ rx.byte1;
      note = rx.byte2;
      velocity = rx.byte3;
      if (rx.header == 0x09 && activeChannel != channel) {
        activeChannel = channel;
        noteOn(2, note, velocity);   
      } else if (rx.header == 0x08) {
        activeChannel = -1;
        noteOff(2, note, velocity);
      }
      MidiUSB.flush();
    }
  } while (rx.header != 0);
}
