import uwebsockets.client
import sys,time
import machine, time
import network
from machine import Pin,SoftI2C
from time import sleep
import _thread
import upip
from hcsr04 import HCSR04
#import ssd1306
from machine import Timer

TIMEOUT_MS = 5000 #soft-reset will happen around 5 sec

def timeout_callback(t):
    try:
        print("I am in Call back")
        websocket.send("Ws send \r")
    finally:
        print("I am in Call back finally")
       

def wlan_connect(ssid,pwd):
    wlan = network.WLAN(network.STA_IF)
    if not wlan.active() or not wlan.isconnected():
        wlan.active(True)
        wlan.connect(ssid,pwd)
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())
    #upip.install("")
    
    
wlan_connect('1234','1234567890')
p0 = Pin(2, Pin.OUT) 
uri = "wss://em7iuj070m.execute-api.ap-south-1.amazonaws.com/production"
websocket = uwebsockets.client.connect(uri)
#print(websocket)
ultrasonic = HCSR04(trigger_pin=13, echo_pin=12, echo_timeout_us=1000000)
while True:
    distance = ultrasonic.distance_cm()
    print(distance)
    websocket.send(str(distance))
    time.sleep_ms(100)
"""
while True:
    #print("Enter Command:\r")
    #mesg=input()
    #websocket.send(mesg + "\r")
    timer = Timer(0)
    timer.init(period=TIMEOUT_MS, callback=timeout_callback)
    resp = websocket.recv()
    if "on" in resp:
        p0.on()
        print("Led is on")
    elif "off" in resp:
        p0.off()
        print("Led is off")
    else:
        print("No data recieved")
    #sleep(1)"""