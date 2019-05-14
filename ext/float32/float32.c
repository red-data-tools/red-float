#include <ruby.h>

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <math.h>

#define rb_raise_static(etype, mesg) \
    rb_exc_raise(rb_exc_new_str(etype, rb_str_new_static(mesg, rb_strlen_lit(mesg))))

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

#define RB_FLOAT32_TYPE_P(x) rb_typeddata_is_kind_of((x), &float32_data_type)

static inline float
rb_float32_get_value(VALUE flt)
{
  rb_float32_data_t data;
  TypedData_Get_Struct(flt, void, &float32_data_type, data.ptr);
  return data.float_value;
}

#define FLOAT32_VALUE(x) rb_float32_get_value(x)

VALUE
rb_float32_new(float value)
{
  rb_float32_data_t data;
  data.float_value = value;

  return TypedData_Wrap_Struct(rb_cFloat32, &float32_data_type, data.ptr);
}

static inline VALUE
rb_float32_new_from_double(double value)
{
  return rb_float32_new((float)value);
}

static inline VALUE
rb_float32_new_from_ruby_float(VALUE flt)
{
  assert(RB_FLOAT_TYPE_P(flt));
  return rb_float32_new_from_double(RFLOAT_VALUE(flt));
}

#define num2dbl_without_to_f(x) \
  (FIXNUM_P(x) ? (double)FIX2LONG(x) : \
   RB_TYPE_P(x, T_BIGNUM) ? rb_big2dbl(x) : \
   (Check_Type(x, T_FLOAT), RFLOAT_VALUE(x)))
#define rat2dbl_without_to_f(x) \
  (num2dbl_without_to_f(rb_rational_num(x)) \
   / num2dbl_without_to_f(rb_rational_den(x)))

static inline VALUE
rb_float32_new_from_ruby_rational(VALUE rat)
{
  double dbl_value;

  assert(RB_TYPE_P(rat, T_RATIONAL));

  dbl_value = rat2dbl_without_to_f(rat);

  return rb_float32_new_from_double(dbl_value);
}

static float
rb_cstr_to_flt(const char *p, int badcheck, int raise, int *error)
{
  const char *q;
  float d;
  char *end;
  const char *ellipsis = "";
  int w;
  enum {max_width = 20};
#define OutOfRange() ((end - p > max_width) ? \
                      (w = max_width, ellipsis = "...") : \
                      (w = (int)(end - p), ellipsis = ""))

  if (!p) return 0.0f;
  q = p;
  while (ISSPACE(*p)) ++p;

  /* Reject 0x... and 0X... */
  if (!badcheck && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
    return 0.0;
  }

  d = strtof(p, &end);
  /* when either overflow or underflow is occurred */
  if (errno == ERANGE) {
    OutOfRange();
    rb_warning("Float32 %.*s%s out of range", w, p, ellipsis);
    errno = 0;
  }

  /* when no conversion is performed */
  if (p == end) {
    if (badcheck) {
bad:
      if (raise)
        rb_invalid_str(q, "Float32()");
      else {
        if (error) *error = 1;
        return 0.0f;
      }
    }
    return d;
  }

  /* when the conversion stopped at the middle of the string */
  if (*end) {
    char buf[FLT_DIG * 4 + 10];
    char *n = buf;
    char *const init_e = buf + FLT_DIG * 4;
    char *e = init_e;
    char prev = 0;
    int dot_seen = 0;

    /* transfering characters before the `*end` */
    switch (*p) { case '+': case '-': prev = *n++ = *p++; }
    if (*p == '0') {
      prev = *n++ = '0';
      while (*++p == '0'); /* removing successive '0's */
    }
    while (p < end && n < e) prev = *n++ = *p++;

    /* processing characters after the `*end` */
    while (*p) {
      if (*p == '_') {
        /* remove an underscore between digits */
        ++p;
        if (n == buf ||  /* '_' is head */
            !ISDIGIT(prev) || !ISDIGIT(*p)  /* '_' not enclosed by digits */
            ) {
          if (badcheck) goto bad;
          break;
        }
      }

      prev = *p++; /* this is not '_' */

      /* when the first exponent symbol is comming */
      if (e == init_e && (prev == 'e' || prev == 'E' || prev == 'p' || prev == 'P')) {
        e = buf + sizeof(buf) - 1;  /* make `e` not equal to `init_e` */
        *n++ = prev;
        switch (*p) { case '+': case '-': prev = *n++ = *p++; }
        if (*p == '0') {
          prev = *n++ = '0';
          while (*++p == '0'); /* removing successive '0's */
        }
        continue;
      }
      else if (ISSPACE(prev)) {
        while (ISSPACE(*p)) ++p;
        if (*p) {
          /* when a non-space character after spaces */
          if (badcheck) goto bad;
          break;
        }
      }
      else if (prev == '.' ? dot_seen++ : !ISDIGIT(prev)) {
        /* when the 2nd time dot or not a digit */
        if (badcheck) goto bad;
        break;
      }
      if (n < e) *n++ = prev;
    }
    *n = '\0';
    p = buf;

    /* Reject 0x... and 0X... */
    if (!badcheck && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
      return 0.0;
    }

    d = strtof(p, &end);
    if (errno == ERANGE) {
      OutOfRange();
      rb_warning("Float32 %.*s%s out of range", w, p, ellipsis);
      errno = 0;
    }
    if (badcheck) {
      if (!end || p == end) goto bad;
      while (*end && ISSPACE(*end)) end++;
      if (*end) goto bad;
    }
  }
  if (errno == ERANGE) {
    errno = 0;
    OutOfRange();
    rb_raise(rb_eArgError, "Float32 %.*s%s out of range", w, q, ellipsis);
  }
  return d;
}

static float
rb_str_to_flt(VALUE str, int badcheck, int raise, int *error)
{
  char *s;
  long len;
  float ret;
  VALUE v = 0;

  StringValue(str);
  s = RSTRING_PTR(str);
  len = RSTRING_LEN(str);
  if (s) {
    if (badcheck && memchr(s, '\0', len)) {
      if (raise) {
        rb_raise(rb_eArgError, "string for Float32 contains null byte");
      }
      else {
        if (error) *error = 1;
        return 0.0f;
      }
    }
    if (s[len]) { /* no sentinel somehow */
      char *p = ALLOCV(v, (size_t)len + 1);
      MEMCPY(p, s, char, len);
      p[len] = '\0';
      s = p;
    }
  }

  ret = rb_cstr_to_flt(s, badcheck, raise, error);
  if (v)
    ALLOCV_END(v);
  return ret;
}

static VALUE
rb_convert_to_float32(VALUE val, int raise_exception)
{
  float flt_value;
  double dbl_value;
  int err;

  if (SPECIAL_CONST_P(val)) {
    if (FIXNUM_P(val)) {
      flt_value = (float)FIX2LONG(val);
      return rb_float32_new(flt_value);
    }
    else if (FLONUM_P(val)) {
      return rb_float32_new_from_ruby_float(val);
    }
    else if (raise_exception) {
      switch (val) {
        case Qnil:
          rb_raise_static(rb_eTypeError, "can't convert nil into Float32");
        case Qtrue:
          rb_raise_static(rb_eTypeError, "can't convert true into Float32");
        case Qfalse:
          rb_raise_static(rb_eTypeError, "can't convert false into Float32");
      }
    }
  }
  else {
    int type = BUILTIN_TYPE(val);
    switch (type) {
      case T_FLOAT:
        return rb_float32_new_from_ruby_float(val);
      case T_BIGNUM:
        dbl_value = (float)rb_big2dbl(val);
        return rb_float32_new((float)dbl_value);
      case T_RATIONAL:
        return rb_float32_new_from_ruby_rational(val);
      case T_STRING:
        err = 0;
        flt_value = rb_str_to_flt(val, 1, raise_exception, &err);
        return err ? Qnil : rb_float32_new(flt_value);
    }
    if (raise_exception) {
      VALUE flt = rb_to_float(val);
      return rb_float32_new_from_ruby_float(flt);
    }
    else {
      VALUE flt = rb_check_to_float(val);
      if (RB_FLOAT_TYPE_P(flt)) {
        return rb_float32_new_from_ruby_float(flt);
      }
    }
  }
  return Qnil;
}

static int
opts_exception_p(VALUE opts)
{
    static ID kwds[1];
    VALUE exception;
    if (!kwds[0]) {
        kwds[0] = rb_intern("exception");
    }
    rb_get_kwargs(opts, kwds, 0, 1, &exception);
    return exception != Qfalse;
}

static VALUE
rb_f_float32(int argc, VALUE *argv, VALUE obj)
{
  VALUE arg = Qnil, opts = Qnil;

  rb_scan_args(argc, argv, "1:", &arg, &opts);
  return rb_convert_to_float32(arg, opts_exception_p(opts));
}

static VALUE
rb_float32_uminus(VALUE x)
{
  return rb_float32_new(-FLOAT32_VALUE(x));
}

static VALUE
rb_float32_plus(VALUE x, VALUE y)
{
  if (RB_TYPE_P(y, T_FIXNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) + (float)FIX2LONG(y));
  }
  else if (RB_TYPE_P(y, T_BIGNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) + (float)rb_big2dbl(y));
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    return rb_float32_new(FLOAT32_VALUE(x) + FLOAT32_VALUE(y));
  }
  else {
    return rb_num_coerce_bin(x, y, '+');
  }
}

static VALUE
rb_float32_minus(VALUE x, VALUE y)
{
  if (RB_TYPE_P(y, T_FIXNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) - (float)FIX2LONG(y));
  }
  else if (RB_TYPE_P(y, T_BIGNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) - (float)rb_big2dbl(y));
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    return rb_float32_new(FLOAT32_VALUE(x) - FLOAT32_VALUE(y));
  }
  else {
    return rb_num_coerce_bin(x, y, '-');
  }
}

static VALUE
rb_float32_mul(VALUE x, VALUE y)
{
  if (RB_TYPE_P(y, T_FIXNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) * (float)FIX2LONG(y));
  }
  else if (RB_TYPE_P(y, T_BIGNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) * (float)rb_big2dbl(y));
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    return rb_float32_new(FLOAT32_VALUE(x) * FLOAT32_VALUE(y));
  }
  else {
    return rb_num_coerce_bin(x, y, '*');
  }
}

static VALUE
rb_float32_div(VALUE x, VALUE y)
{
  if (RB_TYPE_P(y, T_FIXNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) / (float)FIX2LONG(y));
  }
  else if (RB_TYPE_P(y, T_BIGNUM)) {
    return rb_float32_new(FLOAT32_VALUE(x) / (float)rb_big2dbl(y));
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    return rb_float32_new(FLOAT32_VALUE(x) / FLOAT32_VALUE(y));
  }
  else {
    return rb_num_coerce_bin(x, y, '/');
  }
}

NORETURN(static void num_funcall_op_1_recursion(VALUE x, ID func, VALUE y));

static void
num_funcall_op_1_recursion(VALUE x, ID func, VALUE y)
{
    const char *name = rb_id2name(func);
    if (ISALNUM(name[0])) {
	rb_name_error(func, "%"PRIsVALUE".%"PRIsVALUE"(%"PRIsVALUE")",
		      x, ID2SYM(func), y);
    }
    else {
	rb_name_error(func, "%"PRIsVALUE"%"PRIsVALUE"%"PRIsVALUE,
		      x, ID2SYM(func), y);
    }
}

static VALUE
num_funcall_op_1(VALUE y, VALUE arg, int recursive)
{
    ID func = (ID)((VALUE *)arg)[0];
    VALUE x = ((VALUE *)arg)[1];
    if (recursive) {
	num_funcall_op_1_recursion(x, func, y);
    }
    return rb_funcall(x, func, 1, y);
}

VALUE
rb_float32_equal(VALUE x, VALUE y)
{
  volatile float a, b;

  if (FIXNUM_P(y)) {
    b = (float)FIX2LONG(y);
  }
  else if (RB_TYPE_P(y, T_BIGNUM)) {
    float ai, af;
    VALUE ab;
    a = FLOAT32_VALUE(x);
    af = modff(a, &ai);
    if (af != 0) return Qfalse;

    ab = rb_dbl2big((double)ai);
    return rb_big_eq(y, ab);
  }
  else if (RB_FLOAT_TYPE_P(y)) {
    double d = RFLOAT_VALUE(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnan(d)) return Qfalse;
#endif
    b = (float)d;
  }
  else if (RB_TYPE_P(y, T_RATIONAL)) {
    double d = rat2dbl_without_to_f(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnan(d)) return Qfalse;
#endif
    b = (float)d;
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    b = FLOAT32_VALUE(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnanf(b)) return Qfalse;
#endif
  }
  else if (x == y) return Qtrue;
  else {
    VALUE args[2], result;
    args[0] = (VALUE)rb_intern("==");
    args[1] = x;
    result = rb_exec_recursive_paired(num_funcall_op_1, y, x, (VALUE)args);
    if (RTEST(result)) return Qtrue;
    return Qfalse;
  }

  a = FLOAT32_VALUE(x);
#if defined(_MSC_VER) && _MSC_VER < 1300
  if (isnanf(a)) return Qfalse;
#endif
  return (a == b) ? Qtrue : Qfalse;
}

static VALUE
rb_integer_float32_cmp(VALUE x, VALUE y)
{
  float yd = FLOAT32_VALUE(y);
  float yi, yf;
  VALUE rel;

  if (isnan(yd)) return Qnil;
  if (isinf(yd)) {
    if (yd > 0.0) return INT2FIX(-1);
    else return INT2FIX(1);
  }
  yf = modff(yd, &yi);
  if (FIXNUM_P(x)) {
#if SIZEOF_LONG * CHAR_BIT < FLT_MANT_DIG /* assume FLT_RADIX == 2 */
    float xd = (float)FIX2LONG(x);
    if (xd < yd)
      return INT2FIX(-1);
    if (xd > yd)
      return INT2FIX(1);
    return INT2FIX(0);
#else
    long xn, yn;
    if (yi < FIXNUM_MIN)
      return INT2FIX(1);
    if (FIXNUM_MAX+1 <= yi)
      return INT2FIX(-1);
    xn = FIX2LONG(x);
    yn = (long)yi;
    if (xn < yn)
      return INT2FIX(-1);
    if (xn > yn)
      return INT2FIX(1);
    if (yf < 0.0)
      return INT2FIX(1);
    if (0.0 < yf)
      return INT2FIX(-1);
    return INT2FIX(0);
#endif
  }

  y = rb_dbl2big(yi);
  rel = rb_big_cmp(x, y);
  if (yf == 0.0 || rel != INT2FIX(0))
    return rel;
  if (yf < 0.0)
    return INT2FIX(1);
  return INT2FIX(-1);
}

static VALUE
rb_float32_gt(VALUE x, VALUE y)
{
  float a, b;

  a = FLOAT32_VALUE(x);
  if (RB_INTEGER_TYPE_P(y)) {
    VALUE rel = rb_integer_float32_cmp(y, x);
    if (FIXNUM_P(rel))
      return -FIX2INT(rel) > 0 ? Qtrue : Qfalse;
    return Qfalse;
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    b = FLOAT32_VALUE(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnan(b)) return Qfalse;
#endif
  }
  else {
    return rb_num_coerce_relop(x, y, '>');
  }

#if defined(_MSC_VER) && _MSC_VER < 1300
  if (isnan(a)) return Qfalse;
#endif
  return (a > b) ? Qtrue : Qfalse;
}

static VALUE
rb_float32_ge(VALUE x, VALUE y)
{
  float a, b;

  a = FLOAT32_VALUE(x);
  if (RB_INTEGER_TYPE_P(y)) {
    VALUE rel = rb_integer_float32_cmp(y, x);
    if (FIXNUM_P(rel))
      return -FIX2INT(rel) >= 0 ? Qtrue : Qfalse;
    return Qfalse;
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    b = FLOAT32_VALUE(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnan(b)) return Qfalse;
#endif
  }
  else {
    return rb_num_coerce_relop(x, y, rb_intern(">="));
  }

#if defined(_MSC_VER) && _MSC_VER < 1300
  if (isnan(a)) return Qfalse;
#endif
  return (a >= b) ? Qtrue : Qfalse;
}

static VALUE
rb_float32_lt(VALUE x, VALUE y)
{
  float a, b;

  a = FLOAT32_VALUE(x);
  if (RB_INTEGER_TYPE_P(y)) {
    VALUE rel = rb_integer_float32_cmp(y, x);
    if (FIXNUM_P(rel))
      return -FIX2INT(rel) < 0 ? Qtrue : Qfalse;
    return Qfalse;
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    b = FLOAT32_VALUE(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnan(b)) return Qfalse;
#endif
  }
  else {
    return rb_num_coerce_relop(x, y, '<');
  }

#if defined(_MSC_VER) && _MSC_VER < 1300
  if (isnan(a)) return Qfalse;
#endif
  return (a < b) ? Qtrue : Qfalse;
}

static VALUE
rb_float32_le(VALUE x, VALUE y)
{
  float a, b;

  a = FLOAT32_VALUE(x);
  if (RB_INTEGER_TYPE_P(y)) {
    VALUE rel = rb_integer_float32_cmp(y, x);
    if (FIXNUM_P(rel))
      return -FIX2INT(rel) <= 0 ? Qtrue : Qfalse;
    return Qfalse;
  }
  else if (RB_FLOAT32_TYPE_P(y)) {
    b = FLOAT32_VALUE(y);
#if defined(_MSC_VER) && _MSC_VER < 1300
    if (isnan(b)) return Qfalse;
#endif
  }
  else {
    return rb_num_coerce_relop(x, y, rb_intern("<="));
  }

#if defined(_MSC_VER) && _MSC_VER < 1300
  if (isnan(a)) return Qfalse;
#endif
  return (a <= b) ? Qtrue : Qfalse;
}

static VALUE
rb_float32_nan_p(VALUE x)
{
  return isnan(FLOAT32_VALUE(x)) ? Qtrue : Qfalse;
}

static VALUE
rb_float32_next_float(VALUE vx)
{
  float x, y;
  x = FLOAT32_VALUE(vx);
  y = nextafterf(x, HUGE_VALF);
  return rb_float32_new(y);
}

void
Init_float32(void)
{
  rb_cFloat32 = rb_define_class("Float32", rb_cNumeric);

  rb_undef_alloc_func(rb_cFloat32);
  rb_undef_method(CLASS_OF(rb_cFloat32), "new");

  rb_define_global_function("Float32", rb_f_float32, -1);

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

  rb_define_method(rb_cFloat32, "-@", rb_float32_uminus, 0);
  rb_define_method(rb_cFloat32, "+", rb_float32_plus, 1);
  rb_define_method(rb_cFloat32, "-", rb_float32_minus, 1);
  rb_define_method(rb_cFloat32, "*", rb_float32_mul, 1);
  rb_define_method(rb_cFloat32, "/", rb_float32_div, 1);
  rb_define_method(rb_cFloat32, "==", rb_float32_equal, 1);
  rb_define_method(rb_cFloat32, ">", rb_float32_gt, 1);
  rb_define_method(rb_cFloat32, ">=", rb_float32_ge, 1);
  rb_define_method(rb_cFloat32, "<", rb_float32_lt, 1);
  rb_define_method(rb_cFloat32, "<=", rb_float32_le, 1);
  rb_define_method(rb_cFloat32, "nan?", rb_float32_nan_p, 0);
  rb_define_method(rb_cFloat32, "next_float", rb_float32_next_float, 0);
}
