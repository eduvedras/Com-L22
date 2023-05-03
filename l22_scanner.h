#ifndef __L22SCANNER_H__
#define __L22SCANNER_H__

#undef yyFlexLexer
#define yyFlexLexer l22_scanner_FlexLexer
#include <FlexLexer.h>

typedef l22_scanner_FlexLexer l22_scanner;

#endif
