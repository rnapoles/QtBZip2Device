#ifndef QTBZIP2LIB_GLOBAL_H
#define QTBZIP2LIB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_BZIP2_DEMO
#   if defined(QTBZIP2LIB_LIBRARY)
#     define QTBZIP2LIBSHARED_EXPORT Q_DECL_EXPORT
#   else
#     define QTBZIP2LIBSHARED_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define QTBZIP2LIBSHARED_EXPORT
#endif


#endif // QTBZIP2LIB_GLOBAL_H
