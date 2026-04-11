#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static int out_num(char *out, size_t n, size_t *out_idx, unsigned int val, int base, int is_signed) {
    char buf[32];
    int i = 0;
    int count = 0;

    if (is_signed && (int)val < 0 && base == 10) {
        if (*out_idx < n - 1) {
            out[(*out_idx)++] = '-';
        }
        val = (unsigned int)(-(int)val);
        count++;
    }

    if (val == 0) {
        if (*out_idx < n - 1) {
            out[(*out_idx)++] = '0';
        }
        return count + 1;
    }

    while (val > 0) {
        unsigned int rem = val % base;
        buf[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        val /= base;
    }

    while (i > 0) {
        if (*out_idx < n - 1) {
            out[(*out_idx)++] = buf[--i];
        } else {
            i--; 
        }
        count++;
    }
    return count;
}

int printf(const char *fmt, ...) {
    char buf[2048]; 
    va_list ap;
    va_start(ap, fmt);
    int ret = vsprintf(buf, fmt, ap);
    va_end(ap);
    
    for (int i = 0; i < ret; i++) {
        putch(buf[i]);
    }
    
    return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsprintf(out, fmt, ap);
    va_end(ap);
    return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(out, n, fmt, ap);
    va_end(ap);
    return ret; 
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  size_t out_idx = 0;
  const char *p = fmt;
  while (*p) {
    if (*p != '%') {
        if (out_idx < n - 1) out[out_idx++] = *p;
        p++;
        continue;
    }

    p++; 

    char pad_char = ' '; 
    if (*p == '0') {
        pad_char = '0';
        p++;
    }

    int width = 0;
    while (*p >= '0' && *p <= '9') {
        width = width * 10 + (*p - '0');
        p++;
    }

    switch (*p) {
        case 'd': {
            int val = va_arg(ap, int);
            
            if (val < 0) {
                if (out_idx < n - 1) out[out_idx++] = '-';
                val = -val; 
            }

            char num_buf[32];
            int i = 0;
            do {
                num_buf[i++] = (val % 10) + '0';
                val /= 10;
            } while (val > 0);

            while (i < width) {
                if (out_idx < n - 1) out[out_idx++] = pad_char;
                width--;
            }

            while (i > 0) {
                if (out_idx < n - 1) out[out_idx++] = num_buf[--i];
            }
            break;
        }

        case 'c': {
            char c = (char)va_arg(ap, int);
            if (out_idx < n - 1) out[out_idx++] = c;
            break;
        }
        case 'x': {
            unsigned int val = va_arg(ap, unsigned int);
            out_num(out, n, &out_idx, val, 16, 0); 
            break;
        }
        case 's': {
            char *str = va_arg(ap, char*);
            if (!str) str = "(null)";
            while (*str) {
                if (out_idx < n - 1) out[out_idx++] = *str;
                str++;
            }
            break;
        }
        default: {
            if (out_idx < n - 1) out[out_idx++] = *p;
            break;
        }
    }
    p++; 
}

if (n > 0) {
    out[out_idx < n ? out_idx : n - 1] = '\0';
}

return out_idx;    
}

#endif
