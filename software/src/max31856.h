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

#ifndef MAX31856_H
#define MAX31856_H

#include <stdint.h>
#include <stdbool.h>

#define BIT_MASK_REG_WRITE 128
#define RX_TX_BUFFER_LENGTH 8

typedef enum MAX31856_CONFIG_AVERAGING {
  MAX31856_CONFIG_AVERAGING_1 = 1,
  MAX31856_CONFIG_AVERAGING_2 = 2, 
  MAX31856_CONFIG_AVERAGING_4 = 4, 
  MAX31856_CONFIG_AVERAGING_8 = 8,
  MAX31856_CONFIG_AVERAGING_16 = 16
} MAX31856_CONFIG_AVERAGING_t;

typedef enum MAX31856_CONFIG_TYPE {
  MAX31856_CONFIG_TYPE_B = 0,
  MAX31856_CONFIG_TYPE_E,
  MAX31856_CONFIG_TYPE_J,
  MAX31856_CONFIG_TYPE_K,
  MAX31856_CONFIG_TYPE_N,
  MAX31856_CONFIG_TYPE_R,
  MAX31856_CONFIG_TYPE_S,
  MAX31856_CONFIG_TYPE_T,
  MAX31856_CONFIG_TYPE_G8,
  MAX31856_CONFIG_TYPE_G32
} MAX31856_CONFIG_TYPE_t;

typedef enum MAX31856_CONFIG_FILTER {
  MAX31856_CONFIG_FILTER_50HZ = 0,
  MAX31856_CONFIG_FILTER_60HZ
} MAX31856_CONFIG_FILTER_t;

typedef enum MAX31856_REG {
  MAX31856_REG_CR0 = 0, // Configuration 0 Register.
  MAX31856_REG_CR1, // Configuration 1 Register.
  MAX31856_REG_MASK, // Fault Mask Register.
  MAX31856_REG_CJHF, // Cold-Junction High Fault Threshold.
  MAX31856_REG_CJLF, // Cold-Junction Low Fault Threshold.
  MAX31856_REG_LTHFTH, // Linearized Temperature High Fault Threshold MSB.
  MAX31856_REG_LTHFTL, // Linearized Temperature High Fault Threshold LSB.
  MAX31856_REG_LTLFTH, // Linearized Temperature Low Fault Threshold MSB.
  MAX31856_REG_LTLFTL, // Linearized Temperature Low Fault Threshold LSB.
  MAX31856_REG_CJTO, // Cold-Junction Temperature Offset Register.
  MAX31856_REG_CJTH, // Cold-Junction Temperature Register, MSB.
  MAX31856_REG_CJTL, // Cold-Junction Temperature Register, LSB.
  MAX31856_REG_LTCBH, // Linearized TC Temperature, Byte 2.
  MAX31856_REG_LTCBM, // Linearized TC Temperature, Byte 1.
  MAX31856_REG_LTCBL, // Linearized TC Temperature, Byte 0.
  MAX31856_REG_SR // Fault Status Register.
} MAX31856_REG_t;

typedef struct {
  short int temperature;
  uint8_t config_reg_cr0;
  uint8_t config_reg_cr1;
  MAX31856_CONFIG_AVERAGING_t config_averaging;
  MAX31856_CONFIG_TYPE_t config_thermocouple_type;
  MAX31856_CONFIG_FILTER_t config_filter;
  bool error_state_changed;
  bool error_state_over_current;
  bool error_state_open_circuit;
  uint8_t rx[RX_TX_BUFFER_LENGTH];
  uint8_t tx[RX_TX_BUFFER_LENGTH];
  uint8_t index;
} MAX31856_t;

void max31856_init();
void max31856_tick();
void max31856_spi_read_register(const MAX31856_REG_t register_address,
                                const uint8_t data_length);
void max31856_spi_write_register(const MAX31856_REG_t register_address,
                                 const uint8_t data_length);
short int max31856_get_temperature(void);

#endif
