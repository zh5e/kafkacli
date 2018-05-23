#include "parserfuncmgr.h"

#include <string>
#include <fstream>

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


    QTextStream in(&cfgFile);
    while (!in.atEnd()) {
        ParserFunc func;
        func.desc = in.readLine().toStdString();
        const auto &funcSymbol = in.readLine();

        QLibrary lib(funcSymbol);
        if ()

        DLOG << "func desc: " << QString::fromStdString(func.desc)
             << ", func symbol: " << QString::fromStdString(funcSymbol);
    }

    return true;
}

ParserFuncMgr::ParserFuncMgr()
{

}
