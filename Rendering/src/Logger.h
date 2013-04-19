#pragma once
#include <string>
#include "defs.h"

using namespace std;

namespace Logger
{
	void GLIB_API Init(string logFile);
	void GLIB_API Destroy(void);
	void GLIB_API Log(const string& tag, const string& message, const char* funcName, const char* sourceFile, int lineNum);
	void GLIB_API Error(const string& message, bool isFatal, const char* funcName, const char* sourceFile, int lineNum);
	void GLIB_API SetDisplayFlags(unsigned char flags);
}

#ifdef _DEBUG

#define GLIB_ASSERT(expr) \
	do \
	{ \
		if(!(expr)) \
		{ \
			Logger::Error(#expr, false, __FUNCTION__, __FILE__, __LINE__); \
		} \
	\
	} while(0) \

#define GLIB_INFO(str) \
	do \
	{ \
		Logger::Log("INFO", str, NULL,  NULL, 0); \
	} while(0) \

#define GLIB_LOG(tag, str) \
	do \
	{ \
		Logger::Log(tag, str, NULL,  NULL, 0); \
	} while(0) \

#define GLIB_ERROR(str) \
	do \
	{ \
		Logger::Error(str, false, __FUNCTION__,  __FILE__, __LINE__); \
	} while(0) \

#define GLIB_FATAL(str) \
	do \
	{ \
		Logger::Error(str, true, __FUNCTION__,  __FILE__, __LINE__); \
	} while(0) \

#define GLIB_WARNING(str) \
	do \
	{ \
		Logger::Log("WARNING", str, __FUNCTION__,  __FILE__, __LINE__); \
	} while(0) \

#endif

// Logger flags.
const unsigned char LOGFLAG_WRITE_TO_FILE = 1 << 0;
const unsigned char LOGFLAG_WRITE_TO_DEBUGGER = 1 << 1;
const unsigned char LOGFLAG_WRITE_TO_ALL = LOGFLAG_WRITE_TO_FILE | LOGFLAG_WRITE_TO_DEBUGGER;

class LogManager
{
public:
	LogManager(const std::string& logFile);
	~LogManager();

	void SetFlags(unsigned char flags);

	void Error(const string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, int lineNum);
	void Log(const string& tag, const string& message, const char* funcName, const char* sourceFile, int lineNum);
	string GetOutputBuffer(const string& tag, const string& message, const char* funcName, const char* sourceFile, int lineNum);
	void OutputToDebugLogs(const string& output);
	void WriteLogToFile(const string& output);
private:
	string mLogFilename;
	unsigned char mFlags;
};
