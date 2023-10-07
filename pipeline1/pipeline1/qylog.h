#ifndef	QYLOG_H
#define QYLOG_H

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <time.h>
#include <thread>

typedef struct QYend{
} QYend;

#define LOG_SIZE		16384	// should be 2^x
#define	LOG_SIZE_MASK	(LOG_SIZE - 1)

class QYLog {
private:
	std::array<std::string, LOG_SIZE> mLogArray;
	int wptr;
	int backup_rptr;
	int logIdx;

	std::ofstream logFile;

private:
	void backupLog();
	void saveLogFile(const std::string& str);

public:
	QYLog() : wptr(0), backup_rptr(0), logIdx(0) { }

	~QYLog() { }

	QYLog& operator << (bool value);
	QYLog& operator << (int value);
	QYLog& operator << (unsigned int value);
	QYLog& operator << (long value);
	QYLog& operator << (unsigned long value);
	QYLog& operator << (float value);
	QYLog& operator << (double value);
	QYLog& operator << (const char *value);
	QYLog& operator << (const std::string& str);
	QYLog& operator << (QYend end);
};

extern QYLog qylog;
extern QYend qyend;
extern void  setQylogMask(int mask);

void	qyLogAssert(const char *format, ...);

#if defined(_DEBUG)
#define qy_assert( x, format, ... )		if (!((void)0, (x))) {	qyLogAssert("assert in %s: %s() at line %d: " format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); __debugbreak(); }
#define qy_abort( format, ... )		    {	qyLogAssert("abort in %s: %s() at line %d: " format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); __debugbreak(); }
#else
#define qy_assert( x, format, ... )		if (!((void)0, (x))) {  qyLogAssert("assert in %s: %s() at line %d: " format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); }
#define qy_abort( format, ... )		    {	qyLogAssert("abort in %s: %s() at line %d: " format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); }
#endif

#endif // QYLOG_H
