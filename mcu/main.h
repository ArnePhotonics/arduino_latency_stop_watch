/*
 * main.h
 *
 *  Created on: 29.11.2015
 *      Author: arne
 */

#ifndef MAIN_H_
#define MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "channel_codec/channel_codec_types.h"

#include <stdint.h>

extern channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

void xSerialPutChar(uint8_t data);

#ifdef __cplusplus
}
#endif
#endif /* MAIN_H_ */
