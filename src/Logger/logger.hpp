#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <ctime>
#include "sqlite3.h"

using std::string;
using std::mutex;

mutex logMutex;
string databaseFileName = "data/database.db";

#define numberOfPriorities 6

enum LogPriority
{
	TracePriority, DebugPriority, InfoPriority, WarnPriority, ErrorPriority, CriticalPriority
};

class Logger
{
private:
	static LogPriority priority;
	static mutex log_mutex;
	static const char* filepaths[numberOfPriorities];
	static FILE** files;

public:
	static void SetPriority(LogPriority new_priority)
	{
		priority = new_priority;
	}

	static void EnableFileOutput()
	{
		enable_file_output();
	}

	static void CloseFileOutput()
	{
		free_file();
	}

	template<typename... Args>
	static void Trace(const char* message, Args... args)
	{
		log(TracePriority, message, args...);
	}

	template<typename... Args>
	static void Debug(const char* message, Args... args)
	{
		log(DebugPriority, message, args...);
	}

	template<typename... Args>
	static void Info(const char* message, Args... args)
	{
		log(InfoPriority, message, args...);
	}

	template<typename... Args>
	static void Warn(const char* message, Args... args)
	{
		log(WarnPriority, message, args...);
	}

	template<typename... Args>
	static void Error(const char* message, Args... args)
	{
		log(ErrorPriority, message, args...);
	}

	template<typename... Args>
	static void Critical(const char* message, Args... args)
	{
		log(CriticalPriority, message, args...);
	}

	static void PrintDatabase(sqlite3* db) {
		// This log comes at Debug level.
		int rc;
		char query[] = "SELECT * FROM Stock";

		sqlite3_stmt* stmt;
		rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
		if (rc != SQLITE_OK) {
			Logger::Error("Can't prepare database: %s\n", sqlite3_errmsg(db));
		} else {
			// Write time to the file.

			std::time_t current_time = std::time(0);
			std::tm* timestamp = std::localtime(&current_time);
			char buffer[80];
			strftime(buffer, 80, "%c", timestamp);

			logMutex.lock();

			fprintf(files[DebugPriority], "%s Database contents:\n", buffer);
			// Print comma separated values.
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				fprintf(files[DebugPriority], "%s, ", (const char*)sqlite3_column_text(stmt, 0));
				fprintf(files[DebugPriority], "%s, ", (const char*)sqlite3_column_text(stmt, 1));
				fprintf(files[DebugPriority], "%d\n", sqlite3_column_int(stmt, 2));
			}	
			logMutex.unlock();
		}
	}

private:
	template<typename... Args>
	static void log(LogPriority message_priority, const char* message, Args... args) {
		
		std::time_t current_time = std::time(0);
		std::tm* timestamp = std::localtime(&current_time);
		char buffer[80];
		strftime(buffer, 80, "%c", timestamp);

		logMutex.lock();
		if (files[message_priority]) {
			fprintf(files[message_priority], "%s\t", buffer);
			fprintf(files[message_priority], message, args...);
			fprintf(files[message_priority], "\n");
			fflush(files[message_priority]);
		}
		logMutex.unlock();
	}

	static void enable_file_output()
	{
		int i;
		if (files == NULL) {
			files = (FILE**) malloc(numberOfPriorities * sizeof(FILE*));
		}
		for (i = 0; i < numberOfPriorities; i++) {
			files[i] = fopen(filepaths[i], "a");
		}
	}

	static void free_file()
	{
		if (files) {
			for (int i = 0; i < numberOfPriorities; i++) {
				fclose(files[i]);
			}
			free(files);
			files = NULL;
		}
	}
};

LogPriority Logger::priority = TracePriority;
mutex Logger::log_mutex;
const char* Logger::filepaths[] = {"data/Logs/TraceLogs.txt", "data/Logs/DebugLogs.txt", "data/Logs/InfoLogs.txt", "data/Logs/WarningLogs.txt", "data/Logs/ErrorLogs.txt", "data/Logs/CriticalLogs.txt"};
FILE** Logger::files = NULL;
