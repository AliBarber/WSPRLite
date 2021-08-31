#ifndef WSPRLITE_ENCODER_H
#define WSPRLITE_ENCODER_H
#include "message.h"

namespace wsprlite {
	class ConvolutionalEncoder;
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
	class ConvolutionalEncoder
	{
	public:
		static bool parity(const uint32_t input){
			// A bit hacky - but does the job for us
			// TODO: Make this private / protected (and tests friends)
			short parity = 0;
			uint32_t input_shifted = input;
			for(int i = 0; i < 32; i++){
				parity = parity ^ (input_shifted & 0x1);
				input_shifted = input_shifted >> 1;
			}
			return parity > 0;
		}

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
					
					*output_byte = *output_byte | (parity(reg_1_parity_input) << output_byte_counter++);
					*output_byte = *output_byte | (parity(reg_2_parity_input) << output_byte_counter++); 
				}
			}
			return out_msg;
		}
	};
}
#endif