/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Hubert Denkmair
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

enum led_trigger_mode {
	LED_TRIGGER_MODE_OFF,
	LED_TRIGGER_MODE_NORMAL,
	LED_TRIGGER_MODE_ACTIVITY,
};

enum led_trigger_type {
	LED_TRIGGER_TYPE_RX,
	LED_TRIGGER_TYPE_TX,
	LED_TRIGGER_TYPE_MAX,
};

struct led_trigger {
	enum led_trigger_mode mode;
};

struct can_channel;
typedef struct can_channel can_data_t;

void led_trigger_activity(struct led_trigger *led_trigger);
void led_trigger_channel_set_mode(can_data_t *channel, enum led_trigger_mode mode);
