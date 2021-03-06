#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <ctime>
#include <sqlite3.h>

#define numberOfPriorities 6

enum LogPriority
{
	TracePriority, DebugPriority, InfoPriority, WarnPriority, ErrorPriority, CriticalPriority
};

/**
 * @brief The Logger class that handles all the logging.
 * 
 */
class Logger
{
private:
	static LogPriority priority;
	static const char* filepaths[numberOfPriorities];
	static FILE** files;
	static std::mutex logMutex;
public:
	/**
	 * @brief Set the Minimum priority level to be logged.
	 * 
	 * @param new_priority : The new priority level of LogPriority type.
	 */
	static void SetPriority(LogPriority new_priority);

	/**
	 * @brief Enables output to the file.
	 * After calling this, the output will be written to the files.
	 */
	static void EnableFileOutput();

	/**
	 * @brief After this, the output will no longer be written to the files.
	 * It disables the file output.
	 */
	static void CloseFileOutput();

	/**
	 * @brief Logs traces.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message : The format of the message similar to printf.
	 * @param args : The arguments of the message similar to args of printf.
	 */
	template<typename... Args>
	static void Trace(const char* message, Args... args)
	{
		log(TracePriority, message, args...);
	}

	/**
	 * @brief Logs Debug.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message : The format of the message similar to printf.
	 * @param args : The arguments of the message similar to args of printf.
	 */
	template<typename... Args>
	static void Debug(const char* message, Args... args)
	{
		log(DebugPriority, message, args...);
	}

	/**
	 * @brief Logs Info.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message : The format of the message similar to printf.
	 * @param args : The arguments of the message similar to args of printf. 	 
	 */
	template<typename... Args>
	static void Info(const char* message, Args... args)
	{
		log(InfoPriority, message, args...);
	}

	/**
	 * @brief Logs Warnings.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message : The format of the message similar to printf.
	 * @param args : The arguments of the message similar to args of printf.
	 */
	template<typename... Args>
	static void Warn(const char* message, Args... args)
	{
		log(WarnPriority, message, args...);
	}

	/**
	 * @brief Logs Errors.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message : The format of the message similar to printf. 
	 * @param args : The arguments of the message similar to args of printf.
	 */
	template<typename... Args>
	static void Error(const char* message, Args... args)
	{
		log(ErrorPriority, message, args...);
	}

	/**
	 * @brief Logs Critical failures.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message : The format of the message similar to printf.
	 * @param args : The arguments of the message similar to args of printf.
	 */
	template<typename... Args>
	static void Critical(const char* message, Args... args)
	{
		log(CriticalPriority, message, args...);
	}

private:
	/**
	 * @brief The helper function for logging.
	 * It handles the main logging related work.
	 * 
	 * @tparam Args : This means any type of arguments.
	 * @param message_priority : The priority of the message. 
	 * @param message : The format of the message similar to printf.
	 * @param args : The arguments of the message similar to args of printf.
	 */
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

	/**
	 * @brief Helper function for EnableFileOutput.
	 */
	static void enable_file_output();

	/**
	 * @brief Helper funciton for CloseFileOutput.
	 * 
	 */
	static void free_file();
};
