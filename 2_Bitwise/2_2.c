//
// Created by astesh on 29.09.22.
//

/*Реализуйте функцию с прототипом:

typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

extern float_class_t
classify(double *value_ptr);


которая классифицирует специальные значения вещественного числа, на которое указывает value_ptr, в соответствии со стандартом IEEE 754.*/
#include <stdint.h>
#include <string.h>

typedef enum {
  PlusZero = 0x00,
  MinusZero = 0x01,
  PlusInf = 0xF0,
  MinusInf = 0xF1,
  PlusRegular = 0x10,
  MinusRegular = 0x11,
  PlusDenormal = 0x20,
  MinusDenormal = 0x21,
  SignalingNaN = 0x30,
  QuietNaN = 0x31
} float_class_t;

uint64_t d_t_i(double* x) {
  uint64_t res = 0;
  memcpy(&res, x, sizeof(uint64_t));
  return res;
}

extern float_class_t classify(double* value_ptr) {
  uint64_t value = d_t_i(value_ptr);
  uint64_t sign = (value >> 63) & 1;
  uint64_t exp = (value >> 52) & 0b11111111111;
  uint64_t
      frac = value & 0b1111111111111111111111111111111111111111111111111111;
  if (sign == 0) {
    if (exp == 0) {
      if (frac == 0) {
        return PlusZero;
      }
      return PlusDenormal;
    }
    if (exp == 0b11111111111) {
      if (frac == 0) {
        return PlusInf;
      }
      if (frac <= 0b0111111111111111111111111111111111111111111111111111) {
        return SignalingNaN;
      }
      return QuietNaN;
    }
    return PlusRegular;
  }
  if (sign == 1) {
    if (exp == 0) {
      if (frac == 0) {
        return MinusZero;
      }
      return MinusDenormal;
    }
    if (exp == 0b11111111111) {
      if (frac == 0) {
        return MinusInf;
      }
      if (frac <= 0b0111111111111111111111111111111111111111111111111111) {
        return SignalingNaN;
      }
      return QuietNaN;
    }
    return MinusRegular;
  }
  return value;
}