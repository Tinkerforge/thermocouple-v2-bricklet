/* thermocouple-v2-bricklet
 * Copyright (C) 2018 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/utility/callback_value.h"
#include "bricklib2/logging/logging.h"

#include "max31856.h"

CallbackValue callback_value_temperature;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_TEMPERATURE:
			return get_callback_value(message, response, &callback_value_temperature);
		case FID_SET_TEMPERATURE_CALLBACK_CONFIGURATION:
			return set_callback_value_callback_configuration(message,
			                                                 &callback_value_temperature);
		case FID_GET_TEMPERATURE_CALLBACK_CONFIGURATION:
			return get_callback_value_callback_configuration(message,
			                                                 response,
			                                                 &callback_value_temperature);
		case FID_SET_CONFIGURATION:
			return set_configuration(message);
		case FID_GET_CONFIGURATION:
			return get_configuration(message, response);
		case FID_GET_ERROR_STATE:
			return get_error_state(message, response);
		default:
			return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	logd("[+] Thermocouple V2 Bricklet : set_configuration()\n\r");

	uint8_t cr0 = 0;
	uint8_t cr1 = 0;

	max31856_spi_read_register(MAX31856_REG_CR0, 1);

	cr0 = max31856.rx[0];

	if ((MAX31856_CONFIG_FILTER_t)data->filter == MAX31856_CONFIG_FILTER_50HZ) {
		cr0 |= MAX31856_CR0_FILTER_50HZ;
	}
	else if ((MAX31856_CONFIG_FILTER_t)data->filter == MAX31856_CONFIG_FILTER_60HZ) {
		cr0 &= ~1;
	}
	else {
		return HANDLE_MESSAGE_RESPONSE_EMPTY;
	}

	if ((MAX31856_CONFIG_AVERAGING_t)data->averaging == MAX31856_CONFIG_AVERAGING_1) {
		;
	}
	else if ((MAX31856_CONFIG_AVERAGING_t)data->averaging == MAX31856_CONFIG_AVERAGING_2) {
		cr1 = MAX31856_CR1_AVGSEL_2;
	}
	else if ((MAX31856_CONFIG_AVERAGING_t)data->averaging == MAX31856_CONFIG_AVERAGING_4) {
		cr1 = MAX31856_CR1_AVGSEL_4;
	}
	else if ((MAX31856_CONFIG_AVERAGING_t)data->averaging == MAX31856_CONFIG_AVERAGING_8) {
		cr1 = MAX31856_CR1_AVGSEL_8;
	}
	else if ((MAX31856_CONFIG_AVERAGING_t)data->averaging == MAX31856_CONFIG_AVERAGING_16) {
		cr1 = MAX31856_CR1_AVGSEL_16;
	}
	else {
		return HANDLE_MESSAGE_RESPONSE_EMPTY;
	}

	if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_B) {
		;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_E) {
		cr1 |= MAX31856_CR1_TC_TYPE_E;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_J) {
		cr1 |= MAX31856_CR1_TC_TYPE_J;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_K) {
		cr1 |= MAX31856_CR1_TC_TYPE_K;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_N) {
		cr1 |= MAX31856_CR1_TC_TYPE_N;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_R) {
		cr1 |= MAX31856_CR1_TC_TYPE_R;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_S) {
		cr1 |= MAX31856_CR1_TC_TYPE_S;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_T) {
		cr1 |= MAX31856_CR1_TC_TYPE_T;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_G32) {
		cr1 |= MAX31856_CR1_TC_TYPE_G8;
	}
	else if ((MAX31856_CONFIG_TYPE_t)data->thermocouple_type == MAX31856_CONFIG_TYPE_G32) {
		cr1 |= MAX31856_CR1_TC_TYPE_G32;
	}
	else {
		return HANDLE_MESSAGE_RESPONSE_EMPTY;
	}

	// Write the config to MAX31856.
	max31856.tx[0] = cr0;
	max31856.tx[1] = cr1;
	max31856_spi_write_register(MAX31856_REG_CR0, 2);

	max31856.config_averaging = (MAX31856_CONFIG_AVERAGING_t)data->averaging;
	max31856.config_thermocouple_type = (MAX31856_CONFIG_TYPE_t)data->thermocouple_type;
	max31856.config_filter = (MAX31856_CONFIG_FILTER_t)data->filter;

	max31856.skip_do_update_temperature_turns = SKIP_UPDATE_TEMP_TURNS;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data,
                                                  GetConfiguration_Response *response) {
	logd("[+] Thermocouple V2 Bricklet : get_configuration()\n\r");

	response->header.length = sizeof(GetConfiguration_Response);
	response->averaging = (uint8_t)max31856.config_averaging;
	response->thermocouple_type = (uint8_t)max31856.config_thermocouple_type;
	response->filter = (uint8_t)max31856.config_filter;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_error_state(const GetErrorState *data,
                                                GetErrorState_Response *response) {
	logd("[+] Thermocouple V2 Bricklet : get_error_state()\n\r");

	response->header.length = sizeof(GetErrorState_Response);
	response->over_under = max31856.error_state_over_under_voltage;
	response->open_circuit = max31856.error_state_open_circuit;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

bool handle_temperature_callback(void) {
	return handle_callback_value_callback(&callback_value_temperature,
		                                    FID_CALLBACK_TEMPERATURE);
}

bool handle_error_state_callback(void) {
	static bool is_buffered = false;
	static ErrorState_Callback cb;

	if(!is_buffered) {
		if(max31856.do_error_callback) {
			tfp_make_default_header(&cb.header,
			                        bootloader_get_uid(),
			                        sizeof(ErrorState_Callback),
			                        FID_CALLBACK_ERROR_STATE);

			cb.open_circuit = max31856.error_state_open_circuit;
			cb.over_under = max31856.error_state_over_under_voltage;

			is_buffered = true;
		}
		else {
			is_buffered = false;

			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(ErrorState_Callback));

		is_buffered = false;
		max31856.do_error_callback = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	callback_value_init(&callback_value_temperature, max31856_get_temperature);
	communication_callback_init();
}
