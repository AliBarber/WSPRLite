/*
Copyright © 2021 Alastair Barber - OH2XAB

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the “Software”),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef WSPRLITE_ENCODER_H
#define WSPRLITE_ENCODER_H
#include "message.h"
#include "base.h"

namespace wsprlite {
	class ConvolutionalEncoder;
	/**
	* @brief Container class for a convolutional encoded message
	*/
	class ConvEncodedMessage{
	public:
		ConvEncodedMessage() : _data(new uint16_t[11]){
			for(int i = 0; i < 11; i++){
				_data[i] = 0x00;
			}
		}
		~ConvEncodedMessage(){
			if(_data != nullptr){
				delete [] _data;
			}
		}
		const uint16_t* data() const{
			return _data;
		}
	private:
		friend ConvolutionalEncoder;
		uint16_t* _data;
	};

	/**
	* @brief Implements the convolutional encoder stage of message encoding
	*/
	class ConvolutionalEncoder
	{
	public:
		/**
		* Convolutionaly encode an input message into a 161 bit output
		* @oaram message An Encoded Message consting of callsign, locator and power
		*/
		static const ConvEncodedMessage encode(const EncodedMessage& message){
			ConvEncodedMessage out_msg;
			if(message.data() == nullptr){
				return out_msg;
			}

			uint32_t reg_1, reg_2;
			
			reg_1 = 0x00000000;
			reg_2 = 0x00000000;

			for(int i = 0; i < 11; i++){ 
				// The byte from the message
				uint8_t message_byte = (uint8_t) message.data()[i];
				uint16_t *output_byte = out_msg._data + i;
				*output_byte = 0x0000;
				unsigned int output_byte_counter = 0;
				for(int j = 0; j < 8; j++){
					bool message_bit = ((message_byte << j) & 0x80) == 0x80;
					reg_1 = reg_1 << 1;
					reg_2 = reg_2 << 1;

					reg_1 = reg_1 | (uint32_t) message_bit;
					reg_2 = reg_2 | (uint32_t) message_bit;	

					uint32_t reg_1_parity_input = reg_1 & 0xF2D05351;
					uint32_t reg_2_parity_input = reg_2 & 0xE4613C47;
					
					*output_byte = *output_byte | (parity32(reg_1_parity_input) << output_byte_counter++);
					*output_byte = *output_byte | (parity32(reg_2_parity_input) << output_byte_counter++); 
				}
			}
			return out_msg;
		}
	private:
		#ifdef TEST_BUILD
		FRIEND_TEST(ConvEncoder, Parity);
		#endif
		static bool parity32(const uint32_t input){
			short parity = 0;
			uint32_t input_shifted = input;
			for(int i = 0; i < 32; i++){
				parity = parity ^ (input_shifted & 0x1);
				input_shifted = input_shifted >> 1;
			}
			return parity > 0;
		}
	};
}
#endif