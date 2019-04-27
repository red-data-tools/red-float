#include <ruby.h>

static VALUE rb_cFloat16;

void
Init_float16(void)
{
  rb_cFloat16 = rb_define_class("Float16", rb_cNumeric);
}
