#include "mbed.h"
#include "uLCD_4DGL.h"

int j = 1, i = 20;

using namespace std::chrono;

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
AnalogOut  aout(PA_4);
AnalogIn Ain(A0);

Timer debounce;                  //define debounce timer
InterruptIn button1(D9); //Interrupt on digital pushbutton input SW2
InterruptIn button2(D6);
InterruptIn button3(D5);
DigitalOut led1(LED1);
DigitalOut led2(LED2);

int sample = 128;
int l;
float ADCdata[128];

Thread thread;

InterruptIn sw2(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

void led1_info() {
   for (int l = 0; l < sample; l++){
            aout = Ain;
            ADCdata[l] = Ain;
            ThisThread::sleep_for(1000ms/sample);
        }
    for ( l = 0; l < sample; l++) {
        printf("%f\r\n", ADCdata[l]);
        ThisThread::sleep_for(100ms);
    }
}

void Trig_led1()  {
   // Execute the time critical part first
   led1 = !led1;
   // The rest can execute later in user context
   queue.call(led1_info);
}

void toggle1()
{
   if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 200) {
      //only allow toggle if debounce timer has passed 1s
        if (j == 1) {
            i = i;
        } else if (j == 0) {
            i--;
        }
        debounce.reset(); //restart timer when the toggle is performed
   }
}

void toggle2()
{
   if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 200) {
        //only allow toggle if debounce timer has passed 1s
        if (j == 1) {
            i = i;
        } else if (j == 0) {
            i++;
        }
        debounce.reset(); //restart timer when the toggle is performed
   }
}

void toggle3()
{
   if (duration_cast<milliseconds>(debounce.elapsed_time()).count() > 200) {
        //only allow toggle if debounce timer has passed 1s
        if (j == 1) {
            j = 0;
        } else {
            j = 1;
        }
        debounce.reset(); //restart timer when the toggle is performed
   }
}

void uLCD_thread() {
    uLCD.printf("menu\n"); //Default Green on black text
     uLCD.printf("fequency selector\n");
    while (1) {    
        uLCD.locate(0,3);
        uLCD.printf("%2dHz", i);
        if (j == 0) {
            uLCD.locate(0,4);
            uLCD.printf("select");
        } else if (j == 1) {
            uLCD.locate(0,4);
            uLCD.printf("start ");
        }
    }
}

void analog_thread() {
    for (int l = 0; l < sample; l++){
            aout = Ain;
            ADCdata[l] = Ain;
            ThisThread::sleep_for(1000ms/sample);
        }
        for ( l = 0; l < sample; l++) {
            printf("%f\r\n", ADCdata[l]);
            ThisThread::sleep_for(100ms);
        }
}

int main()
{
    thread.start(uLCD_thread);
    // thread.start(analog_thread);

    t.start(callback(&queue, &EventQueue::dispatch_forever));
   // 'Trig_led1' will execute in IRQ context
   sw2.rise(Trig_led1);

    debounce.start();
    button1.rise(&toggle1); // attach the address of the toggle
    button2.rise(&toggle2);
    button3.rise(&toggle3);

    while (1) {
        while (j == 1) {
            for (float k = 0.0; k < 10.0/11.0; k += 10.0/11/9) {
                aout = k;
                ThisThread::sleep_for(100ms/2/i);
            }
            for (float h = 10.0/11 / 2; h >= 0; h -= 10.0/11/2) {
                aout = h;
                ThisThread::sleep_for(100ms/2/i);
            }
        }
        ThisThread::sleep_for(100ms);
    }
}
