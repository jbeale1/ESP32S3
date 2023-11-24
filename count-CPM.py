# Report CPM from geiger counter pulse signal
# uses CircuitPython v8:  adafruit-circuitpython-espressif_esp32s3_devkitc_1_n8r8-en_US-8.2.8.bin
# J.Beale 2023-11-23

import board
import countio
import time
import supervisor
import array   # for circular buffer

# --------------------------------------------------------
bufSize = 20
bufA=array.array('i',(0,)*bufSize)
bufIdx = 0

# circular buffer. bufIdx is index of last added element
def pushBuf(val):
    global bufA, bufIdx
    bufIdx = (bufIdx + 1)  % bufSize
    bufA[bufIdx] = val
    
def avgBuf():
    average = sum(bufA) / bufSize
    return average
    
# --------------------------------------------------------
# wait for top of next time interval...       
def waitForNext(interval_ms):
    msec = supervisor.ticks_ms() # milliseconds since CPU boot
    msecLeft = interval_ms - (msec % interval_ms)
    delaySec = (msecLeft / 1000.0)  # how many seconds to wait
    if (delaySec > 0.002): # otherwise sometimes get short cycles
        time.sleep(delaySec)  # delay until top of next interval
    else:
        time.sleep(interval_ms/1000.0) # delay one full interval

# --------------------------------------------------------
interval_ms = 60000 # this long between cycles of loop, in milliseconds

pin_counter = countio.Counter(board.IO9, edge=countio.Edge.RISE)

print("minutes, average, count")
elapsedOld = 0
countLast = 0  # total pulse count, will roll over eventually
waitForNext(interval_ms)
pin_counter.reset()
tOffset = time.monotonic()
time.sleep(interval_ms/1000.0) # accumulate first interval

bitmask16 = (1<<16)-1 #use this value for every 16-bit operation
# note: countio.Counter has 16-bit register, max value = 65535
while True:
    countNow = pin_counter.count  # total pulse count, will roll over eventually
    pulseCount = (countNow - countLast) & bitmask16
    countLast = countNow
    
    elapsed = time.monotonic()-tOffset # in seconds
    delta = elapsed - elapsedOld
    elapsedOld = elapsed
    pushBuf(pulseCount)
    print("%05.1f, %06.3f, %d" % (elapsed/60, avgBuf(), pulseCount))

    waitForNext(interval_ms)
                
# ================================================================

