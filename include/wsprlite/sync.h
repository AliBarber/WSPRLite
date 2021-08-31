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

#ifndef WSPRLITE_SYNC_H
#define WSPRLITE_SYNC_H
#include "interleaver.h"
#include "base.h"
#include "message.h"
#include "encoder.h"
namespace wsprlite{

	namespace {
		constexpr uint16_t SYNC_DATA[11] = {
			0x7103,
			0x7A4,
			0x40A4,
			0x58B3,
			0x9558,
			0x5634,
			0xc904,
			0xE2CD,
			0xCA0,
			0x6358,
			0x00
		};
	}	

	class Synchroniser;
	class ChannelSymbols{
	public:
		ChannelSymbols() : _data(){}
		~ChannelSymbols(){}
		const unsigned short* data(){
			return _data;
		}

		using iterator = const unsigned short*;
		iterator begin(){ return &_data[0];}
		iterator end(){ return &_data[162];}


	private:
		friend Synchroniser;
		unsigned short _data[162];
	};

	class Synchroniser{
	public:
		static const ChannelSymbols sync(const ShuffledMessage& msg){
			ChannelSymbols symbols;
			if(msg.data() == nullptr){
				return symbols;
			}
			
			auto data_in = msg.data();
			unsigned short *data_out = symbols._data;

			for(unsigned int i = 0; i < 162; i++){
				bool msg_bit = Interleaver::getbit(data_in, 0xFF & i);
				bool sync_bit = Interleaver::getbit(SYNC_DATA, 0xFF & i);

				if(msg_bit && sync_bit) data_out[i] = 3;
				if(msg_bit && !sync_bit) data_out[i] = 2;
				if(!msg_bit && sync_bit) data_out[i] = 1;
				if(!msg_bit && !sync_bit) data_out[i] = 0;
			}
			return symbols;
		}
	};

	class SymbolStream : public Streamable{
	public:
		SymbolStream():  Streamable(new EncodedMessage()){

		}
		const ChannelSymbols encode(){
			_message->encode();
			return Synchroniser::sync(
				Interleaver::shuffle(
					ConvolutionalEncoder::encode(
						*_message
					)
				)
			);	
		}
		~SymbolStream(){
			if(_message){
				delete _message;
			}
		}
	// private:
		// friend Streamable& operator<<(Streamable& lhs, const Packable& rhs);
	};

	Streamable& operator<<(Streamable& lhs, const Packable& rhs){
		lhs._message->_data[rhs.message_position()] = rhs.packed_value();
		return lhs;
	}
}
#endif
