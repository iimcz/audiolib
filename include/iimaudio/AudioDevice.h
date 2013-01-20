/*
 * AudioDevice.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef AUDIODEVICE_H_
#define AUDIODEVICE_H_
#include "AudioTypes.h"
#include "AudioPlatform.h"
#include "AudioPolicies.h"
#include <array>
namespace iimaudio {

template<action_type_t action = action_type_t::action_capture, class Threading = SingleThreaded, class Device = PlatformDevice>
class AudioDevice:private Device,private Threading
{
public:
	typedef typename Device::audio_id_t audio_id;
	typedef typename Threading::lock_t lock_t;
	AudioDevice();
	AudioDevice(const audio_params_t& params, audio_id device_id = Device::default_device());
	virtual ~AudioDevice() {}

	return_type_t start_capture();

	template<typename T>
	size_t capture_data(std::vector<T>& buffer, return_type_t& error_code);
	template<typename T, std::size_t S>
	size_t capture_data(std::array<T,S>& buffer, return_type_t& error_code);
	template<typename T>
	size_t capture_data(T* raw_data, std::size_t data_size, return_type_t& error_code);

	return_type_t set_buffers(uint16_t count, uint32_t samples);

	template<typename T>
	return_type_t fill_buffer(const std::vector<T>& data);

	return_type_t start_playback();

	return_type_t update(size_t delay = 10);
	audio_params_t get_params();
};



/* Implementation of AudioDevice */
template<action_type_t action, class Threading, class Device>
AudioDevice<action, Threading, Device>::AudioDevice():
		AudioDevice<action, Device>(audio_params_t(),Device::default_device()) {}

template<action_type_t action, class Threading, class Device>
AudioDevice<action, Threading, Device>::AudioDevice(const audio_params_t& params,
		audio_id device_id):
		Device(action, device_id, params)
{

}

template<action_type_t action, class Threading, class Device>
return_type_t AudioDevice<action, Threading, Device>::start_capture()
{
	lock_t lock = Threading::lock_instance();
	return Device::do_start_capture();
}

template<action_type_t action, class Threading, class Device>
return_type_t AudioDevice<action, Threading, Device>::set_buffers(uint16_t count, uint32_t samples)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_set_buffers(count, samples);
}

template<action_type_t action, class Threading, class Device>
return_type_t AudioDevice<action, Threading, Device>::start_playback()
{
	lock_t lock = Threading::lock_instance();
	return Device::do_start_playback();
}

template<action_type_t action, class Threading, class Device>
return_type_t AudioDevice<action, Threading, Device>::update(size_t delay)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_update(delay);
}

template<action_type_t action, class Threading, class Device>
audio_params_t AudioDevice<action, Threading, Device>::get_params()
{
	lock_t lock = Threading::lock_instance();
	return Device::do_get_params();
}


template<action_type_t action, class Threading, class Device>
template<typename T>
return_type_t AudioDevice<action, Threading, Device>::fill_buffer(const std::vector<T>& data)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_fill_buffer(reinterpret_cast<const uint8_t*>(&data[0]),data.size()*sizeof(T));

}

template<action_type_t action, class Threading, class Device>
template<typename T>
size_t AudioDevice<action, Threading, Device>::capture_data(std::vector<T>& buffer, return_type_t& error_code)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_capture_data(reinterpret_cast<uint8_t*>(&buffer[0]),buffer.size()*sizeof(T),error_code);
}
template<action_type_t action, class Threading, class Device>
template<typename T, std::size_t S>
size_t AudioDevice<action, Threading, Device>::capture_data(std::array<T,S>& buffer, return_type_t& error_code)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_capture_data(reinterpret_cast<uint8_t*>(&buffer[0]),S*sizeof(T),error_code);
}
template<action_type_t action, class Threading, class Device>
template<typename T>
size_t AudioDevice<action, Threading, Device>::capture_data(T* raw_data, std::size_t data_size, return_type_t& error_code)
{
	lock_t lock = Threading::lock_instance();
	return Device::do_capture_data(reinterpret_cast<uint8_t*>(raw_data),data_size*sizeof(T),error_code);
}



}

#endif /* AUDIODEVICE_H_ */
