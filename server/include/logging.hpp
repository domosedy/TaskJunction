#ifndef LOGGING_HPP_
#define LOGGING_HPP_

#define FILENAME                                                               \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define rDebug QMessageLogger(FILENAME, __LINE__, Q_FUNC_INFO).debug
#define rInfo QMessageLogger(FILENAME, __LINE__, Q_FUNC_INFO).info
#define rWarning QMessageLogger(FILENAME, __LINE__, Q_FUNC_INFO).warning
#define rCritical QMessageLogger(FILENAME, __LINE__, Q_FUNC_INFO).critical
#define rFatal QMessageLogger(FILENAME, __LINE__, Q_FUNC_INFO).fatal

#endif
