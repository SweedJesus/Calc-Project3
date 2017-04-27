#pragma once

/*
 * Special member function generation:
 * 1. Default constructor:
 *    Generator only if class contains no user-declared constructors.
 * 2. Copy constructor:
 *    Generated only if class alcks a user-declared copy constructor. Deleted if
 *    the class declares a move operation.
 * 3. Copy assignment operator:
 *    Generated only if the class lacks a user-declared copy assignment
 *    operator. Deleted if class declares a move operation.
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cassert>

// -----------------------------------------------------------------------------

namespace mesa {

  /** BigInt class
  */
  class BigInt
  {
    public:
      // Type aliases
      using digitT = uint32_t;
      using dataT = std::vector<digitT>;

      /** Constructor (integer)
       * @param n Integer
       */
      BigInt(unsigned long long n = 0) noexcept;

      /** Copy constructor
      */
      BigInt(const BigInt&) = default;

      /** Move constructor
      */
      BigInt(BigInt&&) = default;

      /** Constructor (string)
       * @param s String whose every character is a decimal integer digit
       * @throws Invalid argument exception
       * TODO:
       * I know I'm not supposed to throw exceptions from constructors but I
       * don't understand better alternatives yet.
       */
      BigInt(const std::string& s);

      /** Get underlying container
      */
      const dataT& data() const
      { return m_data; }

      /** Get if underlying container is empty
      */
      bool empty() const
      { return m_data.empty(); }

      /** Get size of underlying container
      */
      size_t size() const
      { return m_data.size(); }

      /** Long conversion operator
       * @throw std::out_of_range
       */
      explicit operator unsigned long() const;

      /** Double conversion operator
       * @throw std::out_of_range
       */
      explicit operator double() const
      { return (double)(operator unsigned long()); }

      /** String conversion operator
      */
      explicit operator std::string() const;

      /** Copy assignment operator
      */
      BigInt& operator=(const BigInt&) = default;

      /** Move assignment operator
      */
      BigInt& operator=(BigInt&&) = default;

      /** Addition assignment operator
      */
      BigInt& operator+=(const BigInt& other);

      /** Prefix increment operator
      */
      BigInt& operator++()
      { return operator+=(1); }

      /** Postfix increment operator
      */
      BigInt operator++(int)
      {
        BigInt temp{*this};
        operator++();
        return temp;
      }

      /** Subtraction assignment operator
       * Uses method of complements
       * https://en.wikipedia.org/wiki/Method_of_complements
       */
      BigInt& operator-=(const BigInt& other);

      /** Prefix decrement operator
      */
      BigInt& operator--()
      { return operator-=(1); }

      /** Postfix decrement operator
      */
      BigInt operator--(int)
      {
        BigInt temp{*this};
        operator--();
        return temp;
      }

      /** Multiplication assignment operator
      */
      BigInt& operator*=(const BigInt& other);

      /** Division assignment operator
      */
      BigInt& operator/=(const BigInt& other);

      /** Exponentiation assignment operator
      */
      BigInt& operator^=(const BigInt& other);

    private:
      dataT m_data; // Has a vector of digits

      /** Remove trailing zeroes
      */
      void resize();

      /** Add trailing zeroes (pad)
      */
      void resize(const size_t& n);

      /** Convert to nines complement
       * Flops each digit to its nines complement
       */
      static void to_nines_comp(dataT& data);

      /** Handle carry digits for a BigInt data container
      */
      static void carry(dataT& data);

      /** Add addition helper function
      */
      void add(const BigInt& other);

      /** Subtraction helper function
      */
      void subtract(const BigInt& other);

      /** Multiplication helper function
      */
      void multiply(const BigInt& other);

      /** Division helper function
      */
      void divide(const BigInt& other);

      /** Exponentiation helper functions
      */
      void exponentiate(const BigInt& other);

      /** FFT implementation TODO
      */
      //BigInt& fft(const BigInt& other);
  };
}

// -----------------------------------------------------------------------------

/** Logical negation
*/
inline bool operator!(
    const mesa::BigInt& lhs)
{ return (lhs.empty()); }

/** Equality operator
*/
inline bool operator==(
    const mesa::BigInt& lhs, const mesa::BigInt& rhs)
{ return lhs.data() == rhs.data(); }

/** Inequality operator
*/
inline bool operator!=(
    const mesa::BigInt& lhs, const mesa::BigInt& rhs)
{ return !operator==(lhs, rhs); }

/** Less-than operator
*/
bool operator<(
    const mesa::BigInt& lhs, const mesa::BigInt& rhs);

/** Greater-than operator
*/
inline bool operator>(
    const mesa::BigInt& lhs, const mesa::BigInt& rhs)
{ return operator<(rhs, lhs); }

/** Less-than or equal-to operator
*/
inline bool operator<=(
    const mesa::BigInt& lhs, const mesa::BigInt& rhs)
{ return !operator>(lhs, rhs); }

/** Greater-than or equal-to operator
*/
inline bool operator>=(
    const mesa::BigInt& lhs, const mesa::BigInt& rhs)
{ return !operator<(lhs, rhs); }

/** mesa::BigInt binary addition
*/
inline mesa::BigInt operator+(
    mesa::BigInt lhs, const mesa::BigInt& rhs)
{ return lhs += rhs; }

/** mesa::BigInt binary subtraction
*/
inline mesa::BigInt operator-(
    mesa::BigInt lhs, const mesa::BigInt& rhs)
{ return lhs -= rhs; }


/** mesa::BigInt binary multiplication
*/
inline mesa::BigInt operator*(
    mesa::BigInt lhs, const mesa::BigInt& rhs)
{ return lhs *= rhs; }

/** mesa::BigInt binary division
*/
inline mesa::BigInt operator/(
    mesa::BigInt lhs, const mesa::BigInt& rhs)
{ return lhs /= rhs; }

/** mesa::BigInt binary exponentiation
*/
inline mesa::BigInt operator^(
    mesa::BigInt lhs, const mesa::BigInt& rhs)
{ return lhs ^= rhs; }

/** mesa::BigInt insertion operator
*/
std::ostream& operator<<(
    std::ostream& os, const mesa::BigInt& rhs);

/** mesa::BigInt extraction operator
*/
std::istream& operator>>(
    std::istream& is, mesa::BigInt& rhs);

// -----------------------------------------------------------------------------


