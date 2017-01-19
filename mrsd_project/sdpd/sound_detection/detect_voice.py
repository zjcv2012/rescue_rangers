import vamp
import librosa
import matplotlib

from matplotlib import pyplot as plt
import numpy as np

def detect():

    audio_file = './rescue_rangers.wav'
    audio, sr = librosa.load(audio_file, sr=44100, mono=True)

    params = {"minfqr": 100.0, "maxfqr": 500.0, "voicing": 0.2, "minpeaksalience": 0.0}
    data = vamp.collect(audio, sr, "mtg-melodia:melodia", parameters=params)
    hop, melody = data['vector']

    timestamps = 8*128/44100.0 +np.arange(len(melody)) * (128/44100.0)

    melody_pos = melody[:]
    melody_pos[melody<=0] = None

    plt.figure(figsize=(18,6))
    plt.plot(timestamps, melody_pos)
    plt.xlabel('Time s')
    plt.ylabel('Freq hz')
    plt.show()

if __name__ == "__main__":
    detect()
