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

#ifndef WSPRLITE_INTERLEAVER_H
#define WSPRLITE_INTERLEAVER_H

#include "encoder.h"
#include "message.h"

#ifdef TEST_BUILD
#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test
#endif

namespace wsprlite{
	class Interleaver;
	class Synchroniser;
	class ShuffledMessage{
	public:
		ShuffledMessage() : _data(new uint16_t[11]){
			for(int i = 0; i < 11; i++){
			 	_data[i] = 0x00;	
			}
		}
		~ShuffledMessage(){
			if(_data != nullptr){
				delete [] _data;
			}
		}
		const uint16_t* data() const{
			return _data;
		}
	private:
		friend Interleaver;
		uint16_t* _data;

	};

	class Interleaver
	{
	public:
		static const ShuffledMessage shuffle(const ConvEncodedMessage& source_message){
			ShuffledMessage shuffle;
			uint16_t* shuffled_data = shuffle._data;
			const uint16_t* source_data = source_message.data();
			if(source_data == nullptr){
				return shuffle;
			}
			unsigned int P = 0;
			for(int i = 0; i < 256; i++){
				unsigned char J = reverse(0xFF & i);
				if(J < 162){
					if(getbit(source_data, 0xFF & P)){
						setbit(shuffled_data,(unsigned int) 0xFF & J);
					}
					P++;
				}
			}

			return shuffle;
		}

	private:
		friend Synchroniser;
		#ifdef TEST_BUILD
		FRIEND_TEST(Interleaver, SetBit);
		FRIEND_TEST(Interleaver, GetBit);
		FRIEND_TEST(Interleaver, BitReverse);
		#endif
		static const unsigned char reverse(const unsigned char b){
			// http://graphics.stanford.edu/~seander/bithacks.html#BitReverseTable
			return (unsigned char) 0xFF & ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 
		}
		
		static void setbit(uint16_t* data, unsigned int bit_number){
			for(unsigned int i = 0; i < 16; i++){
				if((i + 1) * 16 > bit_number){
					data[i] |= 0x1 << (bit_number - (16 * i));;
					break;
				}
			}
		}

		static const bool getbit(const uint16_t* data, unsigned int bit_number){
			for(unsigned int i = 0; i < 16; i++){
				if((i + 1) * 16 > bit_number){
					uint16_t test = 0x1 << (bit_number - (16 * i));
					return (test & data[i]) > 0;
				}
			}
			return 0;
		}
	};
}
#endif 