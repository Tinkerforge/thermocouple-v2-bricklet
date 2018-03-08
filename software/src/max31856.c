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
#include "bricklib2/hal/system_timer/system_timer.h"

#include "xmc_gpio.h"
#include "xmc_usic.h"
#include "xmc_spi.h"

#include "max31856.h"

MAX31856_t max31856 = {
  .temperature = 0,
  .config_averaging = MAX31856_CONFIG_AVERAGING_4,
  .config_thermocouple_type = MAX31856_CONFIG_TYPE_K,
  .config_filter = MAX31856_CONFIG_FILTER_60HZ,
  .error_state_changed = false,
  .error_state_over_current = false,
  .error_state_open_circuit = false
};

#define max31856_rx_irq_handler GET_IRQ_HANDLER(MAX31856_IRQ_RX) /*IRQ_Hdlr_11*/
#define max31856_tx_irq_handler GET_IRQ_HANDLER(MAX31856_IRQ_TX) /*IRQ_Hdlr_12*/

void __attribute__((optimize("-O3"))) max31856_rx_irq_handler(void) {
	/*
  while(!XMC_USIC_CH_RXFIFO_IsEmpty(MAX31856_USIC)) {
		rfm69.data_spi[rfm69.data_read_index] = MAX31856_USIC->OUTR;
		rfm69.data_read_index++;
	}
  */
}

void __attribute__((optimize("-O3"))) max31856_tx_irq_handler(void) {
  /*
	while(!XMC_USIC_CH_TXFIFO_IsFull(MAX31856_USIC)) {
		MAX31856_USIC->IN[0] = rfm69.data_spi[rfm69.data_write_index];
		rfm69.data_write_index++;
		if(rfm69.data_write_index >= rfm69.data_length) {
			XMC_USIC_CH_TXFIFO_DisableEvent(MAX31856_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
			return;
		}
	}
  */
}

void max31856_init() {
  logd("[+] max31856_init()\n\r");

  /*
  max31856.temperature = 0;
  max31856.config_averaging = MAX31856_CONFIG_AVERAGING_4;
  max31856.config_thermocouple_type = MAX31856_CONFIG_TYPE_K;
  max31856.config_filter = MAX31856_CONFIG_FILTER_60HZ;
  max31856.error_state_changed = false;
  max31856.error_state_over_current = false;
  max31856.error_state_open_circuit = false;
  */

  // Initialize SPI interface to MAX31856.

  // USIC channel configuration.
  const XMC_SPI_CH_CONFIG_t channel_config = {
    .baudrate       = MAX31856_SPI_BAUDRATE,
    .bus_mode       = XMC_SPI_CH_BUS_MODE_MASTER,
    .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
    .parity_mode    = XMC_USIC_CH_PARITY_MODE_NONE
  };

  // MOSI pin configuration.
  const XMC_GPIO_CONFIG_t mosi_pin_config = {
    .mode             = MAX31856_MOSI_PIN_AF,
    .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
  };

  //MISO pin configuration.
  const XMC_GPIO_CONFIG_t miso_pin_config = {
    .mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
    .input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
  };

  // SCLK pin configuration.
  const XMC_GPIO_CONFIG_t sclk_pin_config = {
    .mode             = MAX31856_SCLK_PIN_AF,
    .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
  };

  // SELECT pin configuration.
  const XMC_GPIO_CONFIG_t select_pin_config = {
    .mode             = MAX31856_SELECT_PIN_AF,
    .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH
  };

  // Configure MISO pin.
  XMC_GPIO_Init(MAX31856_MOSI_PIN, &miso_pin_config);

  // Initialize USIC channel in SPI master mode.
  XMC_SPI_CH_Init(MAX31856_USIC, &channel_config);
  MAX31856_USIC->SCTR &= ~USIC_CH_SCTR_PDL_Msk; // Set passive data level to 0.
  MAX31856_USIC->PCR_SSCMode &= ~USIC_CH_PCR_SSCMode_TIWEN_Msk; // Disable time between bytes.

  XMC_SPI_CH_SetBitOrderMsbFirst(MAX31856_USIC);
  XMC_SPI_CH_SetWordLength(MAX31856_USIC, (uint8_t)8U);
  XMC_SPI_CH_SetFrameLength(MAX31856_USIC, (uint8_t)64U); // Slave must be deselected to end the frame. Won't automatically end the frame.

  XMC_SPI_CH_SetTransmitMode(MAX31856_USIC, XMC_SPI_CH_MODE_STANDARD_HALFDUPLEX /*XMC_SPI_CH_MODE_STANDARD*/);

  // Configure the clock polarity and clock delay.
  XMC_SPI_CH_ConfigureShiftClockOutput(MAX31856_USIC,
                                       XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_1_DELAY_DISABLED,
                                       XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);
  // Configure Leading/Trailing delay.
  XMC_SPI_CH_SetSlaveSelectDelay(MAX31856_USIC, 2);

  // Set input source path.
  XMC_SPI_CH_SetInputSource(MAX31856_USIC, MAX31856_MISO_INPUT, MAX31856_MISO_SOURCE);

  // SPI Mode: CPOL=1 and CPHA=1.
  MAX31856_USIC_CHANNEL->DX1CR |= USIC_CH_DX1CR_DPOL_Msk;

  // Configure transmit FIFO.
  XMC_USIC_CH_TXFIFO_Configure(MAX31856_USIC, 48, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);

  // Configure receive FIFO.
  XMC_USIC_CH_RXFIFO_Configure(MAX31856_USIC, 32, XMC_USIC_CH_FIFO_SIZE_16WORDS, 8);

  // Set service request for TX FIFO transmit interrupt.
  XMC_USIC_CH_TXFIFO_SetInterruptNodePointer(MAX31856_USIC,
                                             XMC_USIC_CH_TXFIFO_INTERRUPT_NODE_POINTER_STANDARD,
                                             MAX31856_SERVICE_REQUEST_TX);  // IRQ MAX31856_IRQ_TX.

  // Set service request for RX FIFO receive interrupt.
  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(MAX31856_USIC,
                                             XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_STANDARD,
                                             MAX31856_SERVICE_REQUEST_RX);  // IRQ MAX31856_IRQ_RX.

  XMC_USIC_CH_RXFIFO_SetInterruptNodePointer(MAX31856_USIC,
                                             XMC_USIC_CH_RXFIFO_INTERRUPT_NODE_POINTER_ALTERNATE,
                                             MAX31856_SERVICE_REQUEST_RX); // IRQ MAX31856_IRQ_RX.

  //Set priority and enable NVIC node for transmit interrupt
  NVIC_SetPriority((IRQn_Type)MAX31856_IRQ_TX, MAX31856_IRQ_TX_PRIORITY);
  NVIC_EnableIRQ((IRQn_Type)MAX31856_IRQ_TX);

  // Set priority and enable NVIC node for receive interrupt
  NVIC_SetPriority((IRQn_Type)MAX31856_IRQ_RX, MAX31856_IRQ_RX_PRIORITY);
  NVIC_EnableIRQ((IRQn_Type)MAX31856_IRQ_RX);

  // Start SPI
  XMC_SPI_CH_Start(MAX31856_USIC);

  // Configure SCLK pin
  XMC_GPIO_Init(MAX31856_SCLK_PIN, &sclk_pin_config);

  // Configure slave select pin
  XMC_GPIO_Init(MAX31856_SELECT_PIN, &select_pin_config);

  // Configure MOSI pin
  XMC_GPIO_Init(MAX31856_MOSI_PIN, &mosi_pin_config);

  XMC_USIC_CH_RXFIFO_Flush(MAX31856_USIC);
  XMC_USIC_CH_RXFIFO_EnableEvent(MAX31856_USIC,
                                 XMC_USIC_CH_RXFIFO_EVENT_CONF_STANDARD | XMC_USIC_CH_RXFIFO_EVENT_CONF_ALTERNATE);

  max31856_spi_write_config();
}

void max31856_tick() {
  max31856.temperature++;
  max31856_spi_read_register(MAX31856_REG_CR0);
  max31856_spi_write_register(MAX31856_REG_CR1);
}

bool max31856_spi_write_config() {
  logd("[+] max31856_spi_write_config()\n\r");

  return true;
}

void max31856_spi_read_register(const MAX31856_REG_t register_address) {
  /*
    void rfm69_task_spi_transceive(void) {
      XMC_SPI_CH_EnableSlaveSelect(RFM69_USIC, XMC_SPI_CH_SLAVE_SELECT_0);
      XMC_USIC_CH_TXFIFO_EnableEvent(RFM69_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
      XMC_USIC_CH_TriggerServiceRequest(RFM69_USIC, RFM69_SERVICE_REQUEST_TX);

      while(rfm69.data_read_index < rfm69.data_length) {
        rfm69_rx_irq_handler();
        coop_task_yield();
      }

      XMC_SPI_CH_DisableSlaveSelect(RFM69_USIC);
    }
  */
}

void max31856_spi_write_register(const MAX31856_REG_t register_address) {
  /*
    void rfm69_task_spi_transceive(void) {
      XMC_SPI_CH_EnableSlaveSelect(RFM69_USIC, XMC_SPI_CH_SLAVE_SELECT_0);
      XMC_USIC_CH_TXFIFO_EnableEvent(RFM69_USIC, XMC_USIC_CH_TXFIFO_EVENT_CONF_STANDARD);
      XMC_USIC_CH_TriggerServiceRequest(RFM69_USIC, RFM69_SERVICE_REQUEST_TX);

      while(rfm69.data_read_index < rfm69.data_length) {
        rfm69_rx_irq_handler();
        coop_task_yield();
      }

      XMC_SPI_CH_DisableSlaveSelect(RFM69_USIC);
    }
  */
}

short int max31856_get_temperature() {
  return max31856.temperature;
}
