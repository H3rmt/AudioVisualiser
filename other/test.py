#!/usr/bin/env python3

import datetime
import serial
import wave

PORT = "/dev/ttyACM0"
BAUD = 921600
SAMPLE_RATE = 44100
CHANNELS = 1
SAMPLE_WIDTH = 2  # 32-bit
BLOCK_SAMPLES = 512
BLOCK_BYTES = BLOCK_SAMPLES * SAMPLE_WIDTH

filename = (
    f"recording_{datetime.datetime.now().strftime('n%Y-%m-%d_%H-%M-%S')}.wav"
)

ser = serial.Serial(PORT, BAUD, timeout=2)
print("Waiting for data...")

written = 0

with wave.open(filename, "wb") as wf:
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(SAMPLE_WIDTH)
    wf.setframerate(SAMPLE_RATE)

    while True:
        try:
            chunk = ser.read(BLOCK_BYTES)
        except serial.SerialException:
            chunk = None
        if not chunk:
            print("Timeout")
            break
        wf.writeframes(chunk)
        written += len(chunk)
        print(f"\rWritten {written} bytes", end="")

print()
ser.close()
print(f"Saved {filename}")
