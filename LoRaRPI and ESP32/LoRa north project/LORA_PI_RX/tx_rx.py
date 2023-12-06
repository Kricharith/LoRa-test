from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD
import time

BOARD.setup()

class LoRaSender(LoRa):
    def __init__(self, verbose=False):
        super(LoRaSender, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)
    
    def receive_data(self):
        start_time = time.monotonic()
        count = 0
        while True:
            print(time.time())
            self.reset_ptr_rx()
            self.set_mode(MODE.RXCONT)
            if (time.monotonic() - start_time) % 1 == 0:
                count = count + 1
                print(count)
            if (time.monotonic() - start_time) % 5 == 0:
                break
            #sleep(.5)
            rssi_value = self.get_rssi_value()
            status = self.get_modem_status()
            sys.stdout.flush()
            
    def on_rx_done(self):
        pass
        
    def send_data(self, data):
        print("Sending data:", data)
        self.write_payload(data)
        self.set_mode(MODE.TX)
        self.clear_irq_flags(TxDone=1)
        sleep(1)  # รอให้การส่งเสร็จสมบูรณ์
        self.receive_data()
        

    def start(self):
        try:
            while True:
                data_to_send = input("Enter data to send: ")
                self.send_data(data_to_send)
        except KeyboardInterrupt:
            pass
        finally:
            BOARD.teardown()
    
    def str(self):
        self.receive_data()
        
# กำหนดค่าต่างๆ และเริ่มต้น LoRa
lora = LoRaSender(verbose=False)
lora.set_mode(MODE.STDBY)
lora.set_pa_config(pa_select=1)
lora.set_freq(923.0)  # ต้องกำหนดค่าตามที่ใช้งาน

# เริ่มต้นโปรแกรม
lora.str()
