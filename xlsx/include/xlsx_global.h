#ifndef XLSX_GLOBAL_H
#define XLSX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(XLSX_LIBRARY)
#define XLSX_EXPORT Q_DECL_EXPORT
#else
#define XLSX_EXPORT Q_DECL_IMPORT
#endif

#endif // XLSX_GLOBAL_H
