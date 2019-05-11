#include <ruby.h>

#include <float.h>
#include <math.h>

static VALUE rb_cFloat32;

typedef union {
  void *ptr;
  float float_value;
} rb_float32_data_t;

static size_t
float32_memsize(const void *ptr)
{
  return 0;
}

static const rb_data_type_t float32_data_type = {
  "float32",
  {
    NULL,
    NULL,
    float32_memsize,
  },
  0, 0, RUBY_TYPED_FREE_IMMEDIATELY
};

VALUE
rb_float32_new(float value)
{
  rb_float32_data_t data;
  data.float_value = value;

  return TypedData_Wrap_Struct(rb_cFloat32, &float32_data_type, data.ptr);
}

void
Init_float32(void)
{
  rb_cFloat32 = rb_define_class("Float32", rb_cNumeric);

  rb_undef_alloc_func(rb_cFloat32);
  rb_undef_method(CLASS_OF(rb_cFloat32), "new");

  rb_define_const(rb_cFloat32, "MANT_DIG", INT2FIX(FLT_MANT_DIG));
  rb_define_const(rb_cFloat32, "DIG", INT2FIX(FLT_DIG));
  rb_define_const(rb_cFloat32, "MIN_EXP", INT2FIX(FLT_MIN_EXP));
  rb_define_const(rb_cFloat32, "MAX_EXP", INT2FIX(FLT_MAX_EXP));
  rb_define_const(rb_cFloat32, "MIN_10_EXP", INT2FIX(FLT_MIN_10_EXP));
  rb_define_const(rb_cFloat32, "MAX_10_EXP", INT2FIX(FLT_MAX_10_EXP));
  rb_define_const(rb_cFloat32, "MIN", rb_float32_new(FLT_MIN));
  rb_define_const(rb_cFloat32, "MAX", rb_float32_new(FLT_MAX));
  rb_define_const(rb_cFloat32, "EPSILON", rb_float32_new(FLT_EPSILON));
  rb_define_const(rb_cFloat32, "INFINITY", rb_float32_new(HUGE_VALF));
  rb_define_const(rb_cFloat32, "NAN", rb_float32_new(nanf("")));
}
