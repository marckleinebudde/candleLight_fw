#include "board.h"
#include "config.h"
#include "device.h"
#include "gpio.h"
#include "usbd_gs_can.h"

/*
 * LED_RX   PA3
 * LEX_TX   PA4
 * USB_P    PA12
 * USB_N    PA11
 * CAN1_RX  PB8
 * CAN1_TX  PB7
 * CAN1_S   PA15
 * CAN2_RX  PA0
 * CAN2_TX  PB13
 * CAN2_S   PB2
 */

#define LEDRX_GPIO_Port      GPIOA
#define LEDRX_Pin            GPIO_PIN_3
#define LEDRX_Mode           GPIO_MODE_OUTPUT_PP
#define LEDRX_Active_High    1

#define LEDTX_GPIO_Port      GPIOA
#define LEDTX_Pin            GPIO_PIN_4
#define LEDTX_Mode           GPIO_MODE_OUTPUT_PP
#define LEDTX_Active_High    1

static void candlelightFD_setup(USBD_GS_CAN_HandleTypeDef *hcan)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
	    UNUSED(hcan);

	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();

		/* LEDs */
	    HAL_GPIO_WritePin(LEDRX_GPIO_Port, LEDRX_Pin, GPIO_INIT_STATE(LEDRX_Active_High));
	    GPIO_InitStruct.Pin = LEDRX_Pin;
	    GPIO_InitStruct.Mode = LEDRX_Mode;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(LEDRX_GPIO_Port, &GPIO_InitStruct);

	    HAL_GPIO_WritePin(LEDTX_GPIO_Port, LEDTX_Pin, GPIO_INIT_STATE(LEDTX_Active_High));
	    GPIO_InitStruct.Pin = LEDTX_Pin;
	    GPIO_InitStruct.Mode = LEDTX_Mode;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(LEDTX_GPIO_Port, &GPIO_InitStruct);

		/* Setup transceiver silent pin */
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	    GPIO_InitStruct.Pin = GPIO_PIN_15;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	#if NUM_CAN_CHANNEL == 2
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	    GPIO_InitStruct.Pin = GPIO_PIN_2;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	#endif

	    RCC_PeriphCLKInitTypeDef PeriphClkInit = {
	        .PeriphClockSelection = RCC_PERIPHCLK_FDCAN,
	        .FdcanClockSelection  = RCC_FDCANCLKSOURCE_PLL1Q,
	    };

	    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
	    __HAL_RCC_FDCAN_CLK_ENABLE();

	    /* FDCAN1*/
	    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_7;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	#if NUM_CAN_CHANNEL == 2
	    /* FDCAN2*/
	    GPIO_InitStruct.Pin = GPIO_PIN_0;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	#endif
	}

	static void candlelightFD_phy_power_set(can_data_t *channel, bool enable)
	{
	    UNUSED(channel);
	    UNUSED(enable);
	}

	static void candlelightFD_termination_set(can_data_t *channel, enum gs_can_termination_state enable)
	{
	    UNUSED(channel);
	    UNUSED(enable);
	}

	const struct BoardConfig config = {
	    .setup = candlelightFD_setup,
	    .phy_power_set = candlelightFD_phy_power_set,
	    .termination_set = candlelightFD_termination_set,
	    .channels[0] = {
	        .interface = FDCAN1,
	        .leds = {
	            [LED_RX] = { .port = LEDRX_GPIO_Port, .pin = LEDRX_Pin, .active_high = LEDRX_Active_High },
	            [LED_TX] = { .port = LEDTX_GPIO_Port, .pin = LEDTX_Pin, .active_high = LEDTX_Active_High },
	        },
	    },
	#if NUM_CAN_CHANNEL == 2
	    .channels[1] = {
	        .interface = FDCAN2,
	        .leds = {
	            [LED_RX] = { .port = LEDRX_GPIO_Port, .pin = LEDRX_Pin, .active_high = LEDRX_Active_High },
	            [LED_TX] = { .port = LEDTX_GPIO_Port, .pin = LEDTX_Pin, .active_high = LEDTX_Active_High },
	        },
	    },
	#endif
};
