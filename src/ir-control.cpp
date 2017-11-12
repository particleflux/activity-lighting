/*
 * IRremote
 * Version 0.11 August, 2009
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Modified by Paul Stoffregen <paul@pjrc.com> to support other boards and timers
 * Modified  by Mitra Ardron <mitra@mitra.biz>
 * Added Sanyo and Mitsubishi controllers
 * Modified Sony to spot the repeat codes that some Sony's send
 * Modified by Gaspard van Koningsveld to trim out IRrecv, not using PWM anymore, allow setting of IR LED pin, and make it compatible with the Spark Core v1.0 (STM32F103CB based)
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 *
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */

#include "ir-control.h"
#include "application.h"

IRsend::IRsend(int irPin) : irPin(irPin) {};

void IRsend::sendNEC(unsigned long data, int nbits)
{
  enableIROut(38);
  mark(NEC_HDR_MARK);
  space(NEC_HDR_SPACE);
  for (int i = 0; i < nbits; i++) {
    if (data & TOPBIT) {
      mark(NEC_BIT_MARK);
      space(NEC_ONE_SPACE);
    }
    else {
      mark(NEC_BIT_MARK);
      space(NEC_ZERO_SPACE);
    }
    data <<= 1;
  }
  mark(NEC_BIT_MARK);
  space(0);
}

void IRsend::mark(int time) {
  // Sends an IR mark (frequency burst output) for the specified number of microseconds.
  noInterrupts();

  while (time > 0) {
    digitalWrite(irPin, HIGH); // this takes about 3 microseconds to happen
    delayMicroseconds(burstWait);
    digitalWrite(irPin, LOW); // this also takes about 3 microseconds
    delayMicroseconds(burstWait);

    time -= burstLength;
  }

  interrupts();
}

void IRsend::space(int time) {
  // Sends an IR space (no output) for the specified number of microseconds.
  digitalWrite(irPin, LOW); // Takes about 3 microsecondes
  if (time > 3) {
    delayMicroseconds(time - 3);
  }
}

void IRsend::enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // MAX frequency is 166khz.
  pinMode(irPin, OUTPUT);
  digitalWrite(irPin, LOW);

  // This is the time to wait with the IR LED on and off to make the frequency, in microseconds.
  // The - 3.0 at the end is because digitalWrite() takes about 3 microseconds. Info from:
  // https://github.com/eflynch/sparkcoreiremitter/blob/master/ir_emitter/ir_emitter.ino
  burstWait = round(1.0 / khz * 1000.0 / 2.0 - 3.0);
  // This is the total time of a period, in microseconds.
  burstLength = round(1.0 / khz * 1000.0);
}
