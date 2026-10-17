#include "biginteger.hpp"

#include <limits>
#include <stdexcept>

template <typename T>
const T& max(const T& a, const T& b) {
  return (a < b) ? b : a;
}

void BigInteger::UNSvalue(unsigned long long value) {
  if (value == 0) {
    return;
  }
  if (value <= 0xFFFFFFFFULL) {
    data[0] = value;
    sign = 1;
  } else {
    uint32_t* new_data = new uint32_t[2];

    new_data[0] = value;
    new_data[1] = value >> 32;

    delete[] data;

    data = new_data;
    busy = 2;
    sign = 1;
  }
}

void BigInteger::Svalue(long long value) {
  if (value == 0) {
    return;
  }
  if (value > 0) {
    UNSvalue((unsigned long long)value);
    sign = 1;
  } else {
    UNSvalue((unsigned long long)(-value));
    sign = -1;
  }
}

BigInteger::BigInteger() : busy(1), sign(0) { data = new uint32_t[busy](); }

BigInteger::BigInteger(int number) : BigInteger() { Svalue(number); }

BigInteger::BigInteger(unsigned int number) : BigInteger() { UNSvalue(number); }

BigInteger::BigInteger(long number) : BigInteger() { Svalue(number); }

BigInteger::BigInteger(unsigned long number) : BigInteger() {
  UNSvalue(number);
}

BigInteger::BigInteger(long long number) : BigInteger() { Svalue(number); }

BigInteger::BigInteger(unsigned long long number) : BigInteger() {
  UNSvalue(number);
}

bool znak(char ch) { return ch == '+' || ch == '-'; }

void BigInteger::dec(const std::string& str) {
  BigInteger res;
  int sgn = 1;

  if (str[0] == '-') {
    sgn = -1;
  }

  size_t start = (size_t)(znak(str[0]) ? 1 : 0);

  for (size_t i = start; i < str.size(); ++i) {
    int tmp;
    if (str[i] >= '0' && str[i] <= '9') {
      tmp = str[i] - '0';
    } else {
      throw std::invalid_argument("incorrect number given");
    }

    res = res * 10 + BigInteger(tmp);
  }

  if (res.sign != 0) {
    res.sign = sgn;
  }

  res.DelLeadZero();
  *this = std::move(res);
}

uint32_t NumberListToHex(const int* list, int index) {
  uint32_t res = 0;
  for (int i = index + 1; i < 8; ++i) {
    res = res * 16 + list[i];
  }
  return res;
}

void BigInteger::hex(const std::string& str) {
  int new_sign;
  if (str[0] == '-') {
    new_sign = -1;
  } else {
    new_sign = 1;
  }

  int end = znak(str[0]) ? 1 : 0;

  size_t sz = str.size() - end;
  uint32_t* new_data = new uint32_t[(sz + 7) >> 3]();

  int j = 0;  // индекс первого свободного блока data
  int k = 7;            // индекс первой цифры
  int number[8] = {0};  // собранное число

  for (int i = (int)(str.size() - 1); i >= end; --i) {
    int tmp;
    if (str[i] >= '0' && str[i] <= '9') {
      tmp = str[i] - '0';
    } else if (str[i] >= 'A' && str[i] <= 'F') {
      tmp = str[i] - 'A' + 10;
    } else if (str[i] >= 'a' && str[i] <= 'f') {
      tmp = str[i] - 'a' + 10;
    } else {
      delete[] new_data;
      throw std::invalid_argument("incorrect number given");
    }
    number[k--] = tmp;

    if (k < 0) {
      new_data[j++] = NumberListToHex(number, k);
      k = 7;
    }
  }

  if (k != 7) {
    new_data[j++] = NumberListToHex(number, k);
  }
  delete[] data;

  data = new_data;
  busy = j;
  sign = new_sign;

  DelLeadZero();
}

BigInteger::BigInteger(std::string str, biginteger_base base) : BigInteger() {
  // пустая строка
  if (str.empty()) {
    throw std::invalid_argument("string is empty");
  }
  // только знак
  if (str.size() == 1 && znak(str[0])) {
    throw std::invalid_argument("number not specified");
  }
  // два и более знаков подряд
  if (str.size() > 1 && znak(str[0]) && znak(str[1])) {
    throw std::invalid_argument("more than one operator used");
  }

  if (base == biginteger_base::dec) {
    dec(str);
  } else if (base == biginteger_base::hex) {
    hex(str);
  }
}

bool BigInteger::operator==(const BigInteger& other) const {
  if ((sign != other.sign) || (busy != other.busy)) {
    return false;
  }
  for (size_t i = 0; i < busy; ++i) {
    if (data[i] != other.data[i]) {
      return false;
    }
  }
  return true;
}

bool BigInteger::operator!=(const BigInteger& other) const {
  return !(*this == other);
}

bool BigInteger::operator<(const BigInteger& other) const {
  if (sign != other.sign) {
    return sign < other.sign;
  }

  if (sign == 1) {
    if (busy != other.busy) {
      return busy < other.busy;
    }
    for (int i = (int)busy - 1; i >= 0; --i) {
      if (data[i] != other.data[i]) {
        return data[i] < other.data[i];
      }
    }
  }

  else if (sign == -1) {
    if (busy != other.busy) {
      return busy > other.busy;
    }
    for (int i = (int)busy - 1; i >= 0; --i) {
      if (data[i] != other.data[i]) {
        return data[i] > other.data[i];
      }
    }
  }
  return false;
}

bool BigInteger::operator>(const BigInteger& other) const {
  return other < *this;
}

bool BigInteger::operator<=(const BigInteger& other) const {
  return !(*this > other);
}

bool BigInteger::operator>=(const BigInteger& other) const {
  return !(*this < other);
}

BigInteger BigInteger::operator-() const {
  BigInteger res(*this);
  if (res.sign != 0) {
    res.sign = -res.sign;
  }
  return res;
}

void BigInteger::DelLeadZero() {
  while (busy > 1 && data[busy - 1] == 0) {
    --busy;
  }
  if (busy == 1 && data[0] == 0) {
    sign = 0;
  }
}

int Fcmp(const uint32_t* num1, const uint32_t* num2, size_t size1,
         size_t size2) {
  if (size1 < size2) {
    return -1;
  }
  if (size1 > size2) {
    return 1;
  }
  for (int i = (int)size1 - 1; i >= 0; --i) {
    if (num1[i] != num2[i]) {
      return (num1[i] < num2[i]) ? -1 : 1;
    }
  }
  return 0;
}

BigInteger BigInteger::DiffAbs(const BigInteger& other) const {
  BigInteger res;
  uint32_t* new_data = new uint32_t[busy]();

  delete[] res.data;

  res.data = new_data;
  res.busy = busy;
  res.sign = 1;

  uint64_t curr = 0;
  const uint64_t BASE = 1ULL << 32;
  for (size_t i = 0; i < busy; ++i) {
    uint64_t a = data[i];
    uint64_t b = (i < other.busy ? other.data[i] : 0);
    uint64_t tmp = b + curr;

    if (a >= tmp) {
      res.data[i] = (uint32_t)(a - tmp);
      curr = 0;
    } else {
      res.data[i] = (uint32_t)(BASE + a - tmp);
      curr = 1;
    }
  }
  res.DelLeadZero();
  return res;
}

uint32_t BigInteger::BlockDel(uint32_t div) {
  uint64_t r = 0;
  for (int i = (int)busy - 1; i >= 0; --i) {
    uint64_t curr = (r << 32) + data[i];
    data[i] = uint32_t(curr / div);
    r = curr % div;
  }
  DelLeadZero();
  return r;
}

BigInteger BigInteger::BlockMul(uint32_t fac) const {
  BigInteger res;
  uint32_t* new_data = new uint32_t[busy + 1]();

  delete[] res.data;

  res.data = new_data;
  res.busy = busy + 1;
  res.sign = sign;

  uint64_t curr = 0;
  for (size_t i = 0; i < busy; ++i) {
    uint64_t tmp = (uint64_t)data[i] * fac + curr;
    res.data[i] = (uint32_t)tmp;
    curr = (tmp >> 32);
  }

  if (curr != 0U) {
    res.data[busy] = curr;
  }
  res.DelLeadZero();
  return res;
}

void BigInteger::AddBlock(uint32_t block) {
  BigInteger newInteger;
  uint32_t* new_data = new uint32_t[busy + 1];

  new_data[0] = block;

  for (size_t i = 0; i < busy; ++i) {
    new_data[i + 1] = data[i];
  }

  delete[] newInteger.data;

  newInteger.data = new_data;
  newInteger.busy = busy + 1;
  newInteger.sign = (sign != 0) ? sign : 1;

  *this = std::move(newInteger);
}

void BigInteger::DelAbs(const BigInteger& other, BigInteger& q,
                        BigInteger& r) const {
  if (other.sign == 0) {
    throw std::invalid_argument("division by zero");
  }

  if (sign == 0) {
    q = 0;
    r = 0;
    return;
  }

  int cmp = Fcmp(data, other.data, busy, other.busy);
  switch (cmp) {
    case -1:
      q = 0;
      r = *this;
      return;
    case 0:
      q = 1;
      r = 0;
      return;
  }

  if (other.busy == 1) {
    BigInteger tmp(*this);
    uint32_t ost = tmp.BlockDel(other.data[0]);
    q = tmp;
    r = BigInteger(ost);
    return;
  }

  uint32_t* new_data = new uint32_t[busy]();

  delete[] q.data;
  q.data = new_data;
  q.busy = busy;
  q.sign = 1;

  for (int i = (int)busy - 1; i >= 0; --i) {
    r.AddBlock(data[i]);

    uint32_t L = 0;
    uint32_t R = std::numeric_limits<uint32_t>::max();
    uint32_t num = 0;

    while (L <= R) {
      uint32_t mid = L + ((R - L) >> 1);
      BigInteger prod = other.BlockMul(mid);
      if (prod <= r) {
        num = mid;
        L = mid + 1;
      } else {
        if (mid == 0) {
          break;
        }
        R = mid - 1;
      }
    }

    q.data[i] = num;
    r -= other.BlockMul(num);
  }
  r.DelLeadZero();
  q.DelLeadZero();
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (sign == other.sign) {
    size_t mx = max(busy, other.busy);

    uint32_t* new_data = new uint32_t[mx + 1]();

    uint64_t curr = 0;
    for (size_t i = 0; i < mx; ++i) {
      uint64_t a = 0;
      uint64_t b = 0;

      if (i < busy) {
        a = data[i];
      }
      if (i < other.busy) {
        b = other.data[i];
      }

      uint64_t tmp = a + b + curr;
      new_data[i] = (uint32_t)tmp;
      curr = tmp >> 32;
    }

    new_data[mx] = (uint32_t)curr;
    delete[] data;

    data = new_data;
    busy = mx + 1;
    DelLeadZero();
    return *this;
  }

  int cmp = Fcmp(data, other.data, busy, other.busy);

  if (cmp == 0) {
    uint32_t* new_data = new uint32_t[1]();
    delete[] data;
    data = new_data;
    busy = 1;
    sign = 0;
    return *this;
  }

  if (cmp == 1) {
    BigInteger res = this->DiffAbs(other);
    res.sign = sign;
    *this = res;
    return *this;
  }

  BigInteger res = other.DiffAbs(*this);
  res.sign = other.sign;
  *this = res;
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  *this += (-other);
  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  if (sign == 0 || other.sign == 0) {
    uint32_t* new_data = new uint32_t[1]();
    delete[] data;
    data = new_data;
    busy = 1;
    sign = 0;
    return *this;
  }

  uint32_t* new_data = new uint32_t[busy + other.busy]();
  size_t new_busy = busy + other.busy;
  int new_sign = (sign == other.sign) ? 1 : -1;

  for (size_t i = 0; i < busy; ++i) {
    uint64_t curr = 0;
    for (size_t j = 0; j < other.busy; ++j) {
      uint64_t tmp =
          (uint64_t)new_data[i + j] + (uint64_t)data[i] * other.data[j] + curr;
      new_data[i + j] = (uint32_t)tmp;
      curr = tmp >> 32;
    }

    size_t k = i + other.busy;
    while (curr != 0U) {
      uint64_t tmp = (uint64_t)new_data[k] + curr;
      new_data[k] = (uint32_t)tmp;
      curr = tmp >> 32;
      ++k;
    }
  }
  delete[] data;

  data = new_data;
  busy = new_busy;
  sign = new_sign;
  DelLeadZero();
  return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  BigInteger dividend(*this);
  BigInteger divisor(other);

  dividend.sign = 1;
  divisor.sign = 1;

  BigInteger q;
  BigInteger r;
  dividend.DelAbs(divisor, q, r);

  if (q.sign != 0) {
    q.sign = (sign == other.sign) ? 1 : -1;
  }

  *this = q;
  return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  BigInteger dividend(*this);
  BigInteger divisor(other);

  dividend.sign = 1;
  divisor.sign = 1;

  BigInteger q;
  BigInteger r;
  dividend.DelAbs(divisor, q, r);

  if (r.sign != 0) {
    r.sign = sign;
  }

  *this = r;
  return *this;
}

BigInteger operator+(BigInteger value, const BigInteger& other) {
  return value += other;
}

BigInteger operator-(BigInteger value, const BigInteger& other) {
  return value -= other;
}

BigInteger operator*(BigInteger value, const BigInteger& other) {
  return value *= other;
}

BigInteger operator/(BigInteger value, const BigInteger& other) {
  return value /= other;
}

BigInteger operator%(BigInteger value, const BigInteger& other) {
  return value %= other;
}

BigInteger BigInteger::operator++(int) {
  BigInteger tmp(*this);
  *this += 1;
  return tmp;
}

BigInteger BigInteger::operator--(int) {
  BigInteger tmp(*this);
  *this -= 1;
  return tmp;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

void AddNumber(std::string& str, uint32_t x) {
  if (x == 0) {
    str.push_back('0');
    return;
  }

  char list[10];
  int k = 0;

  while (x > 0) {
    list[k++] = (char)((x % 10) + '0');
    x /= 10;
  }

  while (k > 0) {
    str.push_back(list[--k]);
  }
}

std::string to_string(const BigInteger& number) {
  if (number.sign == 0 || (number.busy == 1 && number.data[0] == 0)) {
    return "0";
  }

  const uint32_t MAX = 1000000000U;

  BigInteger tmp(number);
  if (tmp.sign < 0) {
    tmp.sign = 1;
  }

  int i = 0;
  while (tmp.sign != 0) {
    tmp.BlockDel(MAX);
    ++i;
  }

  BigInteger num(number);
  if (num.sign < 0) {
    num.sign = 1;
  }

  if (i == 0) {
    return "0";
  }

  uint32_t* part = new uint32_t[i];
  int j = 0;

  while (num.sign != 0) {
    part[j++] = num.BlockDel(MAX);
  }

  std::string res;
  res.reserve(j * 9 + (number.sign < 0 ? 1 : 0));

  if (number.sign < 0) {
    res.push_back('-');
  }
  AddNumber(res, part[j - 1]);

  for (int i = j - 2; i >= 0; --i) {
    char list[9];
    uint32_t curr = part[i];
    for (int k = 8; k >= 0; --k) {
      list[k] = (char)((curr % 10) + '0');
      curr /= 10;
    }
    res.append(list, 9);
  }

  delete[] part;
  return res;
}

std::string to_hex(const BigInteger& number) {
  if (number.sign == 0 || (number.busy == 1 && number.data[0] == 0)) {
    return "0";
  }

  std::string res;
  res.reserve(number.busy * 8 + (number.sign < 0 ? 1 : 0));

  if (number.sign < 0) {
    res.push_back('-');
  }

  bool start = false;
  for (int i = 28; i >= 0; i -= 4) {
    uint32_t tmp = (number.data[number.busy - 1] >> i) & 0xF;
    if (tmp != 0 || start) {
      if (tmp < 10) {
        res.push_back((char)(tmp + '0'));
      } else {
        res.push_back((char)(tmp - 10 + 'A'));
      }
      start = true;
    }
  }
  if (!start) {
    res.push_back('0');
  }

  for (int i = (int)number.busy - 2; i >= 0; --i) {
    for (int j = 28; j >= 0; j -= 4) {
      uint32_t tmp = (number.data[i] >> j) & 0xF;
      if (tmp < 10) {
        res.push_back((char)(tmp + '0'));
      } else {
        res.push_back((char)(tmp - 10 + 'A'));
      }
    }
  }

  return res;
}
