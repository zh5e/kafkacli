#ifndef FILTERFUNC_H
#define FILTERFUNC_H

#include <string>
#include <vector>


// 过滤函数信息
struct FilterFunc {
    // 过滤方法类型定义, 消息，匹配内容
    using Func = bool (*)(const std::string &, const std::string &);
    using List = std::vector<FilterFunc>;

    std::string desc;
    std::string libPath;
    std::string funcName;
};


#endif // FILTERFUNC_H
