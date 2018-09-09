/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#include <wk/kernel.h>
#include <lib/stdarg.h>
#include <lib/string.h>

#define SIGN	1		/* unsigned/signed, must be 1 */
#define LEFT	2		/* left justified */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define ZEROPAD	16		/* pad with zero, must be 16 == '0' - ' ' */
#define SMALL	32		/* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL	64		/* prefix hex with "0x", octal with "0" */

enum format_type {
	FORMAT_TYPE_NONE, /* Just a string part */
	FORMAT_TYPE_WIDTH,
	FORMAT_TYPE_PRECISION,
	FORMAT_TYPE_CHAR,
	FORMAT_TYPE_STR,
	FORMAT_TYPE_PTR,
	FORMAT_TYPE_PERCENT_CHAR,
	FORMAT_TYPE_INVALID,
	FORMAT_TYPE_LONG_LONG,
	FORMAT_TYPE_ULONG,
	FORMAT_TYPE_LONG,
	FORMAT_TYPE_UBYTE,
	FORMAT_TYPE_BYTE,
	FORMAT_TYPE_USHORT,
	FORMAT_TYPE_SHORT,
	FORMAT_TYPE_UINT,
	FORMAT_TYPE_INT,
	FORMAT_TYPE_SIZE_T,
	FORMAT_TYPE_PTRDIFF
};

struct printf_spec {
	unsigned int	type:8;		/* format_type enum */
	signed int	field_width:24;	/* width of output field */
	unsigned int	flags:8;	/* flags to number() */
	unsigned int	base:8;		/* number base, 8, 10 or 16 only */
	signed int	precision:16;	/* # of digits/chars */
} __packed;
#define FIELD_WIDTH_MAX ((1 << 23) - 1)
#define PRECISION_MAX ((1 << 15) - 1)

static void move_right(char *buf, char *end, unsigned len, unsigned spaces)
{
	size_t size;
	if (buf >= end)	/* nowhere to put anything */
		return;
	size = end - buf;
	if (size <= spaces) {
		memset(buf, ' ', size);
		return;
	}
	if (len) {
		if (len > size - spaces)
			len = size - spaces;
		memmove(buf + spaces, buf, len);
	}
	memset(buf, ' ', spaces);
}

static noinline_for_stack
char *widen_string(char *buf, int n, char *end, struct printf_spec spec)
{
	unsigned spaces;

	if (likely(n >= spec.field_width))
		return buf;
	/* we want to pad the sucker */
	spaces = spec.field_width - n;
	if (!(spec.flags & LEFT)) {
		move_right(buf - n, end, n, spaces);
		return buf + spaces;
	}
	while (spaces--) {
		if (buf < end)
			*buf = ' ';
		++buf;
	}
	return buf;
}

static noinline_for_stack
char *string(char *buf, char *end, const char *s, struct printf_spec spec)
{
	int len = 0;
	size_t lim = spec.precision;

	if ((unsigned long)s < PAGE_SIZE)
		s = "(null)";

	while (lim--) {
		char c = *s++;
		if (!c)
			break;
		if (buf < end)
			*buf = c;
		++buf;
		++len;
	}
	return widen_string(buf, len, end, spec);
}

static noinline_for_stack
char *pointer(const char *fmt, char *buf, char *end, void *ptr,
	      struct printf_spec spec)
{
	const int default_width = 2 * sizeof(void *);

	if (!ptr && *fmt != 'K') {
		/*
		 * Print (null) with the same width as a pointer so it makes
		 * tabular output look nice.
		 */
		if (spec.field_width == -1)
			spec.field_width = default_width;
		return string(buf, end, "(null)", spec);
	}

	switch (*fmt) {
	case 'F':
	case 'f':
		ptr = dereference_function_descriptor(ptr);
		/* Fallthrough */
	case 'S':
	case 's':
	case 'B':
		return symbol_string(buf, end, ptr, spec, fmt);
	case 'R':
	case 'r':
		return resource_string(buf, end, ptr, spec, fmt);
	case 'h':
		return hex_string(buf, end, ptr, spec, fmt);
	case 'b':
		switch (fmt[1]) {
		case 'l':
			return bitmap_list_string(buf, end, ptr, spec, fmt);
		default:
			return bitmap_string(buf, end, ptr, spec, fmt);
		}
	case 'M':			/* Colon separated: 00:01:02:03:04:05 */
	case 'm':			/* Contiguous: 000102030405 */
					/* [mM]F (FDDI) */
					/* [mM]R (Reverse order; Bluetooth) */
		return mac_address_string(buf, end, ptr, spec, fmt);
	case 'I':			/* Formatted IP supported
					 * 4:	1.2.3.4
					 * 6:	0001:0203:...:0708
					 * 6c:	1::708 or 1::1.2.3.4
					 */
	case 'i':			/* Contiguous:
					 * 4:	001.002.003.004
					 * 6:   000102...0f
					 */
		switch (fmt[1]) {
		case '6':
			return ip6_addr_string(buf, end, ptr, spec, fmt);
		case '4':
			return ip4_addr_string(buf, end, ptr, spec, fmt);
		case 'S': {
			const union {
				struct sockaddr		raw;
				struct sockaddr_in	v4;
				struct sockaddr_in6	v6;
			} *sa = ptr;

			switch (sa->raw.sa_family) {
			case AF_INET:
				return ip4_addr_string_sa(buf, end, &sa->v4, spec, fmt);
			case AF_INET6:
				return ip6_addr_string_sa(buf, end, &sa->v6, spec, fmt);
			default:
				return string(buf, end, "(invalid address)", spec);
			}}
		}
		break;
	case 'E':
		return escaped_string(buf, end, ptr, spec, fmt);
	case 'U':
		return uuid_string(buf, end, ptr, spec, fmt);
	case 'V':
		{
			va_list va;

			va_copy(va, *((struct va_format *)ptr)->va);
			buf += vsnprintf(buf, end > buf ? end - buf : 0,
					 ((struct va_format *)ptr)->fmt, va);
			va_end(va);
			return buf;
		}
	case 'K':
		switch (kptr_restrict) {
		case 0:
			/* Always print %pK values */
			break;
		case 1: {
			const struct cred *cred;

			/*
			 * kptr_restrict==1 cannot be used in IRQ context
			 * because its test for CAP_SYSLOG would be meaningless.
			 */
			if (in_irq() || in_serving_softirq() || in_nmi()) {
				if (spec.field_width == -1)
					spec.field_width = default_width;
				return string(buf, end, "pK-error", spec);
			}

			/*
			 * Only print the real pointer value if the current
			 * process has CAP_SYSLOG and is running with the
			 * same credentials it started with. This is because
			 * access to files is checked at open() time, but %pK
			 * checks permission at read() time. We don't want to
			 * leak pointer values if a binary opens a file using
			 * %pK and then elevates privileges before reading it.
			 */
			cred = current_cred();
			if (!has_capability_noaudit(current, CAP_SYSLOG) ||
			    !uid_eq(cred->euid, cred->uid) ||
			    !gid_eq(cred->egid, cred->gid))
				ptr = NULL;
			break;
		}
		case 2:
		default:
			/* Always print 0's for %pK */
			ptr = NULL;
			break;
		}
		break;

	case 'N':
		return netdev_bits(buf, end, ptr, fmt);
	case 'a':
		return address_val(buf, end, ptr, fmt);
	case 'd':
		return dentry_name(buf, end, ptr, spec, fmt);
	case 'C':
		return clock(buf, end, ptr, spec, fmt);
	case 'D':
		return dentry_name(buf, end,
				   ((const struct file *)ptr)->f_path.dentry,
				   spec, fmt);
#ifdef CONFIG_BLOCK
	case 'g':
		return bdev_name(buf, end, ptr, spec, fmt);
#endif

	case 'G':
		return flags_string(buf, end, ptr, fmt);
	case 'O':
		switch (fmt[1]) {
		case 'F':
			return device_node_string(buf, end, ptr, spec, fmt + 1);
		}
	}
	spec.flags |= SMALL;
	if (spec.field_width == -1) {
		spec.field_width = default_width;
		spec.flags |= ZEROPAD;
	}
	spec.base = 16;

	return number(buf, end, (unsigned long) ptr, spec);
}

/*
 * Helper function to decode printf style format.
 * Each call decode a token from the format and return the
 * number of characters read (or likely the delta where it wants
 * to go on the next call).
 * The decoded token is returned through the parameters
 *
 * 'h', 'l', or 'L' for integer fields
 * 'z' support added 23/7/1999 S.H.
 * 'z' changed to 'Z' --davidm 1/25/99
 * 'Z' changed to 'z' --adobriyan 2017-01-25
 * 't' added for ptrdiff_t
 *
 * @fmt: the format string
 * @type of the token returned
 * @flags: various flags such as +, -, # tokens..
 * @field_width: overwritten width
 * @base: base of the number (octal, hex, ...)
 * @precision: precision of a number
 * @qualifier: qualifier of a number (long, size_t, ...)
 */
static noinline_for_stack
int format_decode(const char *fmt, struct printf_spec *spec)
{
	const char *start = fmt;
	char qualifier;

	/* we finished early by reading the field width */
	if (spec->type == FORMAT_TYPE_WIDTH) {
		if (spec->field_width < 0) {
			spec->field_width = -spec->field_width;
			spec->flags |= LEFT;
		}
		spec->type = FORMAT_TYPE_NONE;
		goto precision;
	}

	/* we finished early by reading the precision */
	if (spec->type == FORMAT_TYPE_PRECISION) {
		if (spec->precision < 0)
			spec->precision = 0;

		spec->type = FORMAT_TYPE_NONE;
		goto qualifier;
	}

	/* By default */
	spec->type = FORMAT_TYPE_NONE;

	for (; *fmt ; ++fmt) {
		if (*fmt == '%')
			break;
	}

	/* Return the current non-format string */
	if (fmt != start || !*fmt)
		return fmt - start;

	/* Process flags */
	spec->flags = 0;

	while (1) { /* this also skips first '%' */
		bool found = true;

		++fmt;

		switch (*fmt) {
		case '-': spec->flags |= LEFT;    break;
		case '+': spec->flags |= PLUS;    break;
		case ' ': spec->flags |= SPACE;   break;
		case '#': spec->flags |= SPECIAL; break;
		case '0': spec->flags |= ZEROPAD; break;
		default:  found = false;
		}

		if (!found)
			break;
	}

	/* get field width */
	spec->field_width = -1;

	if (isdigit(*fmt))
		spec->field_width = skip_atoi(&fmt);
	else if (*fmt == '*') {
		/* it's the next argument */
		spec->type = FORMAT_TYPE_WIDTH;
		return ++fmt - start;
	}

precision:
	/* get the precision */
	spec->precision = -1;
	if (*fmt == '.') {
		++fmt;
		if (isdigit(*fmt)) {
			spec->precision = skip_atoi(&fmt);
			if (spec->precision < 0)
				spec->precision = 0;
		} else if (*fmt == '*') {
			/* it's the next argument */
			spec->type = FORMAT_TYPE_PRECISION;
			return ++fmt - start;
		}
	}

qualifier:
	/* get the conversion qualifier */
	qualifier = 0;
	if (*fmt == 'h' || _tolower(*fmt) == 'l' ||
	    *fmt == 'z' || *fmt == 't') {
		qualifier = *fmt++;
		if (unlikely(qualifier == *fmt)) {
			if (qualifier == 'l') {
				qualifier = 'L';
				++fmt;
			} else if (qualifier == 'h') {
				qualifier = 'H';
				++fmt;
			}
		}
	}

	/* default base */
	spec->base = 10;
	switch (*fmt) {
	case 'c':
		spec->type = FORMAT_TYPE_CHAR;
		return ++fmt - start;

	case 's':
		spec->type = FORMAT_TYPE_STR;
		return ++fmt - start;

	case 'p':
		spec->type = FORMAT_TYPE_PTR;
		return ++fmt - start;

	case '%':
		spec->type = FORMAT_TYPE_PERCENT_CHAR;
		return ++fmt - start;

	/* integer number formats - set up the flags and "break" */
	case 'o':
		spec->base = 8;
		break;

	case 'x':
		spec->flags |= SMALL;

	case 'X':
		spec->base = 16;
		break;

	case 'd':
	case 'i':
		spec->flags |= SIGN;
	case 'u':
		break;

	case 'n':
		/*
		 * Since %n poses a greater security risk than
		 * utility, treat it as any other invalid or
		 * unsupported format specifier.
		 */
		/* Fall-through */

	default:
		spec->type = FORMAT_TYPE_INVALID;
		return fmt - start;
	}

	if (qualifier == 'L')
		spec->type = FORMAT_TYPE_LONG_LONG;
	else if (qualifier == 'l') {
		BUILD_BUG_ON(FORMAT_TYPE_ULONG + SIGN != FORMAT_TYPE_LONG);
		spec->type = FORMAT_TYPE_ULONG + (spec->flags & SIGN);
	} else if (qualifier == 'z') {
		spec->type = FORMAT_TYPE_SIZE_T;
	} else if (qualifier == 't') {
		spec->type = FORMAT_TYPE_PTRDIFF;
	} else if (qualifier == 'H') {
		BUILD_BUG_ON(FORMAT_TYPE_UBYTE + SIGN != FORMAT_TYPE_BYTE);
		spec->type = FORMAT_TYPE_UBYTE + (spec->flags & SIGN);
	} else if (qualifier == 'h') {
		BUILD_BUG_ON(FORMAT_TYPE_USHORT + SIGN != FORMAT_TYPE_SHORT);
		spec->type = FORMAT_TYPE_USHORT + (spec->flags & SIGN);
	} else {
		BUILD_BUG_ON(FORMAT_TYPE_UINT + SIGN != FORMAT_TYPE_INT);
		spec->type = FORMAT_TYPE_UINT + (spec->flags & SIGN);
	}

	return ++fmt - start;
}

static void
set_field_width(struct printf_spec *spec, int width)
{
	spec->field_width = width;
	if (spec->field_width != width) {
		spec->field_width = clamp(width, -FIELD_WIDTH_MAX, FIELD_WIDTH_MAX);
	}
}

static void
set_precision(struct printf_spec *spec, int prec)
{
	spec->precision = prec;
	if (spec->precision != prec) {
		spec->precision = clamp(prec, 0, PRECISION_MAX);
	}
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	unsigned long long num;
	char *str, *end;
	struct printf_spec spec = {0};

	/* Reject out-of-range values early.  Large positive sizes are
	   used for unknown buffer sizes. */
	if (size > INT_MAX)
		return 0;

	str = buf;
	end = buf + size;

	/* Make sure end is always >= buf */
	if (end < buf) {
		end = ((void *)-1);
		size = end - buf;
	}

	while (*fmt) {
		const char *old_fmt = fmt;
		int read = format_decode(fmt, &spec);

		fmt += read;

		switch (spec.type) {
		case FORMAT_TYPE_NONE: {
			int copy = read;
			if (str < end) {
				if (copy > end - str)
					copy = end - str;
				memcpy(str, old_fmt, copy);
			}
			str += read;
			break;
		}

		case FORMAT_TYPE_WIDTH:
			set_field_width(&spec, va_arg(args, int));
			break;

		case FORMAT_TYPE_PRECISION:
			set_precision(&spec, va_arg(args, int));
			break;

		case FORMAT_TYPE_CHAR: {
			char c;

			if (!(spec.flags & LEFT)) {
				while (--spec.field_width > 0) {
					if (str < end)
						*str = ' ';
					++str;

				}
			}
			c = (unsigned char) va_arg(args, int);
			if (str < end)
				*str = c;
			++str;
			while (--spec.field_width > 0) {
				if (str < end)
					*str = ' ';
				++str;
			}
			break;
		}

		case FORMAT_TYPE_STR:
			str = string(str, end, va_arg(args, char *), spec);
			break;

		case FORMAT_TYPE_PTR:
			str = pointer(fmt, str, end, va_arg(args, void *),
				      spec);
			while (isalnum(*fmt))
				fmt++;
			break;

		case FORMAT_TYPE_PERCENT_CHAR:
			if (str < end)
				*str = '%';
			++str;
			break;

		case FORMAT_TYPE_INVALID:
			/*
			 * Presumably the arguments passed gcc's type
			 * checking, but there is no safe or sane way
			 * for us to continue parsing the format and
			 * fetching from the va_list; the remaining
			 * specifiers and arguments would be out of
			 * sync.
			 */
			goto out;

		default:
			switch (spec.type) {
			case FORMAT_TYPE_LONG_LONG:
				num = va_arg(args, long long);
				break;
			case FORMAT_TYPE_ULONG:
				num = va_arg(args, unsigned long);
				break;
			case FORMAT_TYPE_LONG:
				num = va_arg(args, long);
				break;
			case FORMAT_TYPE_SIZE_T:
				if (spec.flags & SIGN)
					num = va_arg(args, ssize_t);
				else
					num = va_arg(args, size_t);
				break;
			case FORMAT_TYPE_PTRDIFF:
				num = va_arg(args, ptrdiff_t);
				break;
			case FORMAT_TYPE_UBYTE:
				num = (unsigned char) va_arg(args, int);
				break;
			case FORMAT_TYPE_BYTE:
				num = (signed char) va_arg(args, int);
				break;
			case FORMAT_TYPE_USHORT:
				num = (unsigned short) va_arg(args, int);
				break;
			case FORMAT_TYPE_SHORT:
				num = (short) va_arg(args, int);
				break;
			case FORMAT_TYPE_INT:
				num = (int) va_arg(args, int);
				break;
			default:
				num = va_arg(args, unsigned int);
			}

			str = number(str, end, num, spec);
		}
	}

out:
	if (size > 0) {
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}

	/* the trailing null byte doesn't count towards the total */
	return str-buf;

}

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	i = vsnprintf(buf, size, fmt, args);

	if (likely(i < size))
		return i;
	if (size != 0)
		return size - 1;
	return 0;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	return vsnprintf(buf, INT_MAX, fmt, args);
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, INT_MAX, fmt, args);
	va_end(args);

	return i;
}