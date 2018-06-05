#ifndef PARSERFUNCMGR_H
#define PARSERFUNCMGR_H

#include <list>
#include <string>

#include "filterfunc.h"
#include "parserfunc.h"


class LibFuncMgr
{
public:
    static const char *PARSER_FUNC_CONFIG;
    static const char *FILTER_FUNC_CONFIG;

    static LibFuncMgr &inst();

    bool loadFuncConfig();
    ParserFunc::List &parserFuncList();
    FilterFunc::List &filterFuncList();

private:

    bool loadParserFuncConfig();
    bool loadFilterFuncConfig();

    LibFuncMgr();

private:
    ParserFunc::List _parserFuncList;
    FilterFunc::List _filterFuncList;
};

#endif // PARSERFUNCMGR_H
