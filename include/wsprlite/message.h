#ifndef WSPRLITE_MESSAGE_H
#define WSPRLITE_MESSAGE_H

#include <cstring>

#include "wsprlite/base.h"

#ifdef WSPRLITE_STREAM
#include <iostream>
#endif

namespace wsprlite{
	
	class CallSign;
	class Locator;
	class PowerLevel;
	class Streamable;
	class EncodedMessage{
	public:
		EncodedMessage() : 
			_data(new unsigned int[3]),
			_call(nullptr), 
			_locator(nullptr),
			_power(nullptr), 
			_c(new unsigned int[11])
		{
			for(unsigned int* ptr = _c; ptr < _c + 11; ptr++) *ptr = 0;
			for(unsigned int* ptr = _data; ptr < _data + 3; ptr++) *ptr = 0;
			_call = _data;
			_locator = _call + 1;
			_power = _locator + 1;
		}

		~EncodedMessage(){
			if(_c){
				delete [] _c;
			}
			if(_data){
				delete [] _data;
			}
		}

		unsigned int* data() const{
			if(!*_call || !*_locator || !*_power){
				return nullptr;
			}
			return _c;
		}

		void encode(){
			unsigned int locator_power = (*_locator * 128) + *_power;

			_c[0] = (*_call & 0xFF00000) >> 20;
			_c[1] = (*_call & 0xFF000) >> 12;
			_c[2] = (*_call & 0xFF0) >> 4;
			_c[3] = ((*_call & 0xF) << 4) | ((locator_power & 0x3C0000) >> 18);
			_c[4] = (locator_power & 0x3FC00) >> 10;
			_c[5] = (locator_power & 0x3FC) >> 2;
			_c[6] = ((locator_power & 0x3) << 2) << 4;

		}

		bool is_valid(){
			return (*_call && *_locator && *_power);
		}

	private:
		unsigned int* _data;
		unsigned int* _call;
		unsigned int* _locator;
		unsigned int* _power;
		unsigned int* _c;

	friend EncodedMessage& operator<<(EncodedMessage&, const CallSign&);
	friend EncodedMessage& operator<<(EncodedMessage&, const Locator&);
	friend EncodedMessage& operator<<(EncodedMessage&, const PowerLevel&);
	friend Streamable& operator<<(Streamable&, const Packable&);
	};

	class CallSign : public Packable
	{
	public:
		CallSign(const char* callsign) : _cstr(callsign){
			int orig_length = strlen(_cstr);
			if(orig_length > 6){
				_valid = false;
				return;
			}

			char buffer[6];

			// If the 3rd character is not a number, the first char is a space iff the second char is a number
			// else invalid
			if(!(_cstr[2] >= 48 && _cstr[2] <= 57)){
				if(_cstr[1] >= 48 && _cstr[1] <= 57 && orig_length < 6){
					buffer[0] = ' ';
					strcpy(buffer + 1, _cstr);
				}
				else{
					_valid = false;
					return;
				}
			}
			else{
				strcpy(buffer, _cstr);
			}
			int buf_len = strlen(buffer);
			if(buf_len < 6){
				for(int i = 0; i < 6 - buf_len; i++){
					buffer[buf_len + i] = ' ';
				}
			}
			for(int i = 0; i < 6; i++){
				_values[i] = 36; // Space by default
				if(buffer[i] == ' ') continue;
				if(buffer[i] >= 48 && buffer[i] <= 57){
					_values[i] = ( buffer[i] - 48);	
				}
				else{
					_values[i] = (buffer[i] - 55); // - 65 + 10
				}

			}		
			// for(int i = 0; i < 6; i ++){
			// 	cout << buffer[i] << " " << _values[i] << endl;
			// }
			_valid = true;
		};
		unsigned int packed_value() const{
			if(!_valid){
				return 0;
			}

			unsigned int packed = _values[0];
			packed = (packed * 36) + _values[1];
			packed = (packed * 10) + _values[2];
			packed = (packed * 27) + (_values[3] - 10);
			packed = (packed * 27) + (_values[4] - 10);
			packed = (packed * 27) + (_values[5] - 10);
			return packed;

		}
		unsigned int message_position() const {
			return 0;
		}

	private:
		const char* _cstr;
		short _values[6];

	};

	class PowerLevel : public Packable
	{
	public:
		PowerLevel(const unsigned int power_level) : _power_level(power_level){
			if(_power_level > 60 || _power_level < 0) return;
			_valid = true;
		}

		unsigned int packed_value() const{
			return _power_level + 64;
		}
		unsigned int message_position() const {
			return 2;
		}

	private:
		const unsigned int _power_level;
	};	

	class Locator : public Packable
	{
	public:
		Locator(const char* locator_str) : _cstr(locator_str){
			int length = strlen(_cstr);
			if(length != 4){
				return;
			}
			_values[0] = _cstr[0] - 'A';
			_values[1] = _cstr[1] - 'A';
			_values[2] = _cstr[2] - '0';
			_values[3] = _cstr[3] - '0';

			// cout << (int) _values[0] << " " << (int) _values[1] << " " << (int) _values[2] << " " << (int) _values[3] << endl;
			_valid = true;
		}
		Locator(const Locator& loc){
			Locator(loc._cstr);
		}
		unsigned int packed_value() const{
			if(!_valid){
				return 0;
			}
			unsigned int packed = ((179 - 10 * _values[0]) - _values[2]) * 180 + (10 * _values[1]) + _values[3];
			return packed;
		}
		unsigned int message_position() const {
			return 1;
		}

	private:
		const char* _cstr;
		char _values[4];
	};

	EncodedMessage& operator<<(EncodedMessage& lhs, const CallSign& rhs){
		*lhs._call = rhs.packed_value();
		lhs.encode();
		return lhs;
	}
	EncodedMessage& operator<<(EncodedMessage& lhs, const Locator& rhs){
		*lhs._locator = rhs.packed_value();
		lhs.encode();
		return lhs;
	} 
	EncodedMessage& operator<<(EncodedMessage& lhs, const PowerLevel& rhs){
		*lhs._power = rhs.packed_value();
		lhs.encode();
		return lhs;
	}
	
	#ifdef WSPRLITE_STREAM
	std::ostream& operator<<(std::ostream& lhs, const EncodedMessage& rhs){
		unsigned int *message_data = rhs.data();
		if(message_data == nullptr){
			lhs << "Empty"; 
			return lhs;
		}
		
		for(unsigned int* ptr = message_data; ptr < message_data + 7; ptr++){
			lhs << std::hex << *ptr << " ";
		}
		return lhs;
	}
	#endif
};
#endif
