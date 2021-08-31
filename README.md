![GoogleTest Unit Tests](https://github.com/AliBarber/WSPRLite/actions/workflows/test.yml/badge.svg)

## WSPRLite

WSPRLite is a C++ header-only, lightweight, implementation of the [WSPR](http://www.wsprnet.org/drupal/) protocol.
It values simplicity, portability, and readability. It is especially targeted for use with embedded systems, such as Arduino
or Teensy.

At it simplest, a message can be encoded with the following code:

```
using namespace wsprlite;
SymbolStream encoder;

encoder << CallSign("OH2XAB") << Locator("KP20") << PowerLevel(30);

auto symbols = encoder.encode();
for(auto symbol : symbols){
	std::cout << symbol << " ";
}

```

### Limitations
At present, WSPRLite can only encode type 1 messages, with a max 6 char callsign, and 4 char locator.

### Licence
WSPRLite is available under the MIT Licence. Pull requests and issues are welcomed.