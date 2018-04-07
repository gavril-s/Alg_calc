#ifndef __fsm_h
#define __fsm_h

/**********************************

#ifndef __fsm_h
	#include "fsm.h"
#endif

**********************************/

#define WHITE_SPACES \
	     L' ': case L'\t'

#define END_OF_LINE \
	     L'\r': case L'\n'

#define COMMA L','

#define OPEN_PARENTH L'('
#define CLOSE_PARENTH L')'
#define COLON L':'

#define DECIMAL_DIGITS \
	     L'0': case L'1': case L'2': case L'3': case L'4': \
	case L'5': case L'6': case L'7': case L'8': case L'9'

#define DECIMAL_DIGITS_NO_ZERO \
	     L'1': case L'2': case L'3': case L'4': \
	case L'5': case L'6': case L'7': case L'8': case L'9'

#define VALUE_START \
	     DECIMAL_DIGITS: \
	case L'+': case L'-': case L'.'

#define VALUE_CONTINUE \
	     DECIMAL_DIGITS: \
	case L'+': case L'-': case L'.': case L'e': case L'E'

#define HEXADECIMAL_CAPITAL \
	     L'A': case L'B': case L'C': case L'D': case L'E': case L'F'

#define HEXADECIMAL_SMALL \
	     L'a': case L'b': case L'c': case L'd': case L'e': case L'f'

#define LATIN_CAPITAL \
	     L'A': case L'B': case L'C': case L'D': case L'E': case L'F': \
	case L'G': case L'H': case L'I': case L'J': case L'K': case L'L': \
	case L'M': case L'N': case L'O': case L'P': case L'Q': case L'R': \
	case L'S': case L'T': case L'U': case L'V': case L'W': case L'X': \
	case L'Y': case L'Z'

#define LATIN_SMALL \
	     L'a': case L'b': case L'c': case L'd': case L'e': case L'f': \
	case L'g': case L'h': case L'i': case L'j': case L'k': case L'l': \
	case L'm': case L'n': case L'o': case L'p': case L'q': case L'r': \
	case L's': case L't': case L'u': case L'v': case L'w': case L'x': \
	case L'y': case L'z'

#define LATIN \
	     LATIN_CAPITAL: \
	case LATIN_SMALL

#define COMMENT_START \
	     L'#'

#define BACKSLASH \
	     L'\\'

#define ID_START \
	     LATIN: case L'_'

#define ID_NEXT \
	     LATIN: case L'_': case DECIMAL_DIGITS

#define WILDCARDS \
		L'*': case L'?': case L'[': case L']': case L'-'

#endif
