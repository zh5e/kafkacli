![](https://raw.githubusercontent.com/zh5e/kafkacli/master/app.iconset/icon_128x128.png)

- Qt Create 4.6.1
- Qt 5.10.1

# 安装

安装librdkafka
```sh
brew install librdkafka
```

# 窗口

- 登陆窗口
- 主窗口，显示topic信息
- 分区详情窗口，显示消息数量以及内容

# kafka消息解析器

- 通过加载指定的动态库解析原始消息，输出json数据
- 应用HOME路径${HOME}/.kafkacli/
- 解析动态库${HOME}/.kafkacli/*.dlib下
- 消息窗口会列出解析库

## 消息解析函数

```c++
using Func = std::string(*)(const std::string&);

extern "C" {
std::string parserKafkaMessage(const std::string &message);
}
```

## 过滤消息函数

```c++
using Func = bool (*)(const std::string &, const std::string &);

extern "C" {
bool checkFileName(const std::string &message, const std::string &fileName);
}
```

## 配置文件


### 解析函数

```sh
cat ${HOME}/.kafkacli/parser_func.cfg

消息解析1
/Users/zh/.kafkacli/libxxxx.dylib
parserKafkaMessage
消息解析2
/Users/zh/.kafkacli/libzzzz.dylib
parserKafkaMessage
```

### 过滤函数

```sh
cat ${HOME}/.kafkacli/filter_func.cfg

过滤方法1（参数文件名）
/Users/zh/.kafkacli/libxxxx1.dylib
checkFileName
过滤方法2（参数用户名）
/Users/zh/.kafkacli/libxxxx2.dylib
checkUserName
```
