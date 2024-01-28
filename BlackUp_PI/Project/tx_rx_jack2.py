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

BLYNK_AUTH_TOKEN = 'qTdhnZiZxMPcWAkudaX7VnJzEYCJcyrn'


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
##########################################################

##########################################################

@blynk.on("V6")
def blynk_handle_vpins(value):
    blynkModeNode1 = int(value[0])
    lora.setBlynkModeNode1(blynkModeNode1)
    print("----------------------------------UPDATE--------------------------------------")
    print("blynkModeNode1 ",blynkModeNode1)
@blynk.on("V7")
def blynk_handle_vpins(value):
    blynkTimeNormalNode1 = float(value[0])
    lora.setBlynkTimeNormalNode1(blynkTimeNormalNode1)
    print("----------------------------------UPDATE--------------------------------------")
    print("blynkTimeNormalNode1 ",blynkTimeNormalNode1)
@blynk.on("V8")
def blynk_handle_vpins(value):
    blynkTimeDebugNode1 = float(value[0])
    lora.setBlynkTimeDebugNode1(blynkTimeDebugNode1)
    print("----------------------------------UPDATE--------------------------------------")
    print("blynkTimeDebugNode1 ",blynkTimeDebugNode1)
    
@blynk.on("V16")
def blynk_handle_vpins(value):
    blynkModeNode2 = int(value[0])
    lora.setBlynkModeNode2(blynkModeNode2)
    print("----------------------------------UPDATE--------------------------------------")
    print("blynkModeNode2 ",blynkModeNode2)
@blynk.on("V17")
def blynk_handle_vpins(value):
    blynkTimeNormalNode2 = float(value[0])
    lora.setBlynkTimeNormalNode2(blynkTimeNormalNode2)
    print("----------------------------------UPDATE--------------------------------------")
    print("blynkTimeNormalNode2 ",blynkTimeNormalNode2)
@blynk.on("V18")
def blynk_handle_vpins(value):
    blynkTimeDebugNode2 = float(value[0])
    lora.setBlynkTimeDebugNode2(blynkTimeDebugNode2)
    print("----------------------------------UPDATE--------------------------------------")
    print("blynkTimeDebugNode2 ",blynkTimeDebugNode2)
      
      
# Led control through V0 virtual pin
#function to sync the data from virtual pins
#@blynk.on("connected")
#def blynk_connected():
#    print("Raspberry Pi Connected to New Blynk")
##########################################################
#def myData(self):
	#bme280_data = bme280.sample(bus,address)
	#temp = bme280_data.temperature
	
#	print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
#	print(self.localAddress_Gateway)
#timer.set_interval(10,myData)
BOARD.setup()  
 
class LoRaGateway(LoRa):
	def __init__(self, verbose=False):
		super(LoRaGateway, self).__init__(verbose)
		self.set_mode(MODE.SLEEP)
		self.set_dio_mapping([0] * 6)
		self.localAddress_Gateway = hex(0xbb)
		self.received_data = 0
		self.localAddress_rx = 0
		self.destination_rx = 0
		self.data_length_rx = 0
		self.stateRx = False
		self.checkDataNode = 0
		self.previousMillisNode1 = int(round(time.time() * 1000))  # ใช้ time.time() เพื่อดึงเวลาปัจจุบัน (ในวินาที) และคูณด้วย 1000 เพื่อแปลงเป็นมิลลิวินาที
		self.previousMillisNode2 = int(round(time.time() * 1000))  # ใช้ time.time() เพื่อดึงเวลาปัจจุบัน (ในวินาที) และคูณด้วย 1000 เพื่อแปลงเป็นมิลลิวินาที
		self.previousMillisNode3 = int(round(time.time() * 1000))  # ใช้ time.time() เพื่อดึงเวลาปัจจุบัน (ในวินาที) และคูณด้วย 1000 เพื่อแปลงเป็นมิลลิวินาที
		#//////////////////////Node 1/////////////////////////
		self.destination_Node1 = hex(0xa1)
		self.confirmNode1 = "Pass"
		self.waterLevelNode1 = 0
		self.tempNode1 = 0
		self.humNode1 = 0
		self.battNode1 = 0
		self.stateNode1 = 0
		self.modeNode1 = 0    # 0 = DebugMode , 1 = NormalMode
		self.TimeNormalNode1 = 2
		self.TimeDebugNode1 = 1
		
		self.checkstateNode1 = 0
		self.blynkModeNode1 = 0
		self.blynkTimeNormalNode1 = 0
		self.blynkTimeDebugNode1 = 0
		
		#//////////////////////Node 2/////////////////////////
		self.destination_Node2 = hex(0xb1)
		self.confirmNode2 = "Pass"
		self.waterLevelNode2 = 0
		self.tempNode2 = 0
		self.humNode2 = 0
		self.battNode2 = 0
		self.stateNode2 = 0
		self.modeNode2 = 0    # 0 = DebugMode , 1 = NormalMode
		self.TimeNormalNode2 = 2
		self.TimeDebugNode2 = 1
		
		self.checkstateNode2 = 0
		self.blynkModeNode2 = 0
		self.blynkTimeNormalNode2 = 0
		self.blynkTimeDebugNode2 = 0
		
		#//////////////////////Node 3/////////////////////////
		self.destination_Node3 = hex(0xc1)
		self.confirmNode3 = "Pass"
		self.tempNode3 = 0
		self.humNode3 = 0
		self.pumpModeNode3 = 0	   # 0 = Manual    , 1 = Auto
		self.modeNode3 = 0         # 0 = DebugMode , 1 = NormalMode
		self.pumpStateNode3 = 0    # 0 = OFF       , 1 = ON
		self.pumpStatusNode3 = 0   

		self.statusNode3 = 0
		self.TimeNormalNode3 = 2
		self.TimeDebugNode3 = 1
		self.checkstateNode3 = 0
		#/////////////////////////////////////////////////////
	def setBlynkModeNode1(self, blynkModeNode1):
		self.blynkModeNode1 = blynkModeNode1
		print("pass", self.blynkModeNode1)
		
	def setBlynkTimeNormalNode1(self, blynkTimeNormalNode1):
		self.blynkTimeNormalNode1 = blynkTimeNormalNode1
		print("pass", self.blynkModeNode1)
		
	def setBlynkTimeDebugNode1(self, blynkTimeDebugNode1):
		self.blynkTimeDebugNode1 = blynkTimeDebugNode1
		print("pass", self.blynkTimeDebugNode1)
		
	def setBlynkModeNode2(self, blynkModeNode2):
		self.blynkModeNode2 = blynkModeNode2
		print("pass", self.blynkModeNode2)
		
	def setBlynkTimeNormalNode2(self, blynkTimeNormalNode2):
		self.blynkTimeNormalNode2 = blynkTimeNormalNode2
		print("pass", self.blynkModeNode2)
		
	def setBlynkTimeDebugNode2(self, blynkTimeDebugNode2):
		self.blynkTimeDebugNode2 = blynkTimeDebugNode2
		print("pass", self.blynkTimeDebugNode2)
		
	def on_rx_done(self):
		#print("\nRxDone")
		self.clear_irq_flags(RxDone=1)
		payload = self.read_payload(nocheck=True)
		print("Received:", payload)
		if payload is not None and len(payload) >= 3:  # ตรวจสอบว่า payload มีข้อมูลและมีอย่างน้อย 3 องค์ประกอบ
			#self.received_data = bytes(payload).decode("utf-8", 'ignore')
			self.localAddress_rx = payload[0]
			self.destination_rx = payload[1]
			self.data_length_rx = payload[2]
			payload_without_first_three = payload[3:]
			# แปลงเป็นสตริง
			self.received_data  = ''.join([chr(num) for num in payload_without_first_three])
			print("Received (as string):", self.received_data)
			self.stateRx = True
			blynk.sync_virtual(6,7,8,16,17,18)
	def send_data(self, node):
		if node == 1:
			sleep(2)
			self.checkstateNode1 = 1
			print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Node 1 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
			data = str(self.confirmNode1)+","+str(self.modeNode1)+","+str(self.TimeNormalNode1)+","+str(self.TimeDebugNode1)+","+str(self.checkstateNode1)
			print("Sending data:", data)
			data_length = len(data)
			data_to_send = str(int(self.destination_Node1,16)) + " " + str(int(self.localAddress_Gateway,16)) + " " + str(data_length) + " " + data
			print(data_to_send)
			print(data_to_send.split())
			self.write_payload(data_to_send)
			self.set_mode(MODE.TX)
			self.clear_irq_flags(TxDone=1)
			sleep(0.5) 
			self.set_mode(MODE.STDBY)
			self.set_mode(MODE.RXCONT)
			self.stateNode1 = self.checkstateNode1 
			blynk.virtual_write(5,self.stateNode1)
			self.previousMillisNode1 = int(round(time.time() * 1000)) 
		elif node == 2:
			sleep(2)
			self.checkstateNode2 = 1
			print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Node 2 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
			data = str(self.confirmNode2)+","+str(self.modeNode2)+","+str(self.TimeNormalNode2)+","+str(self.TimeDebugNode2)+","+str(self.checkstateNode2)
			print("Sending data:", data)
			data_length = len(data)
			data_to_send = str(int(self.destination_Node2,16)) + " " + str(int(self.localAddress_Gateway,16)) + " " + str(data_length) + " " + data
			print(data_to_send)
			print(data_to_send.split())
			self.write_payload(data_to_send)
			self.set_mode(MODE.TX)
			self.clear_irq_flags(TxDone=1)
			sleep(0.5) 
			self.set_mode(MODE.STDBY)
			self.set_mode(MODE.RXCONT)
			self.stateNode2 = self.checkstateNode2 
			blynk.virtual_write(15,self.stateNode2)
			self.previousMillisNode2 = int(round(time.time() * 1000)) 
		elif node == 3:
			print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Node 3 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
			sleep(2)
			#pumpModeNode3...modeNode3....pumpStateNode3....statusNode3....TimeNormalNode3....TimeDebugNode3
			data = str(self.confirmNode3)+","+str(self.pumpModeNode3)+","+str(self.modeNode3)+","+str(self.pumpStateNode3)+","+str(self.statusNode3)+","+str(self.TimeNormalNode3)+","+str(self.TimeDebugNode3)
			print("Sending data Node3:", data)
			data_length = len(data)
			data_to_send = str(int(self.destination_Node3,16)) + " " + str(int(self.localAddress_Gateway,16)) + " " + str(data_length) + " " + data
			print(data_to_send.split())
			self.write_payload(data_to_send)
			self.set_mode(MODE.TX)
			self.clear_irq_flags(TxDone=1)
			sleep(0.5) 
			self.set_mode(MODE.STDBY)
			self.set_mode(MODE.RXCONT)
			self.statusNode3 = self.checkstateNode3
			blynk.virtual_write(27,self.statusNode3)
			self.previousMillisNode3 = int(round(time.time() * 1000)) 
		
	def checkDataAndUpdate(self):
		if hex(self.localAddress_rx) == self.localAddress_Gateway and hex(self.destination_rx) == self.destination_Node1:
			current_time = datetime.datetime.now()
			print("***********************************************************")
			print("**************************Node 1***************************")
			print("*** destination  :" ,hex(self.destination_rx))
			print("*** localAddress :" ,hex(self.localAddress_rx))
			print("*** data         :" ,self.received_data)
			print("***********************************************************")
			print("***********************************************************")
			data_list = self.received_data.split(',')
			rxWaterLevelNode1 = float(data_list[0])
			rxTempNode1 = float(data_list[1])
			rxHumNode1 = float(data_list[2])
			rxBattNode1 = float(data_list[3])
			rxStateNode1 = int(data_list[4])
			rxModeNode1 = int(data_list[5])
			rxTimeNormalNode1 = float(data_list[6])
			rxTimeDebugNode1 = float(data_list[7])
			
			print(">>>>>>>>>>>>>>>>>>>>>ON<<<<<<<<<<<<<<<<<<<<<<<< ")
			print("blynkModeNode1 "+ str(self.blynkModeNode1))
			print("blynkTimeNormalNode1 "+str(self.blynkTimeNormalNode1))
			print("blynkTimeDebugNode1 "+str(self.blynkTimeDebugNode1))
			#	print("ssss")
			if rxModeNode1 == self.modeNode1 and rxTimeNormalNode1 == self.TimeNormalNode1 and rxTimeDebugNode1 == self.TimeDebugNode1:
				if self.blynkModeNode1 == self.modeNode1 and self.blynkTimeNormalNode1 == self.TimeNormalNode1 and self.blynkTimeDebugNode1 == self.TimeDebugNode1:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 1 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode1 = rxWaterLevelNode1
					self.tempNode1 = rxTempNode1
					self.humNode1 = rxHumNode1
					self.battNode1 = rxBattNode1
					self.stateNode1 = rxStateNode1
					self.modeNode1 = rxModeNode1
					self.TimeNormalNode1 = rxTimeNormalNode1
					self.TimeDebugNode1 = rxTimeDebugNode1 
					print("send "+str(self.waterLevelNode1)+","+str(self.tempNode1)+","+str(self.humNode1)+","+str(self.battNode1)+","+str(self.stateNode1)+","+str(self.modeNode1)+","+str(self.TimeNormalNode1)+","+str(self.TimeDebugNode1))
					
					blynk.virtual_write(1,self.waterLevelNode1 )
					blynk.virtual_write(2,self.tempNode1 )
					blynk.virtual_write(3,self.humNode1 )
					blynk.virtual_write(4,self.battNode1 )
					blynk.virtual_write(5,self.stateNode1 )
					blynk.virtual_write(6,self.modeNode1 )
					blynk.virtual_write(7,self.TimeNormalNode1 )
					blynk.virtual_write(8,self.TimeDebugNode1 )
					stateBlynkRx = False
				else:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 2 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode1 = rxWaterLevelNode1
					self.tempNode1 = rxTempNode1
					self.humNode1 = rxHumNode1
					self.battNode1 = rxBattNode1
					self.stateNode1 = rxStateNode1
					self.modeNode1 = self.blynkModeNode1
					self.TimeNormalNode1 = self.blynkTimeNormalNode1
					self.TimeDebugNode1 = self.blynkTimeDebugNode1 
					print("send "+str(self.waterLevelNode1)+","+str(self.tempNode1)+","+str(self.humNode1)+","+str(self.battNode1)+","+str(self.stateNode1)+","+str(self.modeNode1)+","+str(self.TimeNormalNode1)+","+str(self.TimeDebugNode1))
					
					blynk.virtual_write(1,self.waterLevelNode1 )
					blynk.virtual_write(2,self.tempNode1 )
					blynk.virtual_write(3,self.humNode1 )
					blynk.virtual_write(4,self.battNode1 )
					blynk.virtual_write(5,self.stateNode1 )
					blynk.virtual_write(6,self.modeNode1 )
					blynk.virtual_write(7,self.TimeNormalNode1 )
					blynk.virtual_write(8,self.TimeDebugNode1 )
					stateBlynkRx = False
			else:
				if self.blynkModeNode1 == self.modeNode1 and self.blynkTimeNormalNode1 == self.TimeNormalNode1 and self.blynkTimeDebugNode1 == self.TimeDebugNode1:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 3 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode1 = rxWaterLevelNode1
					self.tempNode1 = rxTempNode1
					self.humNode1 = rxHumNode1
					self.battNode1 = rxBattNode1
					self.stateNode1 = rxStateNode1
					self.modeNode1 = rxModeNode1
					self.TimeNormalNode1 = rxTimeNormalNode1
					self.TimeDebugNode1 = rxTimeDebugNode1 
					print("send "+str(self.waterLevelNode1)+","+str(self.tempNode1)+","+str(self.humNode1)+","+str(self.battNode1)+","+str(self.stateNode1)+","+str(self.modeNode1)+","+str(self.TimeNormalNode1)+","+str(self.TimeDebugNode1))
					
					blynk.virtual_write(1,self.waterLevelNode1 )
					blynk.virtual_write(2,self.tempNode1 )
					blynk.virtual_write(3,self.humNode1 )
					blynk.virtual_write(4,self.battNode1 )
					blynk.virtual_write(5,self.stateNode1 )
					blynk.virtual_write(6,self.modeNode1 )
					blynk.virtual_write(7,self.TimeNormalNode1 )
					blynk.virtual_write(8,self.TimeDebugNode1 )
					stateBlynkRx = False
				else:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 4 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode1 = rxWaterLevelNode1
					self.tempNode1 = rxTempNode1
					self.humNode1 = rxHumNode1
					self.battNode1 = rxBattNode1
					self.stateNode1 = rxStateNode1
					self.modeNode1 = rxModeNode1
					self.TimeNormalNode1 = rxTimeNormalNode1
					self.TimeDebugNode1 = rxTimeDebugNode1
					print("send "+str(self.waterLevelNode1)+","+str(self.tempNode1)+","+str(self.humNode1)+","+str(self.battNode1)+","+str(self.stateNode1)+","+str(self.modeNode1)+","+str(self.TimeNormalNode1)+","+str(self.TimeDebugNode1))
					
					blynk.virtual_write(1,self.waterLevelNode1 )
					blynk.virtual_write(2,self.tempNode1 )
					blynk.virtual_write(3,self.humNode1 )
					blynk.virtual_write(4,self.battNode1 )
					blynk.virtual_write(5,self.stateNode1 )
					blynk.virtual_write(6,self.modeNode1 )
					blynk.virtual_write(7,self.TimeNormalNode1 )
					blynk.virtual_write(8,self.TimeDebugNode1 )
					stateBlynkRx = False		
			current_data = "Time"+str(current_time)+" Data "+self.received_data 
			#print(current_data)
			#with open("logfile.txt", "a") as log_file:
			#	log_file.write(current_data + "\n")
			sleep(0.1)
			self.checkDataNode = 1
		elif hex(self.localAddress_rx) == self.localAddress_Gateway and hex(self.destination_rx) == self.destination_Node2:
			current_time = datetime.datetime.now()
			print("***********************************************************")
			print("**************************Node 2***************************")
			print("*** destination  :" ,hex(self.destination_rx))
			print("*** localAddress :" ,hex(self.localAddress_rx))
			print("*** data         :" ,self.received_data)
			print("***********************************************************")
			print("***********************************************************")
			data_list = self.received_data.split(',')
			rxWaterLevelNode2 = float(data_list[0])
			rxTempNode2 = float(data_list[1])
			rxHumNode2 = float(data_list[2])
			rxBattNode2 = float(data_list[3])
			rxStateNode2 = int(data_list[4])
			rxModeNode2 = int(data_list[5])
			rxTimeNormalNode2 = float(data_list[6])
			rxTimeDebugNode2 = float(data_list[7])
			
			print(">>>>>>>>>>>>>>>>>>>>>ON<<<<<<<<<<<<<<<<<<<<<<<< ")
			print("blynkModeNode2 "+ str(self.blynkModeNode2))
			print("blynkTimeNormalNode2 "+str(self.blynkTimeNormalNode2))
			print("blynkTimeDebugNode2 "+str(self.blynkTimeDebugNode2))
			#	print("ssss")
			if rxModeNode2 == self.modeNode2 and rxTimeNormalNode2 == self.TimeNormalNode2 and rxTimeDebugNode2 == self.TimeDebugNode2:
				if self.blynkModeNode2 == self.modeNode2 and self.blynkTimeNormalNode2 == self.TimeNormalNode2 and self.blynkTimeDebugNode2 == self.TimeDebugNode2:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 1 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode2 = rxWaterLevelNode2
					self.tempNode2 = rxTempNode2
					self.humNode2 = rxHumNode2
					self.battNode2 = rxBattNode2
					self.stateNode2 = rxStateNode2
					self.modeNode2 = rxModeNode2
					self.TimeNormalNode2 = rxTimeNormalNode2
					self.TimeDebugNode2 = rxTimeDebugNode2 
					print("send "+str(self.waterLevelNode2)+","+str(self.tempNode2)+","+str(self.humNode2)+","+str(self.battNode2)+","+str(self.stateNode2)+","+str(self.modeNode2)+","+str(self.TimeNormalNode2)+","+str(self.TimeDebugNode2))
					
					blynk.virtual_write(11,self.waterLevelNode2)
					blynk.virtual_write(12,self.tempNode2)
					blynk.virtual_write(13,self.humNode2)
					blynk.virtual_write(14,self.battNode2)
					blynk.virtual_write(15,self.stateNode2)
					blynk.virtual_write(16,self.modeNode2)
					blynk.virtual_write(17,self.TimeNormalNode2)
					blynk.virtual_write(18,self.TimeDebugNode2)
					stateBlynkRx = False
				else:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 2 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode2 = rxWaterLevelNode2
					self.tempNode2 = rxTempNode2
					self.humNode2 = rxHumNode2
					self.battNode2 = rxBattNode2
					self.stateNode2 = rxStateNode2
					self.modeNode2 = self.blynkModeNode2
					self.TimeNormalNode2 = self.blynkTimeNormalNode2
					self.TimeDebugNode2 = self.blynkTimeDebugNode2 
					print("send "+str(self.waterLevelNode2)+","+str(self.tempNode2)+","+str(self.humNode2)+","+str(self.battNode2)+","+str(self.stateNode2)+","+str(self.modeNode2)+","+str(self.TimeNormalNode2)+","+str(self.TimeDebugNode2))
					
					blynk.virtual_write(11,self.waterLevelNode2)
					blynk.virtual_write(12,self.tempNode2)
					blynk.virtual_write(13,self.humNode2)
					blynk.virtual_write(14,self.battNode2)
					blynk.virtual_write(15,self.stateNode2)
					blynk.virtual_write(16,self.modeNode2)
					blynk.virtual_write(17,self.TimeNormalNode2)
					blynk.virtual_write(18,self.TimeDebugNode2)
					stateBlynkRx = False
			else:
				if self.blynkModeNode2 == self.modeNode2 and self.blynkTimeNormalNode2 == self.TimeNormalNode2 and self.blynkTimeDebugNode2 == self.TimeDebugNode2:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 3 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode2 = rxWaterLevelNode2
					self.tempNode2 = rxTempNode2
					self.humNode2 = rxHumNode2
					self.battNode2 = rxBattNode2
					self.stateNode2 = rxStateNode2
					self.modeNode2 = rxModeNode2
					self.TimeNormalNode2 = rxTimeNormalNode2
					self.TimeDebugNode2 = rxTimeDebugNode2 
					print("send "+str(self.waterLevelNode2)+","+str(self.tempNode2)+","+str(self.humNode2)+","+str(self.battNode2)+","+str(self.stateNode2)+","+str(self.modeNode2)+","+str(self.TimeNormalNode2)+","+str(self.TimeDebugNode2))
					
					blynk.virtual_write(11,self.waterLevelNode2)
					blynk.virtual_write(12,self.tempNode2)
					blynk.virtual_write(13,self.humNode2)
					blynk.virtual_write(14,self.battNode2)
					blynk.virtual_write(15,self.stateNode2)
					blynk.virtual_write(16,self.modeNode2)
					blynk.virtual_write(17,self.TimeNormalNode2)
					blynk.virtual_write(18,self.TimeDebugNode2)
					stateBlynkRx = False
				else:
					print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Case 4 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					self.waterLevelNode2 = rxWaterLevelNode2
					self.tempNode2 = rxTempNode2
					self.humNode2 = rxHumNode2
					self.battNode2 = rxBattNode2
					self.stateNode2 = rxStateNode2
					self.modeNode2 = rxModeNode2
					self.TimeNormalNode2 = rxTimeNormalNode2
					self.TimeDebugNode2 = rxTimeDebugNode2
					print("send "+str(self.waterLevelNode2)+","+str(self.tempNode2)+","+str(self.humNode2)+","+str(self.battNode2)+","+str(self.stateNode2)+","+str(self.modeNode2)+","+str(self.TimeNormalNode2)+","+str(self.TimeDebugNode2))
					
					blynk.virtual_write(11,self.waterLevelNode2)
					blynk.virtual_write(12,self.tempNode2)
					blynk.virtual_write(13,self.humNode2)
					blynk.virtual_write(14,self.battNode2)
					blynk.virtual_write(15,self.stateNode2)
					blynk.virtual_write(16,self.modeNode2)
					blynk.virtual_write(17,self.TimeNormalNode2)
					blynk.virtual_write(18,self.TimeDebugNode2)
					stateBlynkRx = False		
			current_data = "Time"+str(current_time)+" Data "+self.received_data 
			#print(current_data)
			#with open("logfile.txt", "a") as log_file:
			#	log_file.write(current_data + "\n")
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
			state = "ONE"
	def myDataa(self):
		#bme280_data = bme280.sample(bus,address)
		#temp = bme280_data.temperature
		print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	def setupMode(self):
		blynk.run()
		rssi_value = self.get_rssi_value()
		status = self.get_modem_status()
		sys.stdout.flush()
	def checkStatusNode(self):
		##################################################### Node1 ################################################################################
		if self.modeNode1 == 1:
			currentMillisNode1 = int(round(time.time() * 1000)) # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisNode1 - self.previousMillisNode1 >= self.TimeNormalNode1 * 60 * 1000 * 1.5:     #self.TimeNormalNode1*60*
				if(self.checkstateNode1 == 0):  #update ค่า ขึ้นblynk
					self.checkstateNode1 = 0
					self.stateNode1 = self.checkstateNode1
					blynk.virtual_write(5,self.stateNode1)
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode1                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode1)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode1 = 0
					self.stateNode1 = self.checkstateNode1
					blynk.virtual_write(5,self.stateNode1)
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode1                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode1)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisSw เมื่อทำงานเสร็จสิ้น
				self.previousMillisNode1 = currentMillisNode1
		else:
			currentMillisNode1 = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisNode1 - self.previousMillisNode1 >= self.TimeDebugNode1 * 60 * 1000 * 1.5: #self.TimeDebugNode1*60*
				if(self.checkstateNode1 == 0):  #update ค่า ขึ้นblynk
					self.checkstateNode1 = 0
					self.stateNode1 = self.checkstateNode1
					blynk.virtual_write(5,self.stateNode1)
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode1                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode1)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode1 = 0
					self.stateNode1 = self.checkstateNode1
					blynk.virtual_write(5,self.stateNode1)
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode1                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode1)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisSw เมื่อทำงานเสร็จสิ้น
				self.previousMillisNode1 = currentMillisNode1
		##################################################### Node2 ################################################################################
		if self.modeNode2 == 1:
			currentMillisNode2 = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisNode2 - self.previousMillisNode2 >= self.TimeNormalNode2*60*1000 * 1.5:     #self.TimeNormalNode2*60*
				if(self.checkstateNode2 == 0):  #update ค่า ขึ้นblynk
					self.checkstateNode2 = 0
					self.stateNode2 = self.checkstateNode2
					blynk.virtual_write(15,self.stateNode2)
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode2 = 0
					self.stateNode2 = self.checkstateNode2
					blynk.virtual_write(15,self.stateNode2)
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisNode2 เมื่อทำงานเสร็จสิ้น
				self.previousMillisNode2 = currentMillisNode2
		else:
			currentMillisNode2 = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisNode2 - self.previousMillisNode2 >= self.TimeDebugNode2*60*1000 * 1.5: #self.TimeDebugNode2*60*
				if(self.checkstateNode2 == 0):  #update ค่า ขึ้นblynk
					self.checkstateNode2 = 0
					self.stateNode2 = self.checkstateNode2
					blynk.virtual_write(15,self.stateNode2)
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode2 = 0
					self.stateNode2 = self.checkstateNode2
					blynk.virtual_write(15,self.stateNode2)
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode2"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisNode2 เมื่อทำงานเสร็จสิ้น
				self.previousMillisNode2 = currentMillisNode2
		##################################################### Node3 ################################################################################		
		if self.modeNode3 == 1:
			currentMillisNode3 = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisNode3 - self.previousMillisNode3 >= self.TimeNormalNode3*60*1000 * 1.5:     #self.TimeNormalNode3*60*
				if(self.checkstateNode3 == 0):  #update ค่า ขึ้นblynk
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode3"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode3 = 0
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode3"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisNode3 เมื่อทำงานเสร็จสิ้น
				self.previousMillisNode3 = currentMillisNode3
		else:
			currentMillisNode3 = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisNode3 - self.previousMillisNode3 >= self.TimeDebugNode3*60*1000 * 1.5: #self.TimeDebugNode3*60*
				if(self.checkstateNode3 == 0):  #update ค่า ขึ้นblynk
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode3 = 0
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisNode3 เมื่อทำงานเสร็จสิ้น
				self.previousMillisNode3 = currentMillisNode3
		# ให้รอสักเล็กน้อยเพื่อไม่ให้ลูปทำงานเร็วเกินไป
		time.sleep(0.01)
	def start(self):
		self.reset_ptr_rx()
		self.set_mode(MODE.RXCONT)
		state = "ONE"  # กำหนด initial state เป็น "WAITING"
		while True:
			self.setupMode()
			self.checkStatusNode()
			if state == "ONE":  #อ่านค่าเซ็นเซอร์
				self.setupMode()
				print(state)
				state = "TWO"
			elif state == "TWO": ##นำค่าที่ได้รับมาจากNode มาตรวจสอบ และอัพเดทข้อมูลไปblynk และยืนยันกลับไปที่ node
				self.setupMode()
				print(state)
				if self.stateRx == True:
					self.checkDataAndUpdate()
				#else:
				#	self.checkstateNode1 = 0
				state = "THREE"
			elif state == "THREE": 
			
				state = "FOUR"
			elif state == "FOUR":               
				self.setupMode()
				print(state)
				if self.checkDataNode == 1 or self.checkDataNode == 2 or self.checkDataNode == 3:
					self.send_data(self.checkDataNode)
					self.stateRx = False
					self.checkDataNode = 0
				state = "ONE"
			# กระบวนการที่ต้องทำในทุก iteration
# กำหนดค่าต่างๆ และเริ่มต้น LoRa
lora = LoRaGateway(verbose=True)
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
