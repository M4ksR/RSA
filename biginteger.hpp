#ifndef BIG_INTEGER_HPP
#define BIG_INTEGER_HPP
#include <cstdint>
#include <string>

enum class biginteger_base { dec = 10, hex = 16 };

class BigInteger {
 private:
  // поля
  uint32_t* data;  // число разбитое на блоки
  size_t busy;     // количество используемых блоков
  int sign;        // знак числа

  // методы
  void UNSvalue(unsigned long long);  // unsigned type
  void Svalue(long long);             // signed type
  void dec(const std::string&);       // парс строки из dec
  void hex(const std::string&);       // парс строки из hex
  void DelLeadZero();  // удаление ведущих нулей
  BigInteger DiffAbs(const BigInteger&) const;  // разность модулей
  uint32_t BlockDel(uint32_t);  // деление числа на какой-то div
  BigInteger BlockMul(uint32_t) const;  // умножение числа на какой-то fac
  void AddBlock(uint32_t);  // добавление младшего блока
  void DelAbs(const BigInteger&, BigInteger&,
              BigInteger&) const;  // деление модулей

 public:
  // деструктор
  ~BigInteger() { delete[] data; }

  // дефолтный конструктор
  BigInteger();

  // конструкторы от типов данных
  BigInteger(int);
  BigInteger(unsigned int);
  BigInteger(long);
  BigInteger(unsigned long);
  BigInteger(long long);
  BigInteger(unsigned long long);
  BigInteger(std::string, biginteger_base base = biginteger_base::dec);

  // конструктор копирования
  BigInteger(const BigInteger& other) {
    busy = other.busy;
    sign = other.sign;

    data = new uint32_t[busy];
    for (size_t i = 0; i < busy; ++i) {
      data[i] = other.data[i];
    }
  }

  // конструктор перемещения
  BigInteger(BigInteger&& other) noexcept {
    data = other.data;
    busy = other.busy;
    sign = other.sign;

    other.data = nullptr;
    other.busy = 0;
    other.sign = 0;
  }

  // присвоение копированием
  BigInteger& operator=(const BigInteger& other) {
    if (this != &other) {
      uint32_t* new_data = new uint32_t[other.busy];

      for (size_t i = 0; i < other.busy; ++i) {
        new_data[i] = other.data[i];
      }

      delete[] data;

      data = new_data;
      busy = other.busy;
      sign = other.sign;
    }
    return *this;
  }

  // присвоение перемещением
  BigInteger& operator=(BigInteger&& other) noexcept {
    if (this != &other) {
      delete[] data;
      data = other.data;
      busy = other.busy;
      sign = other.sign;

      other.data = nullptr;
      other.busy = 0;
      other.sign = 0;
    }
    return *this;
  }

  // операторы сравнения
  bool operator==(const BigInteger&) const;
  bool operator!=(const BigInteger&) const;
  bool operator<(const BigInteger&) const;
  bool operator>(const BigInteger&) const;
  bool operator<=(const BigInteger&) const;
  bool operator>=(const BigInteger&) const;

  // арифметические операции
  BigInteger operator+() const { return *this; }
  BigInteger operator-() const;

  BigInteger& operator+=(const BigInteger&);
  BigInteger& operator-=(const BigInteger&);
  BigInteger& operator*=(const BigInteger&);
  BigInteger& operator/=(const BigInteger&);
  BigInteger& operator%=(const BigInteger&);

  BigInteger operator++(int);
  BigInteger operator--(int);
  BigInteger& operator++();
  BigInteger& operator--();

  // friends
  friend std::string to_string(const BigInteger&);
  friend std::string to_hex(const BigInteger&);
};

BigInteger operator+(BigInteger, const BigInteger&);
BigInteger operator-(BigInteger, const BigInteger&);
BigInteger operator*(BigInteger, const BigInteger&);
BigInteger operator/(BigInteger, const BigInteger&);
BigInteger operator%(BigInteger, const BigInteger&);

std::string to_string(const BigInteger&);
std::string to_hex(const BigInteger&);

#endif
