#include <gtest/gtest.h>

#include "wsprlite/message.h"
#include "wsprlite/encoder.h"
#include "wsprlite/interleaver.h"
#include "wsprlite/sync.h"


namespace wsprlite{

	namespace{
		constexpr unsigned short EXPECTED_SYMBOLS[162] = {
			3, 1, 2, 2, 2, 2, 2, 0, 3, 0, 0, 0, 3, 1, 1, 0, 0, 0, 3, 2, 0, 3, 2, 1, 1, 1, 3, 2, 0, 2,
      		0, 0, 2, 2, 3, 0, 0, 3, 2, 3, 2, 0, 0, 2, 0, 2, 3, 2, 3, 1, 0, 0, 3, 3, 2, 3, 0, 2, 2, 3,
      		1, 0, 1, 0, 0, 0, 2, 3, 1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 2, 3, 2, 2, 3, 2, 1, 3, 2, 2, 2, 3,
      		1, 2, 1, 2, 3, 2, 0, 0, 3, 0, 0, 2, 0, 2, 1, 0, 2, 3, 0, 0, 1, 1, 3, 2, 1, 3, 2, 0, 3, 1,
      		0, 3, 0, 2, 0, 1, 3, 1, 2, 0, 2, 0, 2, 3, 0, 1, 0, 0, 3, 3, 0, 0, 2, 2, 2, 0, 2, 1, 3, 0,
      		3, 2, 1, 3, 0, 2, 0, 1, 1, 2, 2, 2
		};
	}

	TEST(CallSign, ValidCallSignEncode){
		CallSign my_call("OH2XAB");
		CallSign short_call("K1AB");
		CallSign very_short_call("OH0");

		EXPECT_EQ(my_call.packed_value(), 0xA56D744);
		EXPECT_EQ(short_call.packed_value(), 0xF70C250);
		EXPECT_EQ(very_short_call.packed_value(), 0xA5648E0);
	}

	TEST(CallSign, RejectInvalidCallsign){
		CallSign long_call("G/OH2XAB/P");
		CallSign empty_call("");
		CallSign unsupported_call("OHFXAB");

		ASSERT_FALSE(long_call.is_valid());
		ASSERT_FALSE(empty_call.is_valid());
		ASSERT_FALSE(unsupported_call.is_valid());
	}

	TEST(Locator, ValidLocators){
		Locator loc1("KP20");
		Locator loc2("AA00");
		Locator loc3("RR99");

		EXPECT_EQ(loc1.packed_value(), 0x36BA);
		EXPECT_EQ(loc2.packed_value(), 0x7DDC);
		EXPECT_EQ(loc3.packed_value(), 0xB3);
	}

	TEST(PowerLevel, PowerLevelEncodes){
		PowerLevel power(0);
		EXPECT_EQ(power.packed_value(), 64);
		PowerLevel invalid(-1);
		ASSERT_FALSE(invalid.is_valid());

	}

	TEST(Combinations, CallSignLocatorPower){
		EncodedMessage message;
		message << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);	
		EXPECT_NE(message.data(), nullptr);
	}

	TEST(EncodedMessage, MessageEncoded){
		EncodedMessage message;
		message << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);
		unsigned int expected[11] = {
			0xA5,
			0x6D,
			0x74,
			0x46,
			0xD7,
			0x57,
			0x80,
			0x00,
			0x00,
			0x00,
			0x00,
		};
		ASSERT_NE(message.data(), nullptr);
		unsigned int *expected_ptr, *test_ptr;
		for(expected_ptr = expected, test_ptr = message.data(); test_ptr < message.data() + 11; expected_ptr++, test_ptr++){
			EXPECT_EQ(*test_ptr, *expected_ptr);
		}
	}

	TEST(ConvEncoder, Parity){
		EXPECT_EQ(ConvolutionalEncoder::parity(6), 0);
		EXPECT_EQ(ConvolutionalEncoder::parity(14), 1);
		EXPECT_EQ(ConvolutionalEncoder::parity(0x328B8020), 1);
		EXPECT_EQ(ConvolutionalEncoder::parity(0x328B8028), 0);
	}

	TEST(ConvEncoder, Encoder){
		EncodedMessage message;
		message << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);
		const ConvEncodedMessage encoded_data = ConvolutionalEncoder::encode(message);
		
		ASSERT_NE(encoded_data.data(), nullptr);

		uint16_t expected[11] = {
			0x4F9B,
			0xE864,
			0xFE00,
			0x2991,
			0x477D,
			0x4A31,
			0x6EFC,
			0x94AC,
			0x3FEA,
			0xCE77,
			0x00,
		};

		const uint16_t *expected_ptr, *test_ptr;
		test_ptr = encoded_data.data();
		for(expected_ptr = expected; test_ptr < encoded_data.data() + 11; test_ptr++, expected_ptr++){
			EXPECT_EQ(*test_ptr, *expected_ptr);
		}
	}

	TEST(Interleaver, BitReverse){
		EXPECT_EQ(Interleaver::reverse(1), 128);
		EXPECT_EQ(Interleaver::reverse(5), 160);
		EXPECT_EQ(Interleaver::reverse(11), 208);
	}

	TEST(Interleaver, SetBit){
		uint16_t data[16];
		for(int i = 0; i < 16; i++){
			data[i] = 0;
		}

		Interleaver::setbit(data, 0);
		EXPECT_EQ(data[0], 1);

		Interleaver::setbit(data, 9);
		EXPECT_EQ(data[0],0x201);

		EXPECT_EQ(data[2],0);	
		Interleaver::setbit(data, 47);
		EXPECT_EQ(data[2],0x8000);

		Interleaver::setbit(data, 161);
		EXPECT_EQ(data[10], 0x2);


	}

	TEST(Interleaver, GetBit){
		uint16_t data[16];
		for(int i = 0; i < 16; i++){
			data[i] = 0;
		}
		
		EXPECT_FALSE(Interleaver::getbit(data, 9));
		EXPECT_FALSE(Interleaver::getbit(data, 161));
		EXPECT_FALSE(Interleaver::getbit(data, 17));

		data[0] = 0x200;
		EXPECT_TRUE(Interleaver::getbit(data, 9));
		EXPECT_FALSE(Interleaver::getbit(data,8));
		data[2] = 0x8000;
		EXPECT_TRUE(Interleaver::getbit(data, 47));
		data[10] = 0x2;
		EXPECT_TRUE(Interleaver::getbit(data, 161));

	}

	TEST(Interleaver, Shuffle){
		EncodedMessage message;
		message << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);

		ShuffledMessage msg = Interleaver::shuffle(ConvolutionalEncoder::encode(message));
		
		uint16_t expected[11] = {
			0x117D,
			0x2C6C,
			0xE9E7,
			0xEF1,
			0xEA8C,
			0xEBEF,
			0xCA4,
			0x4A5B,
			0xCC35,
			0x8AD5,
			0x3
		};

		const uint16_t* test = msg.data();
		const uint16_t *expected_ptr, *test_ptr;
		for(expected_ptr = expected, test_ptr = test; test_ptr < test + 11; test_ptr++, expected_ptr++){
			EXPECT_EQ(*expected_ptr, *test_ptr);
		}
	}

	TEST(Synchroniser, Sync){
		EncodedMessage message;
		message << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);
		ChannelSymbols symbols = Synchroniser::sync(
			Interleaver::shuffle(
				ConvolutionalEncoder::encode(
					message
				)
			)
		);
		for(int i = 0; i < 162; i++){
			EXPECT_EQ(symbols.data()[i], EXPECTED_SYMBOLS[i]);
		}
	}

	TEST(Synchroniser, SyncStream){
		SymbolStream encoder;
		encoder << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);
		ChannelSymbols symbols = encoder.encode();
		for(int i = 0; i < 162; i++){
			EXPECT_EQ(symbols.data()[i], EXPECTED_SYMBOLS[i]);
		}
	}

	TEST(ChannelSymbols, Iterator){
		SymbolStream encoder;
		encoder << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);
		ChannelSymbols symbols = encoder.encode();

		int i = 0;
		for(auto s : symbols){
			EXPECT_EQ(s,EXPECTED_SYMBOLS[i]);
			i++;
		}
		EXPECT_EQ(i, 162);

	}
}