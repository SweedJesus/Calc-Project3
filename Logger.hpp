#pragma once

/** Primitive message logger
 * @author Nils Olsson
 *
 * Example usage:
 * ```
 * // Create stream loggers
 * StreamLogger stdLogger1(&std::cout, LogLevel::Info);
 * StreamLogger stdLogger2(&std::cout, LogLevel::Info | LogLevel::Debug);
 *
 * // Create file logger
 * std::ofstream logFile("log.txt", std::ios::out);
 * FileLogger fileLogger(&logFile, LogLevel::All);
 *
 * // Chain loggers
 * stdLogger1.add(&stdLogger2, &fileLogger);
 * // Or one at a time
 * stdLogger1.add(&stdLogger2);
 * stdLogger1.add(&fileLogger);
 *
 * // Log messages
 * stdLogger1.log(LogLevel::Info, "foo\n");
 * stdLogger1.log(LogLevel::Debug, "bar\n");
 * stdLogger1.log(LogLevel::Error, "baz\n");
 * ```
 *
 * TODO: Use unique_ptr for FileLogger file object?
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

namespace mesa
{
  enum LogLevel
  {
    None    = 1 << 0,
    Info    = 1 << 1,
    Debug   = 1 << 2,
    Warning = 1 << 3,
    Error   = 1 << 4,
    All     = (None | Info | Debug | Warning | Error)
  };

  /** Logger interface class
   */
  class Logger
  {
    public:
      using MessageT = std::pair<LogLevel, std::string>;

      Logger(
          const size_t& logLevel = LogLevel::None):
        m_logLevel{logLevel},
        m_next{nullptr}
      {}

      /** Log message
       */
      Logger& log(
          const size_t& logLevel,
          const std::string& line)
      {
        if ((m_logLevel & logLevel) == logLevel)
          log_helper(line);
        if (m_next != nullptr)
          m_next->log(logLevel, line);
        return (*this);
      }

      /** Log message
       */
      Logger& log(const MessageT& message)
      { return log(message.first, message.second); }

      /** Set log level
       */
      size_t logLevel() const
      { return m_logLevel; }

      /** Set log level
       */
      void logLevel(const size_t& logLevel)
      { m_logLevel = logLevel; }

      /** Get next logger
       */
      Logger* next() const
      { return m_next; }

      /** Set next logger
       */
      void next(Logger* next)
      { m_next = next; };

      /** Add logger to chain
       */
      void add(Logger* next)
      {
        Logger* last = this;
        while (last->m_next != nullptr)
          last = last->m_next;
        last->m_next = next;
      }

      template<class... Args>
      void add(Logger* next, Args... loggers)
      {
        add(next);
        add(loggers...);
      }

    protected:
      /** Logging helper function interface
       */
      virtual void log_helper(const std::string& line) = 0;

      size_t m_logLevel;
      Logger* m_next;
  };

  /** Stream (console) logger
   */
  class StreamLogger: public Logger
  {
    public:
      /**
       * @param os Output stream to log to
       * @param logLevel Logging level
       */
      StreamLogger(
          std::ostream* osPtr = nullptr,
          const size_t& logLevel = LogLevel::None):
        Logger{logLevel},
        m_osPtr{osPtr}
      {}

    protected:
      /** Logging helper function
       */
      void log_helper(const std::string& line) override
      { (*m_osPtr) << line; }

    private:
      std::ostream* m_osPtr;
  };

  class FileLogger: public Logger
  {
    public:
      /**
       * @param file Output file object to log to
       * @param logLevel Logging level
       */
      FileLogger(
          std::ofstream* ofsPtr,
          const size_t& logLevel = LogLevel::None):
        Logger{logLevel},
        m_ofsPtr{ofsPtr}
      {}

    protected:
      /** Logging helper function interface
       */
      void log_helper(const std::string& line) override
      { (*m_ofsPtr) << line; }

    private:
      std::ofstream* m_ofsPtr;
  };
}

inline mesa::Logger& operator<<(mesa::Logger& lhs, const mesa::Logger::MessageT& rhs)
{
  return lhs.log(rhs.first, rhs.second);
}
