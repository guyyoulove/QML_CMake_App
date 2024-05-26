// xlsxglobal.h

#ifndef XLSXGLOBAL_H
#define XLSXGLOBAL_H

#include <cstdio>
#include <string>
#include <iostream>

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QIODevice>
#include <QByteArray>
#include <QStringList>
#include <QSharedPointer>
#include <QtCore/qglobal.h>

#if defined(XLSX_LIBRARY)
#define XLSX_EXPORT Q_DECL_EXPORT
#else
#define XLSX_EXPORT Q_DECL_IMPORT
#endif


#define QT_BEGIN_NAMESPACE_XLSX namespace QXlsx {
#define QT_END_NAMESPACE_XLSX }

#define QXLSX_USE_NAMESPACE using namespace QXlsx;

#if QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 )
#undef QT_BEGIN_MOC_NAMESPACE
#undef QT_END_MOC_NAMESPACE
#define QT_BEGIN_MOC_NAMESPACE namespace QXlsx {
#define QT_END_MOC_NAMESPACE }
#define QStringLiteral QLatin1String
#define Q_DECL_NOTHROW 
#endif



#endif // XLSXGLOBAL_H
