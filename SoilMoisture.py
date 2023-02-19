import machine
import time
from time import sleep
from machine import Pin, ADC
sms = ADC(Pin(34))
sms.atten(ADC.ATTN_11DB)    
sms_val = sms.read()
print (sms.read())
if(sms.read()>4055):
    led=Pin(14,Pin.OUT)
    while True:
        led.on()
        time.sleep(1)
        led.off()
        time.sleep(1)
sleep(3)



