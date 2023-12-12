from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD

BOARD.setup()

class LoRaRcvCont(LoRa):
    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)
    
    def send_data(self, data):
        global count
        print("Sending data : ", data+str(count))
        self.write_payload(data+str(count))
        count = count + 1
        self.set_mode(MODE.TX)
        
    def start(self):
        while True:
            self.send_data(data_to_send)
            sleep(1)
            sys.stdout.flush()
    
    def start2(self):
        self.send_data(data_to_send)
        
            
    def on_tx_done(self):
        print("tx done")
        self.clear_irq_flags(TxDone=1)
        print("Send success")
        self.set_mode(MODE.SLEEP)
        self.reset_ptr_rx()
        sleep(0.1)
        self.set_mode(MODE.STDBY)

lora = LoRaRcvCont(verbose=False)
lora.set_mode(MODE.STDBY)

#  Medium Range  Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on 13 dBm

lora.set_pa_config(pa_select=1)
lora.set_freq(923.0)
count = 0
data_to_send = "Hello CPE "
try:
    lora.start2()
except KeyboardInterrupt:
    sys.stdout.flush()
    print("")
    sys.stderr.write("KeyboardInterrupt\n")
finally:
    sys.stdout.flush()
    print("")
    lora.set_mode(MODE.SLEEP)
    BOARD.teardown()
