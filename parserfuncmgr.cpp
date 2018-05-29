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

        if (!QLibrary::isLibrary(funcSymbol)) {
            WLOG << funcSymbol << " not a library";
            return false;
        }

        QLibrary lib(funcSymbol);
        if (!lib.load()) {
            WLOG << "failed to load dynamic lib" << funcSymbol;
            return false;
        }

        if (!lib.isLoaded()) {
            WLOG << "failed to load dynamic lib" << funcSymbol;
            return false;
        }

        func.func = (ParserFunc::Func)(lib.resolve("parserKafkaMessage"));
        if (func.func && !func.desc.empty()) {
            _funcList.push_back(func);
        }

        DLOG << "func desc: " << QString::fromStdString(func.desc)
             << ", func symbol: " << funcSymbol;
    }

    return true;
}

ParserFuncMgr::ParserFuncMgr()
{

}
