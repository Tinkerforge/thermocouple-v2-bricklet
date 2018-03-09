/* thermocouple-v2-bricklet
 * Copyright (C) 2018 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * config_max31856.c: Configuration for MAX31856 thermocouple sensor
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CONFIG_MAX31856_H
#define CONFIG_MAX31856_H

#include "xmc_gpio.h"

#define MAX31856_SPI_BAUDRATE         400000
#define MAX31856_USIC_CHANNEL         USIC0_CH1
#define MAX31856_USIC                 XMC_SPI0_CH1

#define MAX31856_SCLK_PIN             P0_8
#define MAX31856_SCLK_PIN_AF          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_8_AF_U0C1_SCLKOUT)

#define MAX31856_SELECT_PIN           P0_9
#define MAX31856_SELECT_PIN_AF        (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_9_AF_U0C1_SELO0)

#define MAX31856_MOSI_PIN             P0_7
#define MAX31856_MOSI_PIN_AF          (XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7 | P0_7_AF_U0C1_DOUT0)

#define MAX31856_MISO_PIN             P0_6
#define MAX31856_MISO_INPUT           XMC_USIC_CH_INPUT_DX0
#define MAX31856_MISO_SOURCE          0b010 // DX0C

#endif
