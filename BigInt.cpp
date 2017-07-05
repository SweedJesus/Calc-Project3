/* BigInt.cpp
 * Nils Olsson
 *
 * Note that binary arithmetic operators call helper functions and *then*
 * resize the output numbers. This way a numbers are not resized until a full
 * operator is complete. e.g. Subtraction calls addition, but I don't want to
 * resize the number after addition but until after all operations in
 * subtraction are finished.
 */

#include <iomanip>

#include "BigInt.h"

// -----------------------------------------------------------------------------
// BigInt
// Private non-static member definitions
// -----------------------------------------------------------------------------

using mesa::BigInt;

void BigInt::resize()
{
  m_data.erase(
      std::find_if_not(m_data.rbegin(), --m_data.rend(), [](const DigitT& a)
        { return (a == 0); }).base(), m_data.end());
}

void BigInt::resize(const size_t& n)
{
  m_data.resize(n);
}

void BigInt::to_nines_comp(BigInt::DataT& data)
{
  std::transform(data.begin(), data.end(), data.begin(),
      [](const DigitT& a){ return (9 - a); });
}

void BigInt::carry(BigInt::DataT& data)
{
  BigInt::DigitT temp = 0, carry = 0;
  std::transform(data.begin(), data.end(), data.begin(),
      [&temp, &carry](const BigInt::DigitT& digit)
      { temp = digit + carry; carry = temp / 10; return temp % 10; });
}

void BigInt::add(const BigInt& other)
{
  auto& lhs = m_data;
  auto& rhs = other.m_data;
  // Add digit padding
  resize(std::max(lhs.size(), rhs.size()) + 1);
  // Add phase
  std::transform(rhs.begin(), rhs.end(), lhs.begin(), lhs.begin(),
      [](const DigitT& lhsDigit, const DigitT& rhsDigit)
      { return lhsDigit + rhsDigit; });
  // Carry phase
  carry(m_data);
}

void BigInt::subtract(const BigInt& other)
{
  if (other > (*this))
    throw std::range_error(
        "Negative results unsupported '" +
        std::string{*this} + " - " + std::string{other} + "'");
  to_nines_comp(m_data);
  add(other);
  to_nines_comp(m_data);
  m_data.pop_back();
}

// TODO:
// Implement FFT in another life
void BigInt::multiply(const BigInt& other)
{
  // Using acyclic convolution
  // https://en.wikipedia.org/wiki/Sch%C3%B6nhage%E2%80%93Strassen_algorithm
  // https://en.wikipedia.org/wiki/Circular_convolution

  // Special cases
  if ((*this) == 0 || other == 0) {
    m_data = DataT{0};
    return;
  } else if (other == 1) {
    return;
  } else if ((*this) == 1) {
    m_data = other.m_data;
    return;
  }
  // lhs:multiplicand, rhs:multiplier
  auto& lhs = m_data;
  auto& rhs = other.m_data;
  DataT res(std::max(lhs.size(), rhs.size()) * 2);
  // Multiplication phase
  BigInt m = 0;
  std::for_each(rhs.begin(), rhs.end(),
      [&m, &lhs, &res](const DigitT& rhsDigit) {
      std::transform(lhs.begin(), lhs.end(),
        res.begin()+(unsigned long)(m), res.begin()+(unsigned long)(m),
        [&rhsDigit](const DigitT& lhsDigit, const DigitT& resDigit)
        { return resDigit + lhsDigit * rhsDigit; });
      carry(res);
      ++m;
      });
  // Carry phase
  m_data.swap(res);
}

void BigInt::divide(const BigInt& other)
{
  // https://en.wikipedia.org/wiki/Division_algorithm
  // TODO: Replace literal number 0 with static BigInt with '0' to reduce
  // on new BigInt's created
  if ((*this) == 0)
    return;
  if (other == 0)
    throw std::invalid_argument(
        "Division by zero");
  auto& N = (*this);
  auto& D = other;
  BigInt Q;
  while (N >= D) {
    N -= D;
    ++Q;
  }
  m_data.swap(Q.m_data);
}

void BigInt::exponentiate(const BigInt& other)
{
  // Something is zero...
  if (((*this) == 0) || (other == 0)) {
    if (((*this) == 0) && (other == 0)) {
      throw std::domain_error(
          "Result of '" + std::string{*this} + "/" +
          std::string{other} + "' undefined");
    } else if ((*this) == 0) {
      (*this) = 0;
      return;
    } else { // other == 0
      (*this) = 1;
      return;
    }
  }
  // https://en.wikipedia.org/wiki/Exponentiation_by_squaring

  // lhs:base, rhs:exponent
  auto& X = (*this);
  auto& x = X.m_data;
  auto N = other;
  auto& n = N.m_data;
  BigInt R{1};
  R.resize((unsigned long)(x.size() * N));
  auto& r = R.m_data;
  while (N != 0) {
    if (n.front() % 2 == 1) {
      R *= X;
      --N;
    }
    X *= X;
    N /= 2;
  }
  x.swap(r);
}

// -----------------------------------------------------------------------------
// BigInt
// Public non-static member definitions
// -----------------------------------------------------------------------------

BigInt::BigInt(unsigned long long n) noexcept
{
  // Don't have to remove trailing zeroes (not a thing for numbers)
  // Inserts digits in reverse order
  do {
    m_data.push_back(n % 10);
    n /= 10;
  } while (n != 0);
}

BigInt::BigInt(const std::string& s)
{
  // Check if is numeric
  if (s.empty() || std::find_if(s.begin(), s.end(),
        [](char c) { return !std::isdigit(c); }) != s.end())
    throw std::invalid_argument(
        "Attempted conversion from non-numeric token '" + s + "'");
  // Get position of first non-trailing-zero character...
  auto jt = ((s.size() > 1) ?
      std::string::const_reverse_iterator(
        std::find_if_not(s.begin(), s.end(),
          [](const char& c) { return c == '0'; })) :
      s.crend());
  // And insert from there to end
  for (auto it = s.rbegin(); it != jt; ++it)
    m_data.push_back((*it) - '0');
}

BigInt::operator unsigned long() const
{
  std::ostringstream oss;
  std::copy(m_data.rbegin(), m_data.rend(), std::ostream_iterator<DigitT>(oss));
  return std::stoul(oss.str());
}

BigInt::operator std::string() const
{
  std::ostringstream ss;
  std::copy(m_data.rbegin(), m_data.rend(),
      std::ostream_iterator<DigitT>(ss));
  return ss.str();
}

//BigInt::operator char*() const

BigInt& BigInt::operator+=(const BigInt& other)
{
  if (other == 0)
    return *this;
  add(other);
  resize();
  return *this;
}

BigInt& BigInt::operator-=(const BigInt& other)
{
  subtract(other);
  resize();
  return *this;
}

BigInt& BigInt::operator*=(const BigInt& other)
{
  multiply(other);
  resize();
  return *this;
}

BigInt& BigInt::operator/=(const BigInt& other)
{
  divide(other);
  resize();
  return *this;
}

BigInt& BigInt::operator%=(const BigInt& other)
{
  BigInt temp(*this);
  temp /= other;
  operator-=(temp * other);
  return *this;
}

BigInt& BigInt::operator^=(const BigInt& other)
{
  exponentiate(other);
  resize();
  return *this;
}

// -----------------------------------------------------------------------------
// External definitions
// -----------------------------------------------------------------------------

bool operator<(const BigInt& lhs, const BigInt& rhs)
{
  if (lhs == rhs) {
    return false;
  }
  if (lhs.size() < rhs.size()) {
    return true;
  }
  if (lhs.size() > rhs.size()) {
    return false;
  }
  auto lhsIt = lhs.data().rbegin();
  auto rhsIt = rhs.data().rbegin();
  for (; lhsIt != lhs.data().rend(); ++lhsIt, ++rhsIt) {
    if (*lhsIt < *rhsIt)
      return true;
    else if (*lhsIt > *rhsIt)
      return false;
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, const BigInt& rhs)
{
  return (os << std::string{rhs});
}

std::istream& operator>>(std::istream& is, BigInt& rhs)
{
  std::string token;
  is >> token;
  rhs = BigInt{token};
  return is;
}
