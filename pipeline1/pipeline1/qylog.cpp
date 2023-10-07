#include "qylog.h"
#include <mutex>
#include <Windows.h>

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif

#define isFull(r, w)	(r == ((w + 1) & LOG_SIZE_MASK))
#define isEmpty(r, w)	(r == w)

QYLog qylog;
QYend qyend;
static int g_qylogMask = 1;
static std::recursive_mutex mtx;

void setQylogMask(int mask) {
	g_qylogMask = mask;
}

void QYLog::saveLogFile(const std::string& str) {
}

void QYLog::backupLog() {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (logFile.is_open()) {
		logFile.close();
	}
}

QYLog& QYLog::operator << (float value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator << (double value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator<<(int value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator<<(unsigned int value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator<<(long value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator<<(unsigned long value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator<<(bool value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(std::to_string(value));
	return *this;
}

QYLog& QYLog::operator << (const char *value) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(value);
	return *this;
}

QYLog& QYLog::operator << (const std::string& str) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0)
		mLogArray[wptr].append(str);
	return *this;
}

QYLog& QYLog::operator << (QYend end) {
	std::lock_guard<std::recursive_mutex> lock(mtx);

	if (g_qylogMask != 0) {
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		char strTime[32];
		sprintf(strTime, "%4d-%02d-%02d %02d:%02d:%02d.%03d    ",
			sysTime.wYear, sysTime.wMonth, sysTime.wDay,
			sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
		mLogArray[wptr].insert(0, strTime);
		mLogArray[wptr].append("\n");

		if (g_qylogMask & 0x01) {
			std::cout << mLogArray[wptr];
		}

		if (g_qylogMask & 0x02) {
			saveLogFile(mLogArray[wptr]);
		}

		++wptr;
		wptr &= LOG_SIZE_MASK;
		mLogArray[wptr].clear(); // clear next log

		int rIdx = backup_rptr;
		int wIdx = wptr;
		if (isFull(rIdx, wIdx)) {
			backupLog();
			backup_rptr = wIdx;
		}
	}
	return *this;
}

#define LOG_BUF_SIZE	1024

void qyLogAssert(const char *format, ...) {
	//EvoFactory::instance().saveProjects("projects.xml");

	char buf[LOG_BUF_SIZE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, LOG_BUF_SIZE, format, ap);
	va_end(ap);

	qylog << buf << qyend;
}
