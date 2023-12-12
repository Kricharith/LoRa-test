from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD
import time
import serial

BOARD.setup()

class LoRaSender(LoRa):
	def __init__(self, verbose=False):
		super(LoRaSender, self).__init__(verbose)
		self.set_mode(MODE.SLEEP)
		self.set_dio_mapping([0] * 6)
    
	def on_rx_done(self):
		print("\nRxDone")
		self.clear_irq_flags(RxDone=1)
		payload = self.read_payload(nocheck=True)
		print("Received:", payload)
		received_data = bytes(payload).decode("utf-8", 'ignore')
		destination_rx = payload[0]
		localAddress_rx = payload[1]
		data_length_rx = payload[2]
		if hex(destination_rx) == hex(0xbb):
			print("ID Pass")
			print("destination_rx :" ,hex(destination_rx))
			print("data_length_rx :" ,data_length_rx)
			print("data:", received_data)
			sleep(2)
			print("-------------------")
			data_to_send = input("Enter data to send: ")
			self.send_data(data_to_send)
			sleep(0.1)
		else :
			print("")
	def send_data(self, data):
		print("Sending data:", data)
		destination = 0xF1
		localAddress = 0xBB
		data_length = len(data)
		data_to_send = str(destination) + " " + str(localAddress) + " " + str(data_length) + " " + data
		print(data_to_send.split())
		self.write_payload(data_to_send)
		self.set_mode(MODE.TX)
		self.clear_irq_flags(TxDone=1)
		sleep(1) 
		self.set_mode(MODE.STDBY)
		self.set_mode(MODE.RXCONT)
		
	def start(self):
		try:
			while True:
				#data_to_send = input("Enter data to send: ")
				self.send_data("5")
				sleep(3)
		except KeyboardInterrupt:
			pass
		finally:
			BOARD.teardown()
    
	def str(self):
		self.reset_ptr_rx()
		self.set_mode(MODE.RXCONT)
		while True:
			sleep(.5)
			rssi_value = self.get_rssi_value()
			status = self.get_modem_status()
			sys.stdout.flush()
        
# กำหนดค่าต่างๆ และเริ่มต้น LoRa
lora = LoRaSender(verbose=True)
lora.set_mode(MODE.STDBY)
lora.set_pa_config(pa_select=1)
lora.set_freq(923.0)  # ต้องกำหนดค่าตามที่ใช้งาน

# เริ่มต้นโปรแกรม
lora.str()
