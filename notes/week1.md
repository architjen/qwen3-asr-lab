## Create aiff audio file

```say \
  "Today is the first transcription test for my Qwen three ASR project. The expected number is forty two." \
  -o data/week1-synthetic.aiff
```

## Listen to audio aiff:

```afplay data/week1-synthetic.aiff
```


## Normalize the audio

```
ffmpeg \
  -i data/week1-synthetic.aiff \
  -ar 16000 \
  -ac 1 \
  -c:a pcm_s16le \
  data/week1-synthetic.wav
```