#!/usr/bin/python

import RPi.GPIO as GPIO
from time import sleep
import datetime
from firebase import firebase
import Adafruit_DHT

import urllib2, urllib, httplib
import json
import os 
from functools import partial

GPIO.setmode(GPIO.BCM)
GPIO.cleanup()
GPIO.setwarnings(False)

# Sensor  Adafruit_DHT.DHT11 olarak yazilmalidir,
# DHT22 sensoru kullaniliyorsa Adafruit_DHT.DHT22, ya da  Adafruit_DHT.AM2302 yazilmalidir.
sensor = Adafruit_DHT.DHT11

# 4 nolu GPIO4 pini kullanilmistir.
pin = 4

# Sensorden okuma yapma denenir. Veri okumasi icin read_retry komutu kullanilmistir.
# 15 defaya kadar okuma denenir ve her deneme arasinda 2sn beklenir. 
nem, sicaklik = Adafruit_DHT.read_retry(sensor, pin)

firebase = firebase.FirebaseApplication('https://dht11-d3f03-default-rtdb.firebaseio.com/', None)
#firebase.put("/dht", "/temp", "0.00")
#firebase.put("/dht", "/humidity", "0.00")

def update_firebase():

  nem, sicaklik = Adafruit_DHT.read_retry(sensor, pin)
  if nem is not None and sicaklik is not None:
    sleep(5)
    str_temp = ' {0:0.2f} *C '.format(sicaklik) 
    str_hum  = ' {0:0.2f} %'.format(nem)
    print('Sicaklik={0:0.1f}*C  Nem={1:0.1f}%'.format(sicaklik, nem)) 
      
  else:
    print('Veri okuma hatasi. Tekrar deneyin!') 
    sleep(10)

  data = {"Sicaklik": sicaklik, "Nem": nem}
  firebase.post('/sensor/dht', data)
while True:
    update_firebase()
    
        #sleepTime = int(sleepTime)
    sleep(5)
