kafka查看工具

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
- 消息窗口，显示消息数量以及内容

# kafka消息解析器

- 通过加载指定的动态库解析原始消息，输出json数据
- 解析动态库~/.kafkacli/*.dlib下
- 消息窗口会列出解析库


## 解析函数配置

- 配置文件路径~/.kafkacli/kafkacli.cfg
- 解析函数说明与解析库路径各占一行
