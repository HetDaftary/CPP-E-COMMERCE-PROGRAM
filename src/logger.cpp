#include "../include/logger.hpp"

using std::string;
using std::mutex;

LogPriority Logger::priority = TracePriority;
const char* Logger::filepaths[] = {"data/Logs/TraceLogs.txt", "data/Logs/DebugLogs.txt", "data/Logs/InfoLogs.txt", "data/Logs/WarningLogs.txt", "data/Logs/ErrorLogs.txt", "data/Logs/CriticalLogs.txt"};
FILE** Logger::files = NULL;
mutex Logger::logMutex;

/**
 * @brief Set the Minimum priority level to be logged.
 * 
 * @param new_priority 
 */
void Logger::SetPriority(LogPriority new_priority) {
	priority = new_priority;
}

/**
 * @brief Enables output to the file.
 * After calling this, the output will be written to the files.
 */
void Logger::EnableFileOutput() {
	enable_file_output();
}

/**
 * @brief After this, the output will no longer be written to the files.
 * It disables the file output.
 */
void Logger::CloseFileOutput() {
	free_file();
}

/**
 * @brief Helper function for EnableFileOutput.
 */
void Logger::enable_file_output() {
	int i;
	if (files == NULL) {
		files = (FILE**) malloc(numberOfPriorities * sizeof(FILE*));
	}
	for (i = 0; i < numberOfPriorities; i++) {
		files[i] = fopen(filepaths[i], "a");
	}
}

/**
 * @brief Helper funciton for CloseFileOutput.
 * 
 */
void Logger::free_file() {
	if (files) {
		for (int i = 0; i < numberOfPriorities; i++) {
			fclose(files[i]);
		}
		free(files);
		files = NULL;
	}
}