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

#define SKIP_UPDATE_TEMP_TURNS 8192

#define BIT_MASK_REG_WRITE 128
#define RX_TX_BUFFER_LENGTH 8

// CR0 register bitmasks.
#define MAX31856_CR0_CMODE_AUTO (1 << 7)
#define MAX31856_CR0_1SHOT (1 << 6)
#define MAX31856_CR0_OCFAULT_1 (1 << 4)
#define MAX31856_CR0_OCFAULT_2 (2 << 4)
#define MAX31856_CR0_OCFAULT_3 (3 << 4)
#define MAX31856_CR0_CJ_DISABLE (1 << 3)
#define MAX31856_CR0_FAULT_INTERRUPT (1 << 2)
#define MAX31856_CR0_FAULTCLR (1 << 1)
#define MAX31856_CR0_FILTER_50HZ 1

// CR1 register bitmasks.
#define MAX31856_CR1_AVGSEL_2 (1 << 4)
#define MAX31856_CR1_AVGSEL_4 (2 << 4)
#define MAX31856_CR1_AVGSEL_8 (3 << 4)
#define MAX31856_CR1_AVGSEL_16 (4 << 4)
#define MAX31856_CR1_TC_TYPE_E 1
#define MAX31856_CR1_TC_TYPE_J 2
#define MAX31856_CR1_TC_TYPE_K 3
#define MAX31856_CR1_TC_TYPE_N 4
#define MAX31856_CR1_TC_TYPE_R 5
#define MAX31856_CR1_TC_TYPE_S 6
#define MAX31856_CR1_TC_TYPE_T 7
#define MAX31856_CR1_TC_TYPE_G8 8
#define MAX31856_CR1_TC_TYPE_G32 12

// MAX31856 register addresses.
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
  int32_t temperature;
  uint16_t skip_do_update_temperature_turns;
  uint8_t config_reg_cr0;
  uint8_t config_reg_cr1;
  uint8_t config_averaging;
  uint8_t config_thermocouple_type;
  uint8_t config_filter;
  bool do_error_callback;
  bool error_state_open_circuit;
  bool error_state_over_under_voltage;
  uint8_t rx[RX_TX_BUFFER_LENGTH];
  uint8_t tx[RX_TX_BUFFER_LENGTH];
  uint8_t index;
} MAX31856_t;

extern MAX31856_t max31856;

void max31856_init(void);
void max31856_tick(void);
void max31856_spi_read_register(const MAX31856_REG_t register_address,
                                const uint8_t data_length);
void max31856_spi_write_register(const MAX31856_REG_t register_address,
                                 const uint8_t data_length);
int32_t max31856_get_temperature(void);

#endif
