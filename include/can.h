/*

The MIT License (MIT)

Copyright (c) 2016 Hubert Denkmair

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "gs_usb.h"
#include "hal_include.h"
#include "led.h"
#include "list.h"

typedef struct {
#if defined(STM32G0)
	FDCAN_HandleTypeDef channel;
#else
	CAN_TypeDef *instance;
#endif
	struct list_head list_from_host;
	led_data_t leds;
	uint32_t reg_esr_old;
	uint16_t brp;
	uint8_t phase_seg1;
	uint8_t phase_seg2;
	uint8_t sjw;
	uint8_t nr;
} can_data_t;

extern const struct gs_device_bt_const CAN_btconst;
extern const struct gs_device_bt_const_extended CAN_btconst_ext;

#if defined(STM32G0)
void can_init(can_data_t *channel, FDCAN_GlobalTypeDef *instance);
#else
void can_init(can_data_t *channel, CAN_TypeDef *instance);
#endif
void can_set_bittiming(can_data_t *channel, const struct gs_device_bittiming *timing);

#ifdef CONFIG_CANFD
void can_set_data_bittiming(can_data_t *channel, const struct gs_device_bittiming *timing);
#else
static inline bool can_set_data_bittiming(can_data_t *channel,
										  const struct gs_device_bittiming *timing)
{
	(void)channel;
	(void)timing;

	return false;
}
#endif

void can_enable(can_data_t *channel, uint32_t mode);
void can_disable(can_data_t *channel);
bool can_is_enabled(can_data_t *channel);

bool can_receive(can_data_t *channel, struct gs_host_frame *rx_frame);
bool can_is_rx_pending(can_data_t *channel);

bool can_send(can_data_t *channel, struct gs_host_frame *frame);

/** return CAN->ESR register which contains tx/rx error counters and
 * LEC (last error code).
 */
uint32_t can_get_error_status(can_data_t *channel);

/** parse status value returned by can_get_error_status().
 * @param frame : will hold the generated error frame
 * @param err : holds the contents of the ESR register
 * @return 1 when status changes (if any) need a new error frame sent
 */
bool can_parse_error_status(can_data_t *channel, struct gs_host_frame *frame, uint32_t err);
