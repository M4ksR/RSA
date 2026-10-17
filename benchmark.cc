#include <chrono>
#include <cstring>
#include <iostream>

#include "biginteger.hpp"

class Timer {
 private:
  using clock_t = std::chrono::high_resolution_clock;
  using second_t = std::chrono::duration<double, std::ratio<1>>;

  std::chrono::time_point<clock_t> begin_;

 public:
  Timer() : begin_(clock_t::now()) {}
  void reset() { begin_ = clock_t::now(); }
  // double elapsed() const{ return
  // std::chrono::duration_cast<second_t>(clock_t::now() - begin_).count(); }
  long long elapsed() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               clock_t::now() - begin_)
        .count();
  }
};

BigInteger factorial(int n) {
  BigInteger f = 1;
  for (int i = 2; i <= n; ++i) {
    f *= i;
  }
  return f;
}

BigInteger binomial(int n, int k) {
  if (n < 0 || k < 0 || k > n) {
    return 0;
  }
  return factorial(n) / factorial(k) / factorial(n - k);
}

int main() {
  Timer timer;

  BigInteger fib101("354224848179261915075");
  BigInteger fib201("280571172992510140037611932413038677189525");

  // operator + benchmark
  timer.reset();

  for (int j = 0; j < 3; ++j) {
    BigInteger x1 = 1;
    BigInteger x2 = 1;
    BigInteger x3;
    for (int i = 3; i < 101; ++i) {
      x3 = x1 + x2;
      x1 = x2;
      x2 = x3;
    }
    if (x3 != fib101) {
      std::cout << "Error: sum " << to_string(x3) << " != " << to_string(fib101)
                << std::endl;
      return 1;
    }
    for (int i = 101; i < 201; ++i) {
      x3 = x1 + x2;
      x1 = x2;
      x2 = x3;
    }
    if (x3 != fib201) {
      std::cout << "Error: sum " << to_string(x3) << " != " << to_string(fib201)
                << std::endl;
      return 1;
    }

    for (int i = 201; i < 100001; ++i) {
      x3 = x1 + x2;
      x1 = x2;
      x2 = x3;
    }
  }

  std::cout << "Summa       :: Elapsed time in microseconds: "
            << timer.elapsed() << std::endl;

  // operator *= benchmark
  timer.reset();

  BigInteger f10 = 1;
  for (int i = 2; i < 11; ++i) {
    f10 *= i;
  }

  BigInteger f20 = 1;
  for (int i = 11; i < 21; ++i) {
    f20 *= i;
  }

  BigInteger f30 = 1;
  for (int i = 21; i < 31; ++i) {
    f30 *= i;
  }

  BigInteger f100 = 1;
  for (int i = 31; i < 101; ++i) {
    f100 *= i;
  }

  BigInteger f2000 = 1;
  for (int i = 101; i < 2001; ++i) {
    f2000 *= i;
  }

  BigInteger f3000;
  f3000 = f10 * f20 * f30 * f100;
  for (int i = 101; i < 3001; ++i) {
    f3000 *= i;
  }

  BigInteger fxx1(f3000);
  for (int i = 0; i < 4; ++i) {
    fxx1 *= fxx1;
    fxx1 *= f3000;
  }
  std::cout << "Mult *=     :: Elapsed time in microseconds: "
            << timer.elapsed() << std::endl;

  timer.reset();

  // operator * benchmark
  f10 = 1;
  for (int i = 2; i < 11; ++i) {
    f10 = f10 * i;
  }

  f20 = 1;
  for (int i = 11; i < 21; ++i) {
    f20 = f20 * i;
  }

  f30 = 1;
  for (int i = 21; i < 31; ++i) {
    f30 = f30 * i;
  }

  f100 = 1;
  for (int i = 31; i < 101; ++i) {
    f100 = f100 * i;
  }

  f2000 = 1;
  for (int i = 101; i < 2001; ++i) {
    f2000 = f2000 * i;
  }

  f3000 = f10 * f20 * f30 * f100;
  for (int i = 101; i < 3001; ++i) {
    f3000 = f3000 * i;
  }

  BigInteger fxx2 = f3000;
  for (int i = 0; i < 4; ++i) {
    fxx2 = fxx2 * fxx2;
    fxx2 = fxx2 * f3000;
  }
  std::cout << "Mult        :: Elapsed time in microseconds: "
            << timer.elapsed() << std::endl;

  if (fxx1 != fxx2) {
    std::cout << "Error mult: " << to_hex(fxx1) << " != " << to_hex(fxx2)
              << std::endl;
    return 3;
  }

  // operator / benchmark
  timer.reset();
  f3000 = f3000 / f10;
  auto dt1 = timer.elapsed();
  std::cout << "Div 10!     :: Elapsed time in microseconds: " << dt1
            << std::endl;

  timer.reset();
  f3000 = f3000 / f100;
  auto dt2 = timer.elapsed();
  std::cout << "Div 100!    :: Elapsed time in microseconds: " << dt2
            << std::endl;

  timer.reset();
  f3000 = f3000 / f30;
  f3000 = f3000 / f20;
  f3000 = f3000 / f2000;
  auto dt3 = timer.elapsed();
  std::cout << "Div 3 times :: Elapsed time in microseconds: " << dt3
            << std::endl;

  timer.reset();
  for (int i = 2001; i < 3001; ++i) {
    f3000 /= i;
  }
  auto dt4 = timer.elapsed();
  std::cout << "Div 1000    :: Elapsed time in microseconds: " << dt4
            << std::endl;
  std::cout << "Div total   :: Elapsed time in microseconds: "
            << (dt1 + dt2 + dt3 + dt4) << std::endl;

  if (f3000 != BigInteger("1")) {
    std::cout << "Error div: " << to_string(f3000) << " != 1" << std::endl;
    return 3;
  }

  // sum of binomials benchmark
  timer.reset();
  BigInteger S1 = 0;
  BigInteger S2 = 0;
  int N = 1000;
  for (int i = 0; i <= N; ++i) {
    BigInteger Cnk = binomial(N, i);
    S1 += Cnk;
    S2 += Cnk;
    S2 = -S2;
  }
  std::cout << "Binomial    :: Elapsed time in microseconds: "
            << timer.elapsed() << std::endl;

  BigInteger n2 = 1;
  BigInteger two = 2;
  for (int i = 1; i <= N; ++i) {
    n2 *= two;
  }
  if (S1 != n2) {
    std::cout << "Error binomial: " << to_string(S1) << " != " << to_string(n2)
              << std::endl;
    return 3;
  }

  if (S2 != 0) {
    std::cout << "Error binomial: " << to_string(S2) << " != 0" << std::endl;
    return 3;
  }

  timer.reset();
  std::string result_hex = to_hex(f2000);
  std::cout << "to hex      :: Elapsed time in microseconds: "
            << timer.elapsed() << std::endl;

  timer.reset();
  std::string result = to_string(f2000);
  std::cout << "to string   :: Elapsed time in microseconds: "
            << timer.elapsed() << std::endl;

  if (BigInteger(result.c_str()) !=
      BigInteger(result_hex.c_str(), biginteger_base::hex)) {
    std::cout << "Error convertion: BigInteger(" << result << ") != BigInteger("
              << result_hex << ", biginteger_base::hex)" << std::endl;
    return 3;
  }

  return 0;
}
