/* Miscellaneous declarations.
   Copyright (C) 1995, 1996, 1997, 1998, 2003 Free Software Foundation, Inc.

This file is part of GNU Wget.

GNU Wget is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GNU Wget is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wget; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

In addition, as a special exception, the Free Software Foundation
gives permission to link the code of its release of Wget with the
OpenSSL project's "OpenSSL" library (or with modified versions of it
that use the same license as the "OpenSSL" library), and distribute
the linked executables.  You must obey the GNU General Public License
in all respects for all of the code used other than "OpenSSL".  If you
modify this file, you may extend this exception to your version of the
file, but you are not obligated to do so.  If you do not wish to do
so, delete this exception statement from your version.  */

/* This file contains declarations that are universally useful and
   those that don't fit elsewhere.  It also includes sysdep.h which
   includes some often-needed system includes, like the obnoxious
   <time.h> inclusion.  */

#ifndef WGET_H
#define WGET_H

/* Disable assertions when debug support is not compiled in. */
#ifndef ENABLE_DEBUG
# define NDEBUG
#endif

#ifndef PARAMS
# if PROTOTYPES
#  define PARAMS(args) args
# else
#  define PARAMS(args) ()
# endif
#endif

/* `gettext (FOO)' is long to write, so we use `_(FOO)'.  If NLS is
   unavailable, _(STRING) simply returns STRING.  */
#ifdef HAVE_NLS
# define _(string) gettext (string)
# ifdef HAVE_LIBINTL_H
#  include <libintl.h>
# endif /* HAVE_LIBINTL_H */
#else  /* not HAVE_NLS */
# define _(string) (string)
#endif /* not HAVE_NLS */

/* A pseudo function call that serves as a marker for the automated
   extraction of messages, but does not call gettext().  The run-time
   translation is done at a different place in the code.  The purpose
   of the N_("...") call is to make the message snarfer aware that the
   "..." string needs to be translated.  STRING should be a string
   literal.  Concatenated strings and other string expressions won't
   work.  The macro's expansion is not parenthesized, so that it is
   suitable as initializer for static 'char[]' or 'const char[]'
   variables.  -- explanation partly taken from GNU make.  */
#define N_(string) string

/* I18N NOTE: You will notice that none of the DEBUGP messages are
   marked as translatable.  This is intentional, for a few reasons:

   1) The debug messages are not meant for the users to look at, but
   for the developers; as such, they should be considered more like
   source comments than real program output.

   2) The messages are numerous, and yet they are random and frivolous
   ("double yuck!" and such).  There would be a lot of work with no
   gain.

   3) Finally, the debug messages are meant to be a clue for me to
   debug problems with Wget.  If I get them in a language I don't
   understand, debugging will become a new challenge of its own!  */


/* Include these, so random files need not include them.  */
#include "sysdep.h"
#include "options.h"
/* locale independent replacement for ctype.h */
#include "safe-ctype.h"

#define DO_NOTHING do {} while (0)

/* Print X if debugging is enabled; a no-op otherwise.  */
#ifdef ENABLE_DEBUG
# define DEBUGP(x) do { if (opt.debug) { debug_logprintf x; } } while (0)
#else  /* not ENABLE_DEBUG */
# define DEBUGP(x) DO_NOTHING
#endif /* not ENABLE_DEBUG */

/* Everything uses this, so include them here directly.  */
#include "xmalloc.h"

/* Likewise for logging functions.  */
#include "log.h"

/* Useful macros used across the code: */

/* The number of elements in an array.  For example:
   static char a[] = "foo";     -- countof(a) == 4 (note terminating \0)
   int a[5] = {1, 2};           -- countof(a) == 5
   char *a[] = {                -- countof(a) == 3
     "foo", "bar", "baz"
   }; */
#define countof(array) (sizeof (array) / sizeof ((array)[0]))

/* Zero out a value.  */
#define xzero(x) memset (&(x), '\0', sizeof (x))

/* Convert an ASCII hex digit to the corresponding number between 0
   and 15.  H should be a hexadecimal digit that satisfies isxdigit;
   otherwise, the result is undefined.  */
#define XDIGIT_TO_NUM(h) ((h) < 'A' ? (h) - '0' : TOUPPER (h) - 'A' + 10)
#define X2DIGITS_TO_NUM(h1, h2) ((XDIGIT_TO_NUM (h1) << 4) + XDIGIT_TO_NUM (h2))

/* The reverse of the above: convert a number in the [0, 16) range to
   the ASCII representation of the corresponding hexadecimal digit.
   `+ 0' is there so you can't accidentally use it as an lvalue.  */
#define XNUM_TO_DIGIT(x) ("0123456789ABCDEF"[x] + 0)
#define XNUM_TO_digit(x) ("0123456789abcdef"[x] + 0)

/* Copy the data delimited with BEG and END to alloca-allocated
   storage, and zero-terminate it.  Arguments are evaluated only once,
   in the order BEG, END, PLACE.  */
#define BOUNDED_TO_ALLOCA(beg, end, place) do {	\
  const char *BTA_beg = (beg);			\
  int BTA_len = (end) - BTA_beg;		\
  char **BTA_dest = &(place);			\
  *BTA_dest = alloca (BTA_len + 1);		\
  memcpy (*BTA_dest, BTA_beg, BTA_len);		\
  (*BTA_dest)[BTA_len] = '\0';			\
} while (0)

/* Return non-zero if string bounded between BEG and END is equal to
   STRING_LITERAL.  The comparison is case-sensitive.  */
#define BOUNDED_EQUAL(beg, end, string_literal)				\
  ((end) - (beg) == sizeof (string_literal) - 1				\
   && !memcmp (beg, string_literal, sizeof (string_literal) - 1))

/* The same as above, except the comparison is case-insensitive. */
#define BOUNDED_EQUAL_NO_CASE(beg, end, string_literal)			\
  ((end) - (beg) == sizeof (string_literal) - 1				\
   && !strncasecmp (beg, string_literal, sizeof (string_literal) - 1))

/* Like ptr=strdup(str), but allocates the space for PTR on the stack.
   This cannot be an expression because this is not portable:
     #define STRDUP_ALLOCA(str) (strcpy (alloca (strlen (str) + 1), str))
   The problem is that some compilers can't handle alloca() being an
   argument to a function.  */

#define STRDUP_ALLOCA(ptr, str) do {			\
  char **SA_dest = &(ptr);				\
  const char *SA_src = (str);				\
  *SA_dest = (char *)alloca (strlen (SA_src) + 1);	\
  strcpy (*SA_dest, SA_src);				\
} while (0)

/* Generally useful if you want to avoid arbitrary size limits but
   don't need a full dynamic array.  Assumes that BASEVAR points to a
   malloced array of TYPE objects (or possibly a NULL pointer, if
   SIZEVAR is 0), with the total size stored in SIZEVAR.  This macro
   will realloc BASEVAR as necessary so that it can hold at least
   NEEDED_SIZE objects.  The reallocing is done by doubling, which
   ensures constant amortized time per element.  */

#define DO_REALLOC(basevar, sizevar, needed_size, type)	do {		\
  long DR_needed_size = (needed_size);					\
  long DR_newsize = 0;							\
  while ((sizevar) < (DR_needed_size)) {				\
    DR_newsize = sizevar << 1;						\
    if (DR_newsize < 16)						\
      DR_newsize = 16;							\
    (sizevar) = DR_newsize;						\
  }									\
  if (DR_newsize)							\
    basevar = (type *)xrealloc (basevar, DR_newsize * sizeof (type));	\
} while (0)

extern const char *exec_name;

/* Document type ("dt") flags */
enum
{
  TEXTHTML             = 0x0001,	/* document is of type text/html
                                           or application/xhtml+xml */
  RETROKF              = 0x0002,	/* retrieval was OK */
  HEAD_ONLY            = 0x0004,	/* only send the HEAD request */
  SEND_NOCACHE         = 0x0008,	/* send Pragma: no-cache directive */
  ACCEPTRANGES         = 0x0010,	/* Accept-ranges header was found */
  ADDED_HTML_EXTENSION = 0x0020         /* added ".html" extension due to -E */
};

/* Universal error type -- used almost everywhere.  Error reporting of
   this detail is not generally used or needed and should be
   simplified.  */
typedef enum
{
  NOCONERROR, HOSTERR, CONSOCKERR, CONERROR, CONSSLERR,
  CONIMPOSSIBLE, NEWLOCATION, NOTENOUGHMEM, CONPORTERR,
  BINDERR, BINDOK, LISTENERR, ACCEPTERR, ACCEPTOK,
  CONCLOSED, FTPOK, FTPLOGINC, FTPLOGREFUSED, FTPPORTERR,
  FTPNSFOD, FTPRETROK, FTPUNKNOWNTYPE, FTPRERR,
  FTPREXC, FTPSRVERR, FTPRETRINT, FTPRESTFAIL, URLERROR,
  FOPENERR, FWRITEERR, HOK, HLEXC, HEOF,
  HERR, RETROK, RECLEVELEXC, FTPACCDENIED, WRONGCODE,
  FTPINVPASV, FTPNOPASV,
  CONTNOTSUPPORTED, RETRUNNEEDED, RETRFINISHED, READERR, TRYLIMEXC,
  URLBADPATTERN, FILEBADFILE, RANGEERR, RETRBADPATTERN,
  RETNOTSUP, ROBOTSOK, NOROBOTS, PROXERR, AUTHFAILED,
  QUOTEXC, WRITEFAILED,
  SSLERRCERTFILE,SSLERRCERTKEY,SSLERRCTXCREATE
} uerr_t;

/* In case old systems don't have EAFNOSUPPORT, which we use below. */
#ifndef EAFNOSUPPORT
# define EAFNOSUPPORT EINVAL
#endif

#define CONNECT_ERROR(err) ((   (err) == EAFNOSUPPORT		\
			     || (err) == EINVAL			\
			     || ((err) == ECONNREFUSED		\
				 && !opt.retry_connrefused))	\
			    ? CONIMPOSSIBLE : CONERROR)

#endif /* WGET_H */
