#ifndef PARSERFUNCLOADER_H
#define PARSERFUNCLOADER_H

#include <QLibrary>

#include "parserfunc.h"


class ParserFuncLoader : public ParserFunc {
public:
    ParserFuncLoader(const ParserFunc &func)
        : ParserFunc(func)
        , _lib(QString::fromStdString(func.libPath)) {
    }

    ~ParserFuncLoader() {
        _lib.unload();
    }

    Func resolve() {
        return Func(_lib.resolve(funcName.c_str()));
    }

private:
    QLibrary _lib;
};

#endif // PARSERFUNCLOADER_H
