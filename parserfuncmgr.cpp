#include "parserfuncmgr.h"

#include <string>
#include <fstream>
#include <cassert>

#include <QDir>
#include <QFile>
#include <QLibrary>

#include "logger.h"


ParserFuncMgr &ParserFuncMgr::inst()
{
    static ParserFuncMgr inst;
    return inst;
}

ParserFuncMgr::FuncList &ParserFuncMgr::funcList()
{
    return _funcList;
}

bool ParserFuncMgr::loadFunc()
{
    QDir home = QDir::home().filePath(".kafkacli/kafkacli.cfg");
    home.makeAbsolute();
    QString absPath = home.absolutePath();

    DLOG << "config path: " << absPath;

    QFile cfgFile(absPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
           return false;

    ParserFunc defaultParser;
    defaultParser.desc = "默认解析器";
    defaultParser.func = [] (const std::string &message) -> std::string {
        return message;
    };

    _funcList.push_back(defaultParser);

    QTextStream in(&cfgFile);
    while (!in.atEnd()) {
        ParserFunc func;
        func.desc = in.readLine().toStdString();
        const auto &funcSymbol = in.readLine();


        continue;

        // QLibrary lib(funcSymbol);
        // auto pFunc = ParserFunc::Func(lib.resolve("parserKafkaMessage"));
        // func.func = ParserFunc::Func(lib.resolve("parserKafkaMessage"));
        // assert(func.func);

        // DLOG << "func desc: " << QString::fromStdString(func.desc)
        //      << ", func symbol: " << funcSymbol;

        // _funcList.push_back(func);
    }

    return true;
}

ParserFuncMgr::ParserFuncMgr()
{

}
