#ifndef LOGGER_H
#define LOGGER_H

#include <QMessageLogger>
#include <QDebug>

#define DLOG \
        QMessageLogger(__FILE__, __LINE__, __func__).debug() << __FILE__ << ":" << __LINE__ << ":" << __func__ << " "

#endif // LOGGER_H
