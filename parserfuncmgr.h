#ifndef PARSERFUNCMGR_H
#define PARSERFUNCMGR_H

#include <list>
#include <string>


// 解析函数信息
class ParserFunc {
public:
    static const char * PARSE_SYMBOL;
    using Func = std::string(*)(const std::string&);

    std::string desc;
    std::string libPath;
};


class ParserFuncMgr
{
public:
    using FuncList = std::list<ParserFunc>;

    static ParserFuncMgr &inst();

    FuncList &funcList();

    bool loadFunc();

private:
    ParserFuncMgr();

private:
    FuncList _funcList;
};

#endif // PARSERFUNCMGR_H
