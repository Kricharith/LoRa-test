from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD

BOARD.setup()

class LoRaRcvCont(LoRa):
    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)

    def start(self):
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)
        while True:
            sleep(.5)
            rssi_value = self.get_rssi_value()
            status = self.get_modem_status()
            sys.stdout.flush()
    
    def send_data(self,payload):
        send_payload = self.write_payload(payload)
        self.set_mode(MODE.TX)
        self.clear_irq_flags(TxDone=1)

        print(bytes(send_payload).decode("utf-8", "ignore"))
        sleep(0.1)
    
    def on_rx_done(self):
        print("\nRxDone")
        self.clear_irq_flags(RxDone=1)
        payload = self.read_payload(nocheck=True)
        received_data = bytes(payload).decode("utf-8", 'ignore')
        print("Received:", received_data)
        
        # ส่งข้อมูลกลับ "CPE"
        self.send_data("CPE")

        # เปลี่ยนเป็น RX mode และเตรียมรับข้อมูลอีกครั้ง
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)
        sys.stdout.flush()


lora = LoRaRcvCont(verbose=False)
lora.set_mode(MODE.STDBY)

lora.set_pa_config(pa_select=1)
lora.set_freq(923.0)
try:
    lora.start()
except KeyboardInterrupt:
    sys.stdout.flush()
    print("")
    sys.stderr.write("KeyboardInterrupt\n")
finally:
    sys.stdout.flush()
    print("")
    lora.set_mode(MODE.SLEEP)
    BOARD.teardown()
