<CsoundSynthesizer>
<CsOptions>
; Select audio/midi flags here according to platform
;-odac     ;;;realtime audio out
;-iadc    ;;;uncomment -iadc if realtime audio input is needed too
; For Non-realtime ouput leave only the line below:
; -o oscili.wav -W ;;; for file output any platform
-n 
</CsOptions>

<CsInstruments>

sr = 44100
ksmps = 64
nchnls = 2
0dbfs  = 1

chn_k "gain", 1

instr 1

kamp = .6
kcps = cpspch(p5)
ifn  = p4

kenv linseg 0, 0.01, 1, p3, 0, 1, 0 
kgain chnget "gain"
asig oscil kamp * kenv, kcps, ifn
     outs kgain*asig, kgain*asig

endin

instr 2

ksig oscil 1, 0.08

chnset (0.5 * (ksig + 1)), "showGain" 

endin

</CsInstruments>
<CsScore>
f1 0 128 10 1                                          ; Sine with a small amount of data
f2 0 128 10 1 0.5 0.3 0.25 0.2 0.167 0.14 0.125 .111   ; Sawtooth with a small amount of data
f3 0 128 10 1 0   0.3 0    0.2 0     0.14 0     .111   ; Square with a small amount of data
f4 0 128 10 1 1   1   1    0.7 0.5   0.3  0.1          ; Pulse with a small amount of data

f0 10000
i 1  0 4 1 8.00
i 2  0 -1
e
</CsScore>
</CsoundSynthesizer>

<SvaraUi>
config:
  size:
    width: 400
    height: 300
state:
  init:
    gain!: 0.5
    showGain: 0.0
    but0!: 0
    but1!: 0
    but2!: 0
    but3!: 0
    but4!: 0
    but5!: 0
    but6!: 0
    but7!: 0
    but8!: 0
    but9!: 0
    but10!: 0
  keyboard:
    " ": 
      down: 
        but0: 1  
    z: 
      down: 
        but1: 1  
    x: 
      down: 
        but2: 1  
    c: 
      down: 
        but3: 1  
    v: 
      down: 
        but4: 1  
    b: 
      down: 
        but5: 1  
    n: 
      down: 
        but6: 1  
    m: 
      down: 
        but7: 1  
    ",": 
      down: 
        but8: 1 
    ".": 
      down: 
        but9: 1 
    "/": 
      down: 
        but10: 1 
ui:
  ver:
    - hor:
        - knob: gain
        - dot-meter: showGain
    - hor:
        - button: but0
          style:
            color: maroon
        - button: but1
        - button: but2
        - button: but3
        - button: but4
        - button: but5
        - button: but6
        - button: but7
        - button: but8
        - button: but9
        - button: but10
      scale: 0.5
      style:
        pad: 0.03
csound:
  write: [gain]
  read: [showGain]
  score:
    but0: [1, 0, 2, 1, 6.07]
    but1: [1, 0, 1, 1, 8.00]
    but2: [1, 0, 1, 1, 8.02]
    but3: [1, 0, 1, 1, 8.04]
    but4: [1, 0, 1, 1, 8.05]
    but5: [1, 0, 1, 1, 8.07]
    but6: [1, 0, 1, 1, 8.08]
    but7: [1, 0, 1, 1, 8.11]
    but8: [1, 0, 1, 1, 9.00]
    but9: [1, 0, 1, 1, 9.02]
    but10: [1, 0, 1, 1, 9.04]
</SvaraUi>



