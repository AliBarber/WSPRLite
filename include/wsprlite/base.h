#ifndef WSPRLITE_BASE_HPP
#define WSPRLITE_BASE_HPP

namespace wsprlite{
	
	class EncodedMessage;
	class ChannelSymbols;
	// class Callsign;
	// class Locator;
	// class PowerLevel;

	// Something that we must do some arithmetic to 
	// convert into a WSPR compatible message
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

	// Something that was can pass down a 'stream' 
	// and use with the << operator
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
