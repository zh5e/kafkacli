#ifndef PARSERFUNC_H
#define PARSERFUNC_H

#include <string>
#include <vector>


// 解析函数信息
struct ParserFunc {
    // 解析方法类型定义
    using Func = std::string(*)(const std::string&);
    using List = std::vector<ParserFunc>;

    std::string desc;
    std::string libPath;
    std::string funcName;
};


#endif // PARSERFUNC_H
