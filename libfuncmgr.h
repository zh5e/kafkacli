#ifndef PARSERFUNCMGR_H
#define PARSERFUNCMGR_H

#include <list>
#include <string>


// 解析函数信息
struct ParserFunc {
    static const char * PARSE_SYMBOL;
    // 解析方法类型定义
    using Func = std::string(*)(const std::string&);
    using List = std::list<ParserFunc>;

    std::string desc;
    std::string libPath;
};

// 过滤函数信息
struct FilterFunc {
    // 过滤方法类型定义, 消息，匹配内容
    using Func = bool (*)(const std::string &, const std::string &);
    using List = std::list<FilterFunc>;

    std::string desc;
    std::string libPath;
    std::string funcName;
};


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
