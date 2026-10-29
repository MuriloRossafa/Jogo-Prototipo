"""Generate an original placeholder cue; Python standard library only."""
from pathlib import Path
import math
import random
import struct
import wave

def main():
    target = Path(__file__).resolve().parents[1] / 'SourceAudio' / 'Warning.wav'
    target.parent.mkdir(parents=True, exist_ok=True)
    if target.exists():
        raise SystemExit('Existing audio preserved; remove it explicitly to regenerate.')
    rng = random.Random(1978)
    rate = 22050
    frames = bytearray()
    for i in range(rate):
        t = i / rate
        envelope = math.exp(-t * 9) * min(t * 100, 1)
        sample = (math.sin(t * math.tau * 95) * .65 + rng.uniform(-1, 1) * .25) * envelope
        frames.extend(struct.pack('<h', int(sample * 20000)))
    with wave.open(str(target), 'wb') as audio:
        audio.setparams((1, 2, rate, 0, 'NONE', 'not compressed'))
        audio.writeframes(frames)
    print(target)

if __name__ == '__main__':
    main()
