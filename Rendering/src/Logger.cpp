#include "Logger.h"
#include <stdio.h>
#include <windows.h>

//! Global logger instance, used by the logger macros through the C interface.
static LogManager* gGlobalLogger = nullptr;

#pragma region LogManager implementation
LogManager::LogManager(const std::string& logFile)
{
	SetFlags(LOGFLAG_WRITE_TO_ALL);
	mLogFilename = logFile;
}

LogManager::~LogManager()
{

}

void LogManager::SetFlags(unsigned char flags)
{
	mFlags = flags;
}

void LogManager::Log(const string& tag, const string& message, const char* funcName, const char* sourceFile, int lineNum)
{
	string output = GetOutputBuffer(tag, message, funcName, sourceFile, lineNum);
	OutputToDebugLogs(output);
}

void LogManager::Error(const string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, int lineNum)
{
	string tag = ((isFatal) ? ("FATAL") : ("ERROR"));

	// buffer for our final output string
	string buffer = GetOutputBuffer(tag, errorMessage, funcName, sourceFile, lineNum);
	OutputToDebugLogs(buffer);

	// Show the dialog box.
	int result = ::MessageBoxA(NULL, buffer.c_str(), tag.c_str(), MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_DEFBUTTON3);

	// Act upon the choice.
	switch (result)
	{
	case IDIGNORE : return;
	case IDABORT  : __debugbreak(); return;  // Assembly language instruction to break into the debugger.
	case IDRETRY :	return;
	default :       return;
	}
}

string LogManager::GetOutputBuffer(const string& tag, const string& message, const char* funcName, const char* sourceFile, int lineNum)
{
	string output;
	if(!tag.empty())
		output = "[" + tag + "] " + message;
	else
		output = message;

	if(funcName != NULL)
	{
		output += "\nFunction: ";
		output += funcName;
	}
	if(sourceFile != NULL)
	{
		output += "\nFile: ";
		output += sourceFile;
	}
	if(lineNum != 0)
	{
		output += "\nLine: ";
		output += to_string(lineNum);
	}

	output += "\n";

	return output;
}

void LogManager::OutputToDebugLogs(const string& output)
{
	if(mFlags & LOGFLAG_WRITE_TO_FILE)
	{
		WriteLogToFile(output);
	}
	if(mFlags & LOGFLAG_WRITE_TO_DEBUGGER)
	{
		OutputDebugString(output.c_str());
	}
}

void LogManager::WriteLogToFile(const string& output)
{
	FILE* pLogFile = NULL;
	fopen_s(&pLogFile, mLogFilename.c_str(), "a+");
	if (!pLogFile)
		return;  // Error writing to file.

	fprintf_s(pLogFile, output.c_str());

	fclose(pLogFile);
}

#pragma endregion

/******************************************************************************************//**
* C interface 
*********************************************************************************************/
#pragma region C interface

namespace Logger
{
	void Init(string logFile)
	{
		if(!gGlobalLogger)
		{
			gGlobalLogger = new LogManager(logFile);
		}
	}

	void Destroy(void)
	{
		delete gGlobalLogger;
		gGlobalLogger = nullptr;
	}

	void Log(const string& tag, const string& message, const char* funcName, const char* sourceFile, int lineNum)
	{
		GLIB_ASSERT(gGlobalLogger);
		gGlobalLogger->Log(tag, message, funcName, sourceFile, lineNum);
	}

	void Error(const string& message, bool isFatal, const char* funcName, const char* sourceFile, int lineNum)
	{
		GLIB_ASSERT(gGlobalLogger);
		gGlobalLogger->Error(message, isFatal, funcName, sourceFile, lineNum);
	}

	void SetDisplayFlags(unsigned char flags)
	{
		GLIB_ASSERT(gGlobalLogger);
		gGlobalLogger->SetFlags(flags);
	}
}

#pragma endregion