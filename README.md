## Input/Output

```
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
AnalogOut aout(PA_4);
AnalogIn ain(A0);
InterruptIn sw3(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
DigitalIn B1(D8);
DigitalIn B2(D9);
DigitalIn B3(D10);
```

## Buttom Select frequency
```
while(1) {
    if (B1 == 1){
        f = 10;
    }
    else if (B2 == 1){
        f = 50;
    }
    else if (B3 == 1){
        break;
    }
    uLCD.background_color(WHITE);
    uLCD.textbackground_color(WHITE);
    uLCD.cls();
    uLCD.locate(1, 1);
    uLCD.text_width(4); //4X size text
    uLCD.text_height(4);
    uLCD.color(GREEN);
    uLCD.printf("\n%d\n", f); 
}
```

## Generate triangle wave and store 1 second ADC data
```
float a1, a2;
a1 = (1.0f  / (sample * 0.9f)) * f;
a2 = (1.0f / (sample * 0.1f)) * f;
int k  = 0;

while(1) {                                             
    for (float i = 0; i <= 1; i = i + a1) {
        aout = i;
        if (k >= sample && k < 2*sample)
            ADC[k-sample] = ain;
        k++;
        ThisThread::sleep_for(1000ms / sample);
    }
    for (float i = 1; i >= 0; i = i - a2) {
        aout = i;
        if (k >= sample && k < 2*sample)
            ADC[k-sample] = ain;
        k++;
        ThisThread::sleep_for(1000ms / sample);
    }
}
```
![](https://i.imgur.com/enWeiqx.png)

## Use Thread to output ADC data when press user_button
```
t.start(callback(&queue, &EventQueue::dispatch_forever)); // Build an eventqueue for the multiple task 
sw3.rise(queue.event(Print_ADC));

void Print_ADC() {          
    for (int i = 0; i < 500; i++) {
        printf("%f\r\n", ADC[i]);
        ThisThread::sleep_for(2ms);
    }
}
```
## Result
![](https://i.imgur.com/3DQiQfx.png)

## FFT.py
```
import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 500.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,1,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
y = np.arange(0,1,Ts) # signal vector; create Fs samples

n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # a vector of frequencies; two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
for x in range(0, int(Fs)):
    line=s.readline() # Read an echo string from B_L4S5I_IOT01A terminated with '\n'
    # print line
    y[x] = float(line)

Y = np.fft.fft(y)/n*2 # fft computing and normalization
Y = Y[range(int(n/2))] # remove the conjugate frequency parts

fig, ax = plt.subplots(2, 1)
ax[0].plot(t,y)
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Amplitude')
ax[1].plot(frq,abs(Y),'r') # plotting the spectrum
ax[1].set_xlabel('Freq (Hz)')
ax[1].set_ylabel('|Y(freq)|')
plt.show()
s.close()
```




