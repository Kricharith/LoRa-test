from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD
import datetime
import time
import serial
##########################################################
import bme280
import smbus2 
##########################################################
from RPLCD.i2c import CharLCD

#lcd = CharLCD(i2c_expander='PCF8574', address=0x27, port=1, cols=16, rows=2, dotsize=8)

#lcd.clear()
#lcd.write_string('Hello, World!')
##########################################################
import BlynkLib
import RPi.GPIO as GPIO
from BlynkTimer import BlynkTimer

BLYNK_AUTH_TOKEN = 'CIHc7n1b6j6-3ebPXXCHgMw6m2wM-fM0'

led1 = 7
led2 = 8
GPIO.setmode(GPIO.BCM)
GPIO.setup(led1, GPIO.OUT)
GPIO.setup(led2, GPIO.OUT)

x = 20
# Initialize Blynk
blynk = BlynkLib.Blynk(BLYNK_AUTH_TOKEN)

timer = BlynkTimer()
##############################
port = 1
#address = 0x76 # Adafruit BME280 address. Other BME280s may be different
#bus = smbus2.SMBus(port)
#bme280.load_calibration_params(bus,address)
##############################
# Led control through V0 virtual pin
@blynk.on("V0")
##########################################################

##########################################################
# Led control through V0 virtual pin
@blynk.on("V0")
def v0_write_handler(value):
#    global led_switch
    if int(value[0]) is not 0:
        GPIO.output(led1, GPIO.HIGH)
        print('LED1 HIGH')
    else:
        GPIO.output(led1, GPIO.LOW)
        print('LED1 LOW')

# Led control through V0 virtual pin
@blynk.on("V1")
def v1_write_handler(value):
#    global led_switch
    if int(value[0]) is not 0:
        GPIO.output(led2, GPIO.HIGH)
        print('LED2 HIGH')
    else:
        GPIO.output(led2, GPIO.LOW)
        print('LED2 LOW')

#function to sync the data from virtual pins
@blynk.on("connected")
def blynk_connected():
    print("Raspberry Pi Connected to New Blynk")
##########################################################
def myData():
	#bme280_data = bme280.sample(bus,address)
	#temp = bme280_data.temperature
	temp = 20
	print(datasend)
	blynk.virtual_write(3,str(datasend))
	print("Sensor send")
timer.set_interval(10,myData)

BOARD.setup()  
 
class LoRaSender(LoRa):
	
	def __init__(self, verbose=False):
		super(LoRaSender, self).__init__(verbose)
		self.set_mode(MODE.SLEEP)
		self.set_dio_mapping([0] * 6)
		self.destination_Node1 = hex(0xa1)
		self.destination_Node2 = hex(0xb1)
		self.destination_Node3 = hex(0xc1)
		self.localAddress_Gateway = hex(0xbb)
		self.received_data = 0
		self.localAddress_rx = 0
		self.destination_rx = 0
		self.data_length_rx = 0
		self.stateRx = False
		self.checkDataNode = 0
		self.count = 0
	def on_rx_done(self):
		#print("\nRxDone")
		#self.clear_irq_flags(RxDone=1)
		payload = self.read_payload(nocheck=True)
		print("Received:", payload)
		if payload is not None and len(payload) >= 3:  # ตรวจสอบว่า payload มีข้อมูลและมีอย่างน้อย 3 องค์ประกอบ
			self.received_data = bytes(payload).decode("utf-8", 'ignore')
			self.localAddress_rx = payload[0]
			self.destination_rx = payload[1]
			self.data_length_rx = payload[2]
			self.stateRx = True
	def send_data(self, node):
		if node == 1:
			print(">>>>>>>>>>>>>>> Send Node 1 <<<<<<<<<<<<<<<<<<")
			sleep(1) 
			data ="CPE"
			print("Sending data:", data)
			data_length = len(data)
			data_to_send = str(int(self.destination_Node1,16)) + " " + str(int(self.localAddress_Gateway,16)) + " " + str(data_length) + " " + data
			print(data_to_send.split())
			self.write_payload(data_to_send)
			self.set_mode(MODE.TX)
			self.clear_irq_flags(TxDone=1)
			sleep(0.5) 
			self.set_mode(MODE.STDBY)
			self.set_mode(MODE.RXCONT)
		elif node == 2:
			print(">>>>>>>>>>>>>>> Send Node 2 <<<<<<<<<<<<<<<<<<")
			sleep(1) 
			data ="CPE"
			print("Sending data:", data)
			data_length = len(data)
			data_to_send = str(int(self.destination_Node2,16)) + " " + str(int(self.localAddress_Gateway,16)) + " " + str(data_length) + " " + data
			print(data_to_send.split())
			self.write_payload(data_to_send)
			self.set_mode(MODE.TX)
			self.clear_irq_flags(TxDone=1)
			sleep(0.5) 
			self.set_mode(MODE.STDBY)
			self.set_mode(MODE.RXCONT)
		elif node == 3:
			print(">>>>>>>>>>>>>>> Send Node 3 <<<<<<<<<<<<<<<<<<")
			sleep(1)
			
			if self.count == 0:
				data ="PUMP_ON"
				self.count =1
			elif self.count == 1:
				data ="PUMP_OFF"
				self.count =0
			
			print("Sending data:", data)
			data_length = len(data)
			data_to_send = str(int(self.destination_Node3,16)) + " " + str(int(self.localAddress_Gateway,16)) + " " + str(data_length) + " " + data
			print(data_to_send.split())
			self.write_payload(data_to_send)
			self.set_mode(MODE.TX)
			self.clear_irq_flags(TxDone=1)
			sleep(0.5) 
			self.set_mode(MODE.STDBY)
			self.set_mode(MODE.RXCONT)
		
	def checkData(self):
		if hex(self.localAddress_rx) == self.localAddress_Gateway and hex(self.destination_rx) == self.destination_Node1:
			#current_time = datetime.datetime.now()
			print("***********************************************************")
			print("**************************Node 1***************************")
			print("*** destination  :" ,hex(self.destination_rx))
			print("*** localAddress :" ,hex(self.localAddress_rx))
			print("*** data         :" ,self.received_data)
			print("***********************************************************")
			print("***********************************************************")
			sleep(0.1)
			self.checkDataNode = 1
		elif hex(self.localAddress_rx) == self.localAddress_Gateway and hex(self.destination_rx) == self.destination_Node2:
			print("***********************************************************")
			print("**************************Node 2***************************")
			print("*** destination  :" ,hex(self.destination_rx))
			print("*** localAddress :" ,hex(self.localAddress_rx))
			print("*** data         :" ,self.received_data)
			print("***********************************************************")
			print("***********************************************************")
			sleep(0.1)
			self.checkDataNode = 2
		elif hex(self.localAddress_rx) == self.localAddress_Gateway and hex(self.destination_rx) == self.destination_Node3:
			print("***********************************************************")
			print("**************************Node 3***************************")
			print("*** destination  :" ,hex(self.destination_rx))
			print("*** localAddress :" ,hex(self.localAddress_rx))
			print("*** data         :" ,self.received_data)
			print("***********************************************************")
			print("***********************************************************")
			sleep(0.1)
			self.checkDataNode = 3
		else :
			self.checkDataNode = 0
	def start(self):
		self.reset_ptr_rx()
		self.set_mode(MODE.RXCONT)
		state = "ONE"  # กำหนด initial state เป็น "WAITING"
		while True:
			# การทำงานตาม State 
			if state == "ONE":  #   อ่านค่าเซ็นเซอร์
				print(state)
				state = "TWO"
			elif state == "TWO": #นำค่าที่ได้รับมาจากNode มาตรวจสอบก่อนยืนยันกลับไปที่ node
				print(state)
				if self.stateRx == True:
					self.checkData()
				state = "THREE"
			elif state == "THREE":                #ส่งข้อมูลยืนยันกลับไปที่ node
				print(state)
				if self.checkDataNode == 1 or self.checkDataNode == 2 or self.checkDataNode == 3:
					self.send_data(self.checkDataNode)
					self.stateRx = False
					self.checkDataNode = 0
				state = "ONE"
			# กระบวนการที่ต้องทำในทุก iteration
			sleep(0.2)
			blynk.run()
			rssi_value = self.get_rssi_value()
			status = self.get_modem_status()
			sys.stdout.flush()
        
# กำหนดค่าต่างๆ และเริ่มต้น LoRa
lora = LoRaSender(verbose=True)
lora.set_mode(MODE.STDBY)
lora.set_pa_config(pa_select=1)
lora.set_freq(923.0)  # ต้องกำหนดค่าตามที่ใช้งาน

# เริ่มต้นโปรแกรม
lora.start()

# save by jack 
#print("destination_rx :" ,hex(self.destination_rx))
#print("data_length_rx :" ,self.data_length_rx),
#datasend = self.received_data
#blynk.virtual_write(3,str(datasend))
#current_data = "Time"+str(current_time)+" Data "+self.received_data 
#print(current_data)
#with open("logfile.txt", "a") as log_file:
#	log_file.write(current_data + "\n")
