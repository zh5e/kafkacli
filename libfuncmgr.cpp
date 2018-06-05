#include "libfuncmgr.h"

#include <string>
#include <fstream>
#include <cassert>

#include <QDir>
#include <QFile>
#include <QLibrary>

#include "logger.h"

const char * LibFuncMgr::PARSER_FUNC_CONFIG = ".kafkacli/parser_func.cfg";
const char * LibFuncMgr::FILTER_FUNC_CONFIG = ".kafkacli/filter_func.cfg";


LibFuncMgr &LibFuncMgr::inst()
{
    static LibFuncMgr inst;
    return inst;
}

ParserFunc::List &LibFuncMgr::parserFuncList()
{
    return _parserFuncList;
}

FilterFunc::List &LibFuncMgr::filterFuncList()
{
    return _filterFuncList;
}

bool LibFuncMgr::loadFuncConfig()
{
    return loadFilterFuncConfig() && loadParserFuncConfig();
}

bool LibFuncMgr::loadParserFuncConfig()
{
    QDir parserConfig = QDir::home().filePath(PARSER_FUNC_CONFIG);
    parserConfig.makeAbsolute();
    const QString absPath = parserConfig.absolutePath();

    DLOG << "parser config path: " << absPath;

    QFile cfgFile(absPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
           return false;

    ParserFunc defaultParser;
    defaultParser.desc = "默认解析器";
    _parserFuncList.push_back(defaultParser);

    QTextStream in(&cfgFile);
    while (!in.atEnd()) {
        ParserFunc func;
        func.desc = in.readLine().toStdString();
        func.libPath = in.readLine().toStdString();
        func.funcName = in.readLine().toStdString();

        DLOG << "func desc: " << QString::fromStdString(func.desc)
             << ", lib path: " << QString::fromStdString(func.libPath);

        _parserFuncList.push_back(func);
    }

    return true;
}

bool LibFuncMgr::loadFilterFuncConfig()
{
    QDir filterConfig = QDir::home().filePath(FILTER_FUNC_CONFIG);
    filterConfig.makeAbsolute();

    const QString absPath = filterConfig.absolutePath();

    DLOG << "filter config path: " << absPath;

    QFile cfgFile(absPath);
    if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    FilterFunc defaultFilter;
    defaultFilter.desc = "默认过滤器";
    defaultFilter.funcName = "defaultFilter";
    _filterFuncList.push_back(defaultFilter);

    QTextStream in(&cfgFile);
    while (!in.atEnd()) {
        FilterFunc func;
        func.desc = in.readLine().toStdString();
        func.libPath = in.readLine().toStdString();
        func.funcName = in.readLine().toStdString();

        DLOG << "func desc: " << QString::fromStdString(func.desc)
             << ", lib path: " << QString::fromStdString(func.libPath)
             << ", func name: " << QString::fromStdString(func.funcName);

        _filterFuncList.push_back(func);
    }

    return true;
}

LibFuncMgr::LibFuncMgr()
{

}
