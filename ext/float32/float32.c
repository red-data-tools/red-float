#include <ruby.h>

static VALUE rb_cFloat32;

void
Init_float32(void)
{
  rb_cFloat32 = rb_define_class("Float32", rb_cNumeric);
}
