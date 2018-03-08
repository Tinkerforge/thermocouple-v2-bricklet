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
#include "max31856.h"

CallbackValue callback_value_temperature;
extern MAX31856_t max31856;

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_TEMPERATURE: return get_callback_value(message, response, &callback_value_temperature);
		case FID_SET_TEMPERATURE_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration(message, &callback_value_temperature);
		case FID_GET_TEMPERATURE_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration(message, response, &callback_value_temperature);
		case FID_SET_CONFIGURATION: return set_configuration(message);
		case FID_GET_CONFIGURATION: return get_configuration(message, response);
		case FID_GET_ERROR_STATE: return get_error_state(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	max31856.config_averaging = (MAX31856_CONFIG_AVERAGING_t)data->averaging;
	max31856.config_thermocouple_type = (MAX31856_CONFIG_TYPE_t)data->thermocouple_type;
	max31856.config_filter = (MAX31856_CONFIG_FILTER_t)data->filter;

	// Write the config to MAX31856.

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfiguration_Response *response) {
	response->header.length = sizeof(GetConfiguration_Response);
	response->averaging = (uint8_t)max31856.config_averaging;
	response->thermocouple_type = (uint8_t)max31856.config_thermocouple_type;
	response->filter = (uint8_t)max31856.config_filter;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_error_state(const GetErrorState *data, GetErrorState_Response *response) {
	response->header.length = sizeof(GetErrorState_Response);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

bool handle_temperature_callback(void) {
	return handle_callback_value_callback(&callback_value_temperature, FID_CALLBACK_TEMPERATURE);
}

bool handle_error_state_callback(void) {
	static bool is_buffered = false;
	static ErrorState_Callback cb;

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(ErrorState_Callback), FID_CALLBACK_ERROR_STATE);
		// TODO: Implement ErrorState callback handling

		return false;
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(ErrorState_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	// TODO: Add proper functions
	callback_value_init(&callback_value_temperature, max31856_get_temperature);

	communication_callback_init();
}
