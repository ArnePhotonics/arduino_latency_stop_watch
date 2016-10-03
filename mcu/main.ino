


#include "Arduino.h"


#include "main.h"

#include "errorlogger/generic_eeprom_errorlogger.h"
#include "channel_codec/channel_codec.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"

#define LEDPIN 13  // LEDPIN is a constant

#define CHANNEL_CODEC_TX_BUFFER_SIZE 64
#define CHANNEL_CODEC_RX_BUFFER_SIZE 64

channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

static char cc_rxBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_RX_BUFFER_SIZE];
static char cc_txBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_TX_BUFFER_SIZE];

void ChannelCodec_errorHandler(channel_codec_instance_t *instance,  channelCodecErrorNum_t ErrNum){
	(void)ErrNum;
	(void)instance;

}


void xSerialPutChar(uint8_t data){
	Serial.write(data);
}

void setup() {
	// start serial port at 9600 bps:
  Serial.begin(115200);

  cc_instances[channel_codec_comport_transmission].aux.port = channel_codec_comport_transmission;

  channel_init_instance(&cc_instances[channel_codec_comport_transmission],
		  cc_rxBuffers[channel_codec_comport_transmission],CHANNEL_CODEC_RX_BUFFER_SIZE,
		  cc_txBuffers[channel_codec_comport_transmission],CHANNEL_CODEC_TX_BUFFER_SIZE);


  while (!Serial) {
	; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(LEDPIN, OUTPUT); // LED init
}

void toggleLED() {
  boolean ledstate = digitalRead(LEDPIN); // get LED state
  ledstate ^= 1;   // toggle LED state using xor
  digitalWrite(LEDPIN, ledstate); // write inversed state back
}

void loop() {
	static int i=0;

	uint16_t inByte;




	if (i>1000){
		while (Serial.available() > 0) {
			// read the incoming byte:
			inByte = Serial.read();

			channel_push_byte_to_RPC(&cc_instances[channel_codec_comport_transmission],inByte);
		}

		delay(5);
		i = 0;
	}
	i++;
	qtUpdateMCUADCValues(18,1,1,1);
}
