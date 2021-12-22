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
#ifndef WSPRLITE_BASE_HPP
#define WSPRLITE_BASE_HPP

#ifdef TEST_BUILD
#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test
#endif

namespace wsprlite{
	
	class EncodedMessage;
	class ChannelSymbols;

	/**
	* @brief Packable Absctract Class
	* 
	* Subclasses of this class are values that can be 'packed' into 
	* a single unsgined int. Eg, callsign.
	*/
	class Packable{
	public:	
		Packable() : _valid(false){}
		virtual ~Packable(){}
		bool is_valid(){
			return _valid;
		}
		virtual unsigned int packed_value() const = 0;
		virtual unsigned int message_position() const = 0;
	protected:
		bool _valid;
	};

	/**
	* @brief Classes that can be treated similarly to a sream
	*
	* These classes are used to build up a compound value, such as
	* the final set of symbols from the message inputs.
	*/
	class Streamable{
	public:
		Streamable(EncodedMessage* message) : _message(message){}
		virtual const ChannelSymbols encode() = 0;
	protected:
		friend Streamable& operator<<(Streamable& lhs, const Packable& rhs);
		EncodedMessage* _message;
	};
};
#endif
