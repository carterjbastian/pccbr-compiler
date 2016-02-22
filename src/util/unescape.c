/*
 * sws, based on code by Wietse Venema, IBM T.J. Watson Research
 */
#include "unescape.h"

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char *unescape(const char *data)
{
    int     ch;
    int     oval;
    int     i;
    char *result, *rp;

#define UCHAR(cp)	((unsigned char *) (cp))
#define ISOCTAL(ch)	(isdigit(ch) && (ch) != '8' && (ch) != '9')


    assert(data);

    result = calloc(strlen(data)+1,sizeof(char));
    assert(result);
    rp = result;

    while ((ch = *UCHAR(data++)) != 0) {
      if (ch == '\\') {
	if ((ch = *UCHAR(data++)) == 0)
	  break;
	switch (ch) {
	case 'a':				/* \a -> audible bell */
	  ch = '\a';
	  break;
	case 'b':				/* \b -> backspace */
	  ch = '\b';
	  break;
	case 'f':				/* \f -> formfeed */
	  ch = '\f';
	  break;
	case 'n':				/* \n -> newline */
	  ch = '\n';
	  break;
	case 'r':				/* \r -> carriagereturn */
	  ch = '\r';
	  break;
	case 't':				/* \t -> horizontal tab */
	  ch = '\t';
	  break;
	case 'v':				/* \v -> vertical tab */
	  ch = '\v';
	  break;
	case '0':				/* \nnn -> ASCII value */
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	  for (oval = ch - '0', i = 0;
	       i < 2 && (ch = *UCHAR(data)) != 0 && ISOCTAL(ch);
	       i++, data++) {
	    oval = (oval << 3) | (ch - '0');
	  }
	  ch = oval;
	  break;
	default:				/* \any -> any */
	  break;
	}
      }
      *(rp++) = (unsigned char) ch;

    }

    return (result);
}

