#include "filterfuncloader.h"

FilterFuncLoader::FilterFuncLoader(const FilterFunc &funcInfo)
    : FilterFunc(funcInfo)
    , _lib(QString::fromStdString(funcInfo.libPath))
{
}

FilterFuncLoader::~FilterFuncLoader()
{
    _lib.unload();
}

FilterFunc::Func FilterFuncLoader::resove()
{
    return FilterFunc::Func(_lib.resolve(funcName.c_str()));
}
