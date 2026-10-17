#include <fstream>
#include <iostream>

#include "biginteger.hpp"

BigInteger fastpow(BigInteger a, BigInteger e, const BigInteger& m) {
  BigInteger res(1);
  a %= m;
  while (e > BigInteger()) {
    if ((e % BigInteger(2)) == BigInteger(1)) {
      res = (res * a) % m;
    }
    a = (a * a) % m;
    e /= BigInteger(2);
  }
  return res;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "\tIncorrect number of arguments :)" << std::endl;
    return 1;
  }

  std::ifstream InputKey(argv[1]);
  if (!InputKey) {
    std::cout << "Can not open InputKey" << std::endl;
    return 1;
  }

  BigInteger n;  // mod
  BigInteger d;  // exponent

  std::string line;
  while (InputKey >> line) {
    if (line == "modulus:") {
      InputKey >> line;
      n = BigInteger(line, biginteger_base::hex);
    } else if (line == "privateExponent:") {
      InputKey >> line;
      d = BigInteger(line, biginteger_base::hex);
    }
  }
  InputKey.close();

  std::ifstream InputMsg(argv[2]);
  if (!InputMsg) {
    std::cout << "Can not open InputMsg" << std::endl;
    return 1;
  }

  InputMsg >> line;  // msg
  InputMsg.close();

  BigInteger c(line, biginteger_base::hex);
  BigInteger m = fastpow(c, d, n);
  std::cout << to_hex(m);

  return 0;
}
