#include "parserfuncmgr.h"

#include <string>
#include <fstream>
#include <cassert>

#include <QDir>
#include <QFile>

#include "logger.h"

const char* ParserFunc::PARSE_SYMBOL = "parserKafkaMessage";


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
    _funcList.push_back(defaultParser);

    QTextStream in(&cfgFile);
    while (!in.atEnd()) {
        ParserFunc func;
        func.desc = in.readLine().toStdString();
        func.libPath = in.readLine().toStdString();

        DLOG << "func desc: " << QString::fromStdString(func.desc)
             << ", lib path: " << QString::fromStdString(func.libPath);

        _funcList.push_back(func);
    }

    return true;
}

ParserFuncMgr::ParserFuncMgr()
{

}
