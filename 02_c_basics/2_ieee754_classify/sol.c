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
  const uint64_t frac_mask = 0xFFFFFFFFFFFFF;
  const uint64_t exp_mask = 0x7FF;
  const uint64_t frac_mask_without_elder = 0x7FFFFFFFFFFFF;
  const int sign_shift = 63;
  const int exp_shift = 52;
  uint64_t value = d_t_i(value_ptr);
  uint64_t sign = (value >> sign_shift) & 1;
  uint64_t exp = (value >> exp_shift) & exp_mask;
  uint64_t frac = value & frac_mask;
  if (sign == 0) {
    if (exp == 0) {
      if (frac == 0) {
        return PlusZero;
      }
      return PlusDenormal;
    }
    if (exp == exp_mask) {
      if (frac == 0) {
        return PlusInf;
      }
      if (frac <= frac_mask_without_elder) {
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
    if (exp == exp_mask) {
      if (frac == 0) {
        return MinusInf;
      }
      if (frac <= frac_mask_without_elder) {
        return SignalingNaN;
      }
      return QuietNaN;
    }
    return MinusRegular;
  }
  return value;
}
