/*
 * AudioTypes.h
 *
 *  Created on: 19.1.2013
 *      Author: neneko
 */

#ifndef AUDIOTYPES_H_
#define AUDIOTYPES_H_
#include <cstdint>
#include <vector>
#include <string>
#include <set>

namespace iimaudio {
enum class sampling_rate_t: uint8_t {
	rate_unknown,
	rate_8kHz,
	rate_11kHz,
	rate_22kHz,
	rate_44kHz,
	rate_48kHz
};

enum class sampling_format_t: uint8_t {
	format_unknown,
	format_8bit_unsigned,
	format_8bit_signed,
	format_16bit_unsigned,
	format_16bit_signed,
};


enum class action_type_t: uint8_t {
	action_capture,
	action_playback
};

enum class return_type_t: uint8_t {
	ok,
	failed,
	xrun,
	invalid,
	buffer_full,
	buffer_empty,
	busy
};

uint32_t convert_rate_to_int(const sampling_rate_t rate);
sampling_rate_t convert_int_to_rate(const size_t irate);
uint32_t get_sample_size(const sampling_format_t format);
std::string error_string(const return_type_t error);

std::string sampling_rate_string(const sampling_rate_t rate);
std::string sampling_format_string(const sampling_format_t format);


struct audio_buffer_t {
	std::vector<uint8_t> data;
	bool empty;
	uint32_t position;
	audio_buffer_t():empty(true),position(0) {}
};

struct audio_params_t {
	sampling_rate_t rate;
	sampling_format_t format;
	uint8_t num_channels;
	audio_params_t(sampling_rate_t rate = sampling_rate_t::rate_22kHz, sampling_format_t format=sampling_format_t::format_16bit_signed, uint8_t num_channels=1):
		rate(rate),format(format),num_channels(num_channels) {}
	uint16_t sample_size() const { return get_sample_size(format)*num_channels; }
};

struct audio_info_t {
	std::string name;
	std::size_t max_channels;
	std::set<std::pair<sampling_format_t, sampling_rate_t>> supported_formats;
	bool default;
};

template<typename T>
struct circular_buffer_t {
	std::vector<T> data;
	std::size_t start_;
	std::size_t end_;
	circular_buffer_t(std::size_t size):data(size,0),start_(0),end_(0) {}
	bool store_data(const T *in_data, std::size_t size) {
		bool overflow = false;
		const std::size_t till_end = data.size() - end_;
		const std::size_t copy1 = std::min(till_end,size);
		std::copy(in_data,in_data+copy1,data.begin()+end_);
		if ((end_ < start_) && (end_ +copy1 >= start_ )) {
			overflow = true;
			start_ = (end_+copy1+1)%data.size();
		}
		end_=(end_+copy1)%data.size();
		const std::size_t remaining = size - copy1;
		if (!remaining) return overflow;

		return store_data(in_data+copy1,remaining);
	}
	std::size_t get_data_block(T* out_data, std::size_t max_size) {
		if (start_ < end_) {
			const std::size_t avail = std::min(end_ - start_,max_size);
			std::copy(data.begin()+start_,data.begin()+start_+avail,out_data);
			start_ = (start_ + avail)%data.size();
			return avail;
		}
		if (start_ > end_) {
			const std::size_t till_end = data.size() - start_;
			const std::size_t copy1 = std::min(max_size, till_end);
			std::copy(data.begin()+start_,data.begin()+start_+copy1,out_data);
			start_ = (start_ + copy1)%data.size();
			if (copy1 >= max_size) return copy1;
			return get_data_block(out_data+copy1,max_size-copy1);
		}
		return 0;
	}
};



}
#endif /* AUDIOTYPES_H_ */
