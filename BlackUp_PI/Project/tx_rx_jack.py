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
blynkModeNode1 = 0    
blynkTimeNormalNode1 = 0
blynkTimeDebugNode1 = 0

@blynk.on("V6")
def blynk_handle_vpins(value):
    blynkModeNode1 = int(value[0])
    lora.updateBlynkModeNode1(blynkModeNode1)
    print("----------------------------------UPDATE--------------------------------------")
@blynk.on("V7")
def blynk_handle_vpins(value):
    blynkTimeNormalNode1 = float(value[0])
    lora.updateBlynkTimeNormalNode1(blynkTimeNormalNode1)
    print("----------------------------------UPDATE--------------------------------------")
@blynk.on("V8")
def blynk_handle_vpins(value):
    blynkTimeDebugNode1 = float(value[0])
    lora.updateBlynkTimeDebugNode1(blynkTimeDebugNode1)
    print("----------------------------------UPDATE--------------------------------------")
    
# Led control through V0 virtual pin
#function to sync the data from virtual pins
@blynk.on("connected")
def blynk_connected():
    print("Raspberry Pi Connected to New Blynk")
##########################################################
def myData(self):
	#bme280_data = bme280.sample(bus,address)
	#temp = bme280_data.temperature
	
	print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	print(self.localAddress_Gateway)
timer.set_interval(10,myData)
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
		self.count = 0
		self.stateBlynkRx = False
		self.previousMillisSw = int(round(time.time() * 1000))  # ใช้ time.time() เพื่อดึงเวลาปัจจุบัน (ในวินาที) และคูณด้วย 1000 เพื่อแปลงเป็นมิลลิวินาที
		#//////////////////////Node 1/////////////////////////
		self.destination_Node1 = hex(0xa1)
		self.confirmNode1 = "Pass"
		self.waterLevelNode1 = 0
		self.tempNode1 = 0
		self.humNode1 = 0
		self.battNode1 = 0
		self.stateNode1 = 0
		self.modeNode1 = 0    # 0 = DebugMode , 1 = NormalMode
		self.TimeNormalNode1 = 5
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
		self.TimeNormalNode2 = 5
		self.TimeDebugNode2 = 1
		self.checkstateNode2 = 0
		#//////////////////////Node 3/////////////////////////
		self.destination_Node3 = hex(0xc1)
		self.confirmNode3 = "Pass"
		self.pumpState = 0    # 0 = OFF       , 1 = ON
		self.pumpMode = 0	  # 0 = Manual    , 1 = Auto
		self.modeNode3 = 0    # 0 = DebugMode , 1 = NormalMode
		self.TimeNormalNode3 = 5
		self.TimeDebugNode3 = 1
		self.checkstateNode3 = 0
		#/////////////////////////////////////////////////////
	def updateBlynkModeNode1(self, value):
		self.blynkModeNode1 = value
		print("blynkModeNode1 = "+str(self.blynkModeNode1))
		self.stateBlynkRx = True
	def updateBlynkTimeNormalNode1(self, value):
		self.blynkTimeNormalNode1 = value
		print("blynkTimeNormalNode1 = "+str(self.blynkTimeNormalNode1))
		self.stateBlynkRx = True
	def updateBlynkTimeDebugNode1(self, value):
		self.blynkTimeDebugNode1 = value
		print("blynkTimeDebugNode1 = "+str(self.blynkTimeDebugNode1))
		self.stateBlynkRx = True
		
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
	def send_data(self, node):
		if node == 1:
			sleep(2)
			self.checkstateNode1 = 1
			
			print(">>>>>>>>>>>>>>> Send Node 1 <<<<<<<<<<<<<<<<<<")
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
		elif node == 2:
			sleep(2) 
			print(">>>>>>>>>>>>>>> Send Node 2 <<<<<<<<<<<<<<<<<<")
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
			sleep(2)
			
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
			
			
			
			while self.stateBlynkRx == True:
			#	print("dddd")
				blynk.sync_virtual(6,7,8)
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
					self.stateBlynkRx = False
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
					self.stateBlynkRx = False
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
					self.stateBlynkRx = False
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
					self.stateBlynkRx = False		
			current_data = "Time"+str(current_time)+" Data "+self.received_data 
			#print(current_data)
			#with open("logfile.txt", "a") as log_file:
			#	log_file.write(current_data + "\n")
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
			currentMillisSw = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisSw - self.previousMillisSw >= self.TimeNormalNode1 * 60 * 1000 * 1.5:     #self.TimeNormalNode1*60*
				# ทำสิ่งที่คุณต้องการทำทุก 500 มิลลิวินาที
				print("Do something every 500 milliseconds")
				if(self.checkstateNode1 == 0):  #update ค่า ขึ้นblynk
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
				self.previousMillisSw = currentMillisSw
		else:
			currentMillisSw = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisSw - self.previousMillisSw >= self.TimeNormalNode1 * 60 * 1000 * 1.5: #self.TimeDebugNode1*60*
				# ทำสิ่งที่คุณต้องการทำทุก 500 มิลลิวินาที
				print("Do something every 500 milliseconds")
				if(self.checkstateNode1 == 0):  #update ค่า ขึ้นblynk
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
				self.previousMillisSw = currentMillisSw
		##################################################### Node2 ################################################################################
		if self.modeNode2 == 1:
			currentMillisSw = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisSw - self.previousMillisSw >= self.TimeNormalNode2*60*1000:     #self.TimeNormalNode2*60*
				# ทำสิ่งที่คุณต้องการทำทุก 500 มิลลิวินาที
				print("Do something every 500 milliseconds")
				if(self.checkstateNode2 == 0):  #update ค่า ขึ้นblynk
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode2 = 0
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisSw เมื่อทำงานเสร็จสิ้น
				self.previousMillisSw = currentMillisSw
		else:
			currentMillisSw = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisSw - self.previousMillisSw >= self.TimeDebugNode2*60*1000: #self.TimeDebugNode2*60*
				# ทำสิ่งที่คุณต้องการทำทุก 500 มิลลิวินาที
				print("Do something every 500 milliseconds")
				if(self.checkstateNode2 == 0):  #update ค่า ขึ้นblynk
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode2 = 0
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode2                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode2)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisSw เมื่อทำงานเสร็จสิ้น
				self.previousMillisSw = currentMillisSw
		##################################################### Node3 ################################################################################		
		if self.modeNode3 == 1:
			currentMillisSw = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisSw - self.previousMillisSw >= self.TimeNormalNode3*60*1000:     #self.TimeNormalNode3*60*
				# ทำสิ่งที่คุณต้องการทำทุก 500 มิลลิวินาที
				print("Do something every 500 milliseconds")
				if(self.checkstateNode3 == 0):  #update ค่า ขึ้นblynk
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode3"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode3 = 0
					print(">>>>>>>>>>>>>>>>>>>>              TimeNormalNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode3"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisSw เมื่อทำงานเสร็จสิ้น
				self.previousMillisSw = currentMillisSw
		else:
			currentMillisSw = int(round(time.time() * 1000))  # ดึงเวลาปัจจุบันในทุกครั้งในลูป
			if currentMillisSw - self.previousMillisSw >= self.TimeDebugNode3*60*1000: #self.TimeDebugNode3*60*
				# ทำสิ่งที่คุณต้องการทำทุก 500 มิลลิวินาที
				print("Do something every 500 milliseconds")
				if(self.checkstateNode3 == 0):  #update ค่า ขึ้นblynk
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                  NO PASS                    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				else:							#update ค่า ขึ้นblynk
					self.checkstateNode3 = 0
					print(">>>>>>>>>>>>>>>>>>>>               TimeDebugNode3                <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>                   PASS                      <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
					print(">>>>>>>>>>>>>>>>>>>>checkstateNode1"+str(self.checkstateNode3)+" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
				# อัปเดตค่า previousMillisSw เมื่อทำงานเสร็จสิ้น
				self.previousMillisSw = currentMillisSw
		# ให้รอสักเล็กน้อยเพื่อไม่ให้ลูปทำงานเร็วเกินไป
		time.sleep(0.01)
	def start(self):
		self.reset_ptr_rx()
		self.set_mode(MODE.RXCONT)
		state = "ONE"  # กำหนด initial state เป็น "WAITING"
		while True:
			#blynk.run()
			#blynk.sync_virtual(6,7,8)
			#print("-----------------------------------------------------")
			#sleep(1)
			# การทำงานตาม State 
			self.setupMode()
			self.checkStatusNode()
			if state == "ONE":  #อ่านค่าเซ็นเซอร์
				self.setupMode()
				print(state)
				state = "TWO"
			elif state == "TWO": ##นำค่าที่ได้รับมาจากNode มาตรวจสอบ และอัพเดทข้อมูลไปblynk
				self.setupMode()
				print(state)
				if self.stateRx == True:
					#blynk.sync_virtual(6,7,8)
					self.checkDataAndUpdate()
				#else:
				#	self.checkstateNode1 = 0
					
				state = "THREE"
			elif state == "THREE": #นำค่าที่ได้รับมาจากNode มาตรวจสอบก่อนยืนยันกลับไปที่ node
			
				state = "FOUR"
			elif state == "FOUR":                #ส่งข้อมูลยืนยันกลับไปที่ node
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
