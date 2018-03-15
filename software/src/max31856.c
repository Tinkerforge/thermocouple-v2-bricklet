/* thermocouple-v2-bricklet
 * Copyright (C) 2018 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * max31856.h: Driver for MAX31856 thermocouple sensor
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "max31856.h"

#include "communication.h"
#include "configs/config_max31856.h"
#include "bricklib2/logging/logging.h"

#include "xmc_gpio.h"
#include "xmc_usic.h"
#include "xmc_spi.h"

MAX31856_t max31856;

static void drain_rx_buffer() {
  // Drain RBUF[0] and RBUF[1].
  XMC_SPI_CH_GetReceivedData(MAX31856_USIC);
  XMC_SPI_CH_GetReceivedData(MAX31856_USIC);
}

static void do_update_temperature() {
  max31856_spi_read_register(MAX31856_REG_LTCBH, 3);

  max31856.temperature = (max31856.rx[2] | (max31856.rx[1] << 8) | (max31856.rx[0] << 16)) >> 5;

  // Transfer sign of 19 bit temperature value to 32 bit variable.
  if(max31856.temperature & 0x40000) {
    max31856.temperature |= 0xFFFC0000;
  }

  // For G8 and G32 we return the value as read from the MAX31856.
  if(max31856.config_thermocouple_type != THERMOCOUPLE_V2_TYPE_G8 &&
     max31856.config_thermocouple_type != THERMOCOUPLE_V2_TYPE_G32) {
      // *100/128 for 0.01°C steps.
      max31856.temperature = (max31856.temperature * 100) / 128;
  }
}

static void do_update_error_state() {
  // Save current error state.
  bool _error_state_open_circuit = max31856.error_state_open_circuit;
  bool _error_state_over_under_voltage = max31856.error_state_over_under_voltage;

  // Read fault status register of MAX31856.
  max31856_spi_read_register(MAX31856_REG_SR, 1);

  if((max31856.rx[0] & 1) != 0) {
    max31856.error_state_open_circuit = true;
  }
  else {
    max31856.error_state_open_circuit = false;
  }

  if((max31856.rx[0] & 2) != 0) {
    max31856.error_state_over_under_voltage = true;
  }
  else {
    max31856.error_state_over_under_voltage = false;
  }

  if (_error_state_open_circuit != max31856.error_state_open_circuit ||
      _error_state_over_under_voltage != max31856.error_state_over_under_voltage) {
          max31856.do_error_callback = true;
  }
}

void max31856_init() {
  logd("[+] Thermocouple V2 Bricklet : max31856_init()\n\r");

  // Initialize default configuration.
  max31856.temperature = 0;
  max31856.skip_do_update_temperature_turns = 0;
  max31856.config_reg_cr0 = MAX31856_CR0_CMODE_AUTO | MAX31856_CR0_OCFAULT_1 | MAX31856_CR0_FILTER_50HZ;
  max31856.config_reg_cr1 = MAX31856_CR1_AVGSEL_16 | MAX31856_CR1_TC_TYPE_K;
  max31856.config_averaging = (uint8_t)THERMOCOUPLE_V2_AVERAGING_16;
  max31856.config_thermocouple_type = (uint8_t)THERMOCOUPLE_V2_TYPE_K;
  max31856.config_filter = (uint8_t)THERMOCOUPLE_V2_FILTER_OPTION_50HZ;
  max31856.do_error_callback = false;
  max31856.error_state_open_circuit = false;
  max31856.error_state_over_under_voltage = false;
  max31856.index = 0;

  // Initialize SPI interface to MAX31856.

  // USIC channel configuration.
  const XMC_SPI_CH_CONFIG_t channel_config = {
    .baudrate       = MAX31856_SPI_BAUDRATE,
    .bus_mode       = XMC_SPI_CH_BUS_MODE_MASTER,
    .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
    .parity_mode    = XMC_USIC_CH_PARITY_MODE_NONE
  };

  // SELECT pin configuration.
  const XMC_GPIO_CONFIG_t select_pin_config = {
    .mode             = MAX31856_SELECT_PIN_AF,
    .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
  };

  // SCLK pin configuration.
  const XMC_GPIO_CONFIG_t sclk_pin_config = {
    .mode             = MAX31856_SCLK_PIN_AF,
    .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
  };

  // MOSI pin configuration.
  const XMC_GPIO_CONFIG_t mosi_pin_config = {
    .mode             = MAX31856_MOSI_PIN_AF,
    .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
  };

  // MISO pin configuration.
  const XMC_GPIO_CONFIG_t miso_pin_config = {
    .mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
    .input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
  };

  // Initialize USIC channel in SPI master mode.
  XMC_SPI_CH_Init(MAX31856_USIC, &channel_config);

  // Apply pin configuration.
  XMC_GPIO_Init(MAX31856_SELECT_PIN, &select_pin_config);
  XMC_GPIO_Init(MAX31856_SCLK_PIN, &sclk_pin_config);
  XMC_GPIO_Init(MAX31856_MOSI_PIN, &mosi_pin_config);
  XMC_GPIO_Init(MAX31856_MISO_PIN, &miso_pin_config);

  // MSB first.
  XMC_SPI_CH_SetBitOrderMsbFirst(MAX31856_USIC);
  // Set word length.
  XMC_SPI_CH_SetWordLength(MAX31856_USIC, (uint8_t)8U);
  // Frame is manually ended by deselecting the slave select pin.
  XMC_SPI_CH_SetFrameLength(MAX31856_USIC, (uint8_t)64U);

  // Full-Duplex.
  XMC_SPI_CH_SetTransmitMode(MAX31856_USIC, XMC_SPI_CH_MODE_STANDARD);
  // Set input source path.
  XMC_SPI_CH_SetInputSource(MAX31856_USIC,
                            MAX31856_MISO_INPUT,
                            MAX31856_MISO_SOURCE);

  // Start SPI.
  XMC_SPI_CH_Start(MAX31856_USIC);

  // Do initial configuration of MAX31856.

  // Write to fault mask register.
  max31856.tx[0] = 252; // OV/UV and Open fault mask enabled.
  max31856_spi_write_register(MAX31856_REG_MASK, 1);

  // Write to CR0 and CR1 registers.
  max31856.tx[0] = max31856.config_reg_cr0;
  max31856.tx[1] = max31856.config_reg_cr1;
  max31856_spi_write_register(MAX31856_REG_CR0, 2);
}

void max31856_tick() {
  if (max31856.skip_do_update_temperature_turns > 0) {
    max31856.skip_do_update_temperature_turns--;
  }
  else {
    if(!max31856.error_state_open_circuit) {
      do_update_temperature();
    }
  }

  do_update_error_state();
}

void max31856_spi_read_register(const MAX31856_REG_t register_address,
                                const uint8_t data_length) {
  // Enable slave select line.
  XMC_SPI_CH_EnableSlaveSelect(MAX31856_USIC, XMC_SPI_CH_SLAVE_SELECT_0);

  // Send address write byte.
  XMC_SPI_CH_Transmit(MAX31856_USIC,
                      (uint8_t)(register_address),
                      XMC_SPI_CH_MODE_STANDARD);

  // Wait for the TX to finish.
  while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U) {
    ;
  }

  XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC, XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);

  // Drain RX buffer.
  drain_rx_buffer();

  max31856.index = 0;

  while(max31856.index < data_length)
  {
    // Receive one byte.
    XMC_SPI_CH_Receive(MAX31856_USIC, XMC_SPI_CH_MODE_STANDARD);

    // Wait for the RX to finish.
    while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U) {
      ;
    }

    while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION) == 0U) {
      ;
    }

    max31856.rx[max31856.index++] = (uint8_t)XMC_SPI_CH_GetReceivedData(MAX31856_USIC);

    XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC, XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);
    XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC, XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION);

    // Drain RX buffer.
    drain_rx_buffer();
  }

  // Disable slave select line.
  XMC_SPI_CH_DisableSlaveSelect(MAX31856_USIC);
}

void max31856_spi_write_register(const MAX31856_REG_t register_address,
                                 const uint8_t data_length) {
  // Enable slave select line.
  XMC_SPI_CH_EnableSlaveSelect(MAX31856_USIC, XMC_SPI_CH_SLAVE_SELECT_0);

  // Send address write byte.
  XMC_SPI_CH_Transmit(MAX31856_USIC,
                      (uint8_t)(register_address | BIT_MASK_REG_WRITE),
                      XMC_SPI_CH_MODE_STANDARD);
  // Wait for the TX.
  while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U) {
    ;
  }

  XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC,
                             XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);

  // Drain RX buffer.
  drain_rx_buffer();

  max31856.index = 0;

  while(max31856.index < data_length)
  {
    // Send write data byte.
    XMC_SPI_CH_Transmit(MAX31856_USIC,
                        max31856.tx[max31856.index++],
                        XMC_SPI_CH_MODE_STANDARD);
    // Wait for the TX.
    while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U) {
      ;
    }

    XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC, XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);

    // Drain RX buffer.
    drain_rx_buffer();
  }

  // Disable slave select line.
  XMC_SPI_CH_DisableSlaveSelect(MAX31856_USIC);
}

int32_t max31856_get_temperature() {
  return max31856.temperature;
}
