import network
import upip
# Wlan_connect helps us to connect our esp32 board to internet , if it connects sucessfully then it prints the ipaddress
def wlan_connect(ssid,pwd):
    wlan = network.WLAN(network.STA_IF)
    if not wlan.active() or not wlan.isconnected():
        wlan.active(True)
        wlan.connect(ssid,pwd)
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())
    #upip install is used to install required libraries on ESP32 board
    upip.install("micropython-logging")
    
#pass wifi userid and password     
wlan_connect('V2030', 'Sai Likhita12')
