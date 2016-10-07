


#include "Arduino.h"


#include "main.h"
#include "globals.h"
#include "vc.h"
#include "errorlogger/generic_eeprom_errorlogger.h"
#include "channel_codec/channel_codec.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"

#define LEDPIN 13  // LEDPIN is a constant

#define CHANNEL_CODEC_TX_BUFFER_SIZE 32
#define CHANNEL_CODEC_RX_BUFFER_SIZE 32

channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

static char cc_rxBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_RX_BUFFER_SIZE];
static char cc_txBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_TX_BUFFER_SIZE];

bool xSerialCharAvailable(){
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

	void xSerialToRPC(void){
		while (xSerialCharAvailable()) {
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


  pinMode(LEDPIN, OUTPUT); // LED init
  digitalWrite(LEDPIN, 1); // write inversed state back
}

void toggleLED() {
  boolean ledstate = digitalRead(LEDPIN); // get LED state
  ledstate ^= 1;   // toggle LED state using xor
  digitalWrite(LEDPIN, ledstate); // write inversed state back
}

void loop() {
	while(1){
		xSerialToRPC();
#if 1
		for (int i=0;i<3;i++){
			toggleLED();
			delay(100);
		}
		RPC_RESULT  result = RPC_FAILURE;
		digitalWrite(LEDPIN, true);
		result = qtKeyPressed(rpcKeyStatus_pressed);
		if (result == RPC_SUCCESS){
			digitalWrite(LEDPIN, true); // write inversed state back
			Serial.print("SUCCESS");
		}else if (result == RPC_FAILURE){
			Serial.print("FAIL");
			digitalWrite(LEDPIN, false); // write inversed state back
		}
		delay(10);
		qtUpdateMCUADCValues(18,1,1,1);
#endif
	}


}
