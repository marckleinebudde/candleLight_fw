/*
 *
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
 */

#include "can.h"
#include "led_trigger.h"

void led_trigger_activity(struct led_trigger *led_trigger)
{
    if (led_trigger->mode < LED_TRIGGER_MODE_ACTIVITY)
        led_trigger->mode = LED_TRIGGER_MODE_ACTIVITY;
}

void led_trigger_channel_set_mode(can_data_t *channel, enum led_trigger_mode mode)
{
	channel->led_trigger[LED_TRIGGER_TYPE_RX].mode = mode;
	channel->led_trigger[LED_TRIGGER_TYPE_TX].mode = mode;
}
