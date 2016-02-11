#include <QtCore/QtGlobal>

#if defined(WAMP_LIBRARY)
#  define WAMP_EXPORT Q_DECL_EXPORT
#else
#  define WAMP_EXPORT Q_DECL_IMPORT
#endif


