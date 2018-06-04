#ifndef LOGGER_H
#define LOGGER_H

#include <QMessageLogger>
#include <QDebug>

#define DLOG \
        QMessageLogger(__FILE__, __LINE__, __func__).debug() << __FILE__ << ":" << __LINE__ << ":" << __func__ << " "

#define WLOG \
        QMessageLogger(__FILE__, __LINE__, __func__).warning() << __FILE__  << ":" << __LINE__ << ":" << __func__ << " "

#define ILOG DLOG
#define ELOG WLOG
#define FLOG WLOG

#endif // LOGGER_H
