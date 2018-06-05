#ifndef FILTERFUNCLOADER_H
#define FILTERFUNCLOADER_H

#include <QLibrary>

#include "filterfunc.h"


class FilterFuncLoader : public FilterFunc
{
public:
    FilterFuncLoader(const FilterFunc &funcInfo);
    ~FilterFuncLoader();

    Func resove();

private:
    QLibrary _lib;
};

#endif // FILTERFUNCLOADER_H
