#include"rpc_transmission/server/generated_general/RPC_types.h"
#include "channel_codec/channel_codec_types.h"
#include "serial.h"
RPC_RESULT phyPushDataBuffer(channel_codec_instance_t *instance, const char *buffer, size_t length){

	for (int i=0;i<length;i++){
		xSerialPutChar(buffer[i]);
	}
	return RPC_SUCCESS;
}
