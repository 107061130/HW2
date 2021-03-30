#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
AnalogOut aout(PA_4);
AnalogIn ain(A0);
InterruptIn sw3(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
DigitalIn B1(D8);
DigitalIn B2(D9);
DigitalIn B3(D10);

Thread t;
float ADC[500];
void Print_ADC();

int main(){

    int f = 10;
    int sample = 500;  

    aout = 3.3;
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
        uLCD.text_width(4); 
        uLCD.text_height(4);
        uLCD.color(GREEN);
        uLCD.printf("\n%d\n", f); 
   }

    float a1, a2;
    a1 = (1.0f  / (sample * 0.9f)) * f;
    a2 = (1.0f / (sample * 0.1f)) * f;
    int k  = 0;

    t.start(callback(&queue, &EventQueue::dispatch_forever)); // Build an eventqueue for the multiple task 
    sw3.rise(queue.event(Print_ADC));
                        
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

}

void Print_ADC() {          
    for (int i = 0; i < 500; i++) {
        printf("%f\r\n", ADC[i]);
        ThisThread::sleep_for(2ms);
    }
}
