#ifndef PARSERFUNC_H
#define PARSERFUNC_H

#include <string>
#include <vector>


// 解析函数信息
struct ParserFunc {
    static const char * PARSE_SYMBOL;
    // 解析方法类型定义
    using Func = std::string(*)(const std::string&);
    using List = std::vector<ParserFunc>;

    std::string desc;
    std::string libPath;
};


#endif // PARSERFUNC_H
