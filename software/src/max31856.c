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

#include "configs/config_max31856.h"
#include "bricklib2/logging/logging.h"

#include "xmc_gpio.h"
#include "xmc_usic.h"
#include "xmc_spi.h"
#include "max31856.h"

/*
 * Default CR0 value (144):
 *
 * BIT-7     BIT-6     BIT-5     BIT-4   BIT-3     BIT-2     BIT-1     BIT-0
 * ========  ========  ================  ========  ========  ========  ========
 * CMODE     1SHOT     OCFAULT           CJ        FAULT     FLTCLEAR  FILTER
 * 1         0         0         1       0         0         0         0
 *
 * Default CR1 value (35):
 *
 * BIT-7     BIT-6     BIT-5     BIT-4     BIT-3     BIT-2     BIT-1     BIT-0
 * ========  ============================  ======================================
 * RESERVED  AVGSEL                        TC TYPE
 * 0         0         1         0         0         0         1         1
 *
 */
#define DEFAULT_CR0_VALUE 144
#define DEFAULT_CR1_VALUE 35

MAX31856_t max31856 = {
  .temperature = 0,
  .config_reg_cr0 = DEFAULT_CR0_VALUE,
  .config_reg_cr1 = DEFAULT_CR1_VALUE,
  .config_averaging = MAX31856_CONFIG_AVERAGING_4,
  .config_thermocouple_type = MAX31856_CONFIG_TYPE_K,
  .config_filter = MAX31856_CONFIG_FILTER_60HZ,
  .error_state_changed = false,
  .error_state_over_current = false,
  .error_state_open_circuit = false,
  .rx = {0, 0, 0, 0, 0, 0, 0, 0},
  .tx = {0, 0, 0, 0, 0, 0, 0, 0},
  .index = 0
};

static void drain_rx_buffer() {
  if ((XMC_USIC_CH_GetReceiveBufferStatus(MAX31856_USIC) & XMC_USIC_CH_RBUF_STATUS_DATA_VALID0)) {
    XMC_SPI_CH_GetReceivedData(MAX31856_USIC);
  }

  if ((XMC_USIC_CH_GetReceiveBufferStatus(MAX31856_USIC) & XMC_USIC_CH_RBUF_STATUS_DATA_VALID1)) {
    XMC_SPI_CH_GetReceivedData(MAX31856_USIC);
  }
}

void max31856_init() {
  logd("[+] Thermocouple V2 Bricklet : max31856_init()\n\r");

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
  max31856.tx[0] = 254; // Value to be written to the register.
  max31856_spi_write_register(MAX31856_REG_MASK, 1);

  // Write to CR0 register.
  max31856.tx[0] = max31856.config_reg_cr0; // Value to be written to the register.
  max31856_spi_write_register(MAX31856_REG_CR0, 1);

  // Write to CR1 register.
  max31856.tx[0] = max31856.config_reg_cr1; // Value to be written to the register.
  max31856_spi_write_register(MAX31856_REG_CR1, 1);
}

void max31856_tick() {
  max31856.temperature++;
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
  while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U)
    ;

  XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC, XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);

  // Drain RX buffer.
  drain_rx_buffer();

  max31856.index = 0;

  while(max31856.index < data_length)
  {
    // Receive one byte.
    XMC_SPI_CH_Receive(MAX31856_USIC, XMC_SPI_CH_MODE_STANDARD);

    // Wait for the RX to finish.
    while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U)
      ;

    while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION) == 0U)
      ;

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
  while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U)
    ;

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
    while((XMC_SPI_CH_GetStatusFlag(MAX31856_USIC) & XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION) == 0U)
      ;

    XMC_SPI_CH_ClearStatusFlag(MAX31856_USIC, XMC_SPI_CH_STATUS_FLAG_TRANSMIT_SHIFT_INDICATION);

    // Drain RX buffer.
    drain_rx_buffer();
  }

  // Disable slave select line.
  XMC_SPI_CH_DisableSlaveSelect(MAX31856_USIC);
}

short int max31856_get_temperature() {
  return max31856.temperature;
}
