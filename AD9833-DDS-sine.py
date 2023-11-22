# generate 1.5 kHz sine signal using AD9833 DDS
# on ESP32-S3-DevKitC-1-N8R2 with ESP32S3

import board
import cedargrove_ad9833
import busio
import time
import neopixel # circup install neopixel

pixel = neopixel.NeoPixel(board.NEOPIXEL, 1, pixel_order=neopixel.RGBW)
# Green Red Blue (none)
pixel[0] = (1, 0, 1)  # if you don't set it,gets annoyingly bright

DDSspi = busio.SPI(clock=board.IO41, MOSI=board.IO42)
wave_gen = cedargrove_ad9833.AD9833(spi=DDSspi, select=board.IO40)

print("AD9833 Frequency test")

FREQ1K5 = 1500  # fixed or sweep starting frequency (Hz)
WAVE_TYPE = "sine"  # sine, triangle, or square waveform

while True:
    pixel[0] = (1, 0, 1)  # if you don't set it, gets annoyingly bright
    print("reset")
    wave_gen.reset()
    wave_gen.WAVE_TYPE = WAVE_TYPE
    print("start")
    wave_gen.start()

    wave_gen.wave_freq = FREQ1K5
    wave_gen.start()
    time.sleep(10)  # 10sec fixed hold time

    print("stop")
    wave_gen.stop()  # stop wave generator

    pixel[0] = (0, 0, 0)  # if you don't set it,gets annoyingly bright
    time.sleep(4)  # wait a second then do it all over again
