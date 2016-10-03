


#include "Arduino.h"


#include "main.h"
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

void ChannelCodec_errorHandler(channel_codec_instance_t *instance,  channelCodecErrorNum_t ErrNum){
	(void)ErrNum;
	(void)instance;

}


void xSerialPutChar(uint8_t data){
	static uint16_t counter = 0;
	//data = 0xFF;
//	data = counter;
	Serial.write(data);
	counter++;
}

void setup() {
	// start serial port at 9600 bps:
  Serial.begin(115200);

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
	int i=0;
	uint16_t inByte;
	#if 1
		while (Serial.available() > 0) {
			// read the incoming byte:
			inByte = Serial.read();

			channel_push_byte_to_RPC(&cc_instances[channel_codec_comport_transmission],inByte);
		}
#endif

	delay(1000);
	toggleLED();

	qtUpdateMCUADCValues(18,1,1,1);
}
