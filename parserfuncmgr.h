#ifndef PARSERFUNCMGR_H
#define PARSERFUNCMGR_H

#include <list>
#include <string>
#include <functional>


// 解析函数信息
class ParserFunc {
public:
    using Func = std::function<std::string(const std::string&)>;
    // using Func = std::string(*)(const std::string&);

    std::string desc;
    Func func;
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
