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

#include <string.h>

#include "board.h"
#include "led.h"

#define LED_UPDATE_INTERVAL 10  // number of ticks from HAL_GetTick

#define time_after(a, b) ((int)((b) - (a)) < 0)

#if 0

static uint32_t led_set_sequence_step(led_data_t *leds, uint32_t step_num)
{
	const led_seq_step_t *step = &leds->sequence[step_num];
	leds->sequence_step = step_num;
	led_set(&leds->led_state[LED_RX], step->state & 0x01);
	led_set(&leds->led_state[LED_TX], step->state & 0x02);
	uint32_t delta = 10 * step->time_in_10ms;
	if (delta > INT32_MAX) {
		delta = INT32_MAX;  //clamp
	}
	leds->t_sequence_next = HAL_GetTick() + delta;
	return delta;
}

void led_run_sequence(led_data_t *leds, const led_seq_step_t *sequence, int32_t num_repeat)
{
	leds->mode = LED_MODE_SEQUENCE;
	leds->sequence = sequence;
	leds->seq_num_repeat = num_repeat;
	led_set_sequence_step(leds, 0);
	led_update(leds);
}

static void led_update_sequence(led_data_t *leds)
{
	if (leds->sequence == NULL) {
		return;
	}

	uint32_t now = HAL_GetTick();
	if (SEQ_ISPASSED(now, leds->t_sequence_next)) {

		uint32_t t = led_set_sequence_step(leds, ++leds->sequence_step);

		if (t > 0) { // the saga continues

			leds->t_sequence_next = now + t;

		} else { // end of sequence

			if (leds->seq_num_repeat != 0) {

				if (leds->seq_num_repeat > 0) {
					leds->seq_num_repeat--;
				}

				led_set_sequence_step(leds, 0);

			} else {
				leds->sequence = NULL;
			}
		}
	}
}

#endif

const struct led_sequence_step led_sequence_steps_activity[] = {
	{
		.on = false,
		.duration = 30,
	}, {
		.on = true,
		.duration = 20,
	}, {
		/* sentinel */
	},
};

const struct led_sequence led_sequence_activity = {
	.steps = led_sequence_steps_activity,
	.repeat = 1,
};

static void led_set(struct led *led, bool on)
{
	GPIO_PinState state = on ? GPIO_PIN_SET : GPIO_PIN_RESET;

	if (!led->config->active_high) {
		state = !state;
	}

	HAL_GPIO_WritePin(led->config->port, led->config->pin, state);
}

static void
led_update_sequence(struct led *led, struct led_trigger *trigger, uint32_t now)
{
	const struct led_sequence *sequence = led->sequence;
	const struct led_sequence_step *step = &sequence->steps[led->step];

	if (!time_after(now, led->next_step_time)) {
		return;
	}

	led_set(led, step->on);
	led->step++;

	step = &sequence->steps[led->step];
	if (!step->duration) {
		led->repeat--;
		if (!led->repeat) {
			trigger->mode = LED_TRIGGER_MODE_NORMAL;

			return;
		}

		led->step = 0;
		step = &sequence->steps[led->step];
	}

	led->next_step_time = now + step->duration;
}

static const struct led_trigger led_trigger_none[1] = {
	{
		.mode = LED_TRIGGER_MODE_OFF,
	},
};

static struct led_trigger *
led_update_one_trigger_get(USBD_GS_CAN_HandleTypeDef *hcan,
						   const struct led *led)
{
	const struct BoardLEDConfig *led_config = led->config;
	struct led_trigger *active_trigger = (struct led_trigger *)led_trigger_none;

	for (unsigned int i = 0; i < ARRAY_SIZE(led_config->triggers); i++) {
		const struct BoardLEDTriggerConfig *trigger_config = &led_config->triggers[i];
		struct led_trigger *iter = &hcan->channels[trigger_config->channel].led_trigger[trigger_config->type];

		if (iter->mode >= active_trigger->mode) {
			/* mark previous trigger as done */
			if (active_trigger->mode >= LED_TRIGGER_MODE_ACTIVITY) {
				active_trigger->mode = LED_TRIGGER_MODE_NORMAL;
			}

			active_trigger = iter;
		}
	}

	return active_trigger;
}

static void
led_update_one(USBD_GS_CAN_HandleTypeDef *hcan, struct led *led, uint32_t now)
{
	struct led_trigger *trigger;

	trigger = led_update_one_trigger_get(hcan, led);

	switch (trigger->mode) {
		case LED_TRIGGER_MODE_OFF:
			led_set(led, false);
			break;

		case LED_TRIGGER_MODE_NORMAL:
			led_set(led, true);
			break;

		case LED_TRIGGER_MODE_ACTIVITY:
			led_update_sequence(led, trigger, now);
			break;
	}
}

void led_update(USBD_GS_CAN_HandleTypeDef *hcan)
{
	static uint32_t next_update;
	uint32_t now = HAL_GetTick();

	if (!time_after(now, next_update)) {
		return;
	}
	next_update = now + LED_UPDATE_INTERVAL;

	for (unsigned int i = 0; i < ARRAY_SIZE(hcan->leds); i++) {
		struct led *led = &hcan->leds[i];

		led_update_one(hcan, led, now);
	}
}

void led_init(struct led *led, const struct BoardLEDConfig *config)
{
	led->config = config;
}
