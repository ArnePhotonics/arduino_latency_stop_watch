


#include "Arduino.h"


#include "main.h"
#include "globals.h"
#include "vc.h"
#include "errorlogger/generic_eeprom_errorlogger.h"
#include "channel_codec/channel_codec.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"

#define LEDPIN 13  // LEDPIN is a constant

#define CHANNEL_CODEC_TX_BUFFER_SIZE 64
#define CHANNEL_CODEC_RX_BUFFER_SIZE 64


channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

static char cc_rxBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_RX_BUFFER_SIZE];
static char cc_txBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_TX_BUFFER_SIZE];

uint16_t round_times[INPUT_PINS_COUNT_FOR_ROUND_TIME] = {0};
uint8_t old_input_pins = 0;
uint8_t edge_mask_falling = 0;

uint8_t trigger_pin_map = 0;
uint8_t round_time_pin_map = 0;
uint8_t active_pins_for_roundtime_map = 0;

bool xSerialCharAvailable(){
    uint8_t input_pins = PIND & active_pins_for_roundtime_map;
    if (input_pins != old_input_pins){
        for (char i=0; i< INPUT_PINS_COUNT_FOR_ROUND_TIME; i++){
            uint8_t mask = 1<<i;
            if (edge_mask_falling & mask){
                //if rising edge for specific pin
                if(((input_pins & mask) > 0) && ((old_input_pins & mask) == 0)){
                    //rising edge detected
                    round_times[i] = TCNT1;
                }
                    
            }else{
                if(((input_pins & mask) == 0) && ((old_input_pins & mask) > 0)){
                    //falling edge detected
                    round_times[i] = TCNT1;
                }
            }
        }
    }
    old_input_pins = input_pins;
    if(Serial.available()){

		return true;
	}else{
		return false;
	}
}

bool xSerialGetChar(char *data){
	if(Serial.available()){
		*data = Serial.read();
		return true;
	}else{
		return false;
	}
}



#ifdef __cplusplus
extern "C" {
#endif

      void toggleLED() {
	boolean ledstate = digitalRead(LEDPIN); // get LED state
	ledstate ^= 1;   // toggle LED state using xor
	digitalWrite(LEDPIN, ledstate); // write inversed state back
      }

	void xSerialToRPC(void){
		while (xSerialCharAvailable()) {
		      toggleLED();
			// read the incoming byte:
			char inByte = 0;
			xSerialGetChar(&inByte);

			channel_push_byte_to_RPC(&cc_instances[channel_codec_comport_transmission],inByte);
		}
	}



	void xSerialPutChar(uint8_t data){
		Serial.write(data);
	}

	void SET_LED(int ledstatus){
		digitalWrite(LEDPIN, ledstatus); // write inversed state back
		if (ledstatus==30){
			Serial.print("Answer\n");
		}

	}

	void delay_ms(uint32_t sleep_ms){
		delay(sleep_ms);

	}

#ifdef __cplusplus
}
#endif

void ChannelCodec_errorHandler(channel_codec_instance_t *instance,  channelCodecErrorNum_t ErrNum){
	(void)ErrNum;
	(void)instance;

}




void setup() {
	// start serial port at 9600 bps:
  Serial.begin(115200);
  RPC_TRANSMISSION_mutex_init();

  cc_instances[channel_codec_comport_transmission].aux.port = channel_codec_comport_transmission;

  channel_init_instance(&cc_instances[channel_codec_comport_transmission],
		  cc_rxBuffers[channel_codec_comport_transmission],CHANNEL_CODEC_RX_BUFFER_SIZE,
		  cc_txBuffers[channel_codec_comport_transmission],CHANNEL_CODEC_TX_BUFFER_SIZE);


#if 1
  while (!Serial) {
	; // wait for serial port to connect. Needed for native USB port only
  }
#endif

    DDRD = 0x00; // set PORTD to input (D0-D7)
    pinMode(LEDPIN, OUTPUT); // LED init (D13=B5)
    digitalWrite(LEDPIN, 1); // write inversed state back
}



void loop() {
  while(1){
    xSerialToRPC();
  }
}
