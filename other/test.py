import datetime
import wave
import serial
import time
import struct

ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
print("Waiting for serial connection...")

channels = 1
sample_width = 2  # 16-bit

now = datetime.datetime.now()
filename = f"recording_oo{now.strftime('%Y-%m-%d_%H-%M-%S')}.wav"

samples = []
print(f"Reading data...")

start = None
got_any = False
while True:
    line = ser.readline()
    if not line:
        if not got_any:
            # no data yet — keep waiting for first sample
            time.sleep(0.01)
            continue
        # we had data before and now got no data — stop reading
        break
    if not got_any:
        start = time.time()
        print("Started capturing samples...")
        got_any = True
    try:
        val = int(line) >> 14
        samples.append(val)
    except ValueError:
        continue

elapsed = time.time() - start
sample_rate = int(len(samples) / elapsed)
print(f"Captured {len(samples)} samples in {elapsed:.2f}s (~{sample_rate} Hz)")

if sample_rate <= 0:
    print("No samples captured, exiting.")
    ser.close()
    exit(1)

# Write WAV file
with wave.open(filename, 'wb') as wf:
    wf.setnchannels(channels)
    wf.setsampwidth(sample_width)
    wf.setframerate(44100)
    for v in samples:
        try: 
            wf.writeframes(struct.pack('<h', v))
        except Exception as e:
            print(f"Error writing frame: {e}")
            continue

print(f"Saved {filename} with sample_rate={sample_rate}")
ser.close()
