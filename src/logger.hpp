//    _  __     __   __     ___  ___
//   / |/ /__ _/ /  / /__ _|_  |/ _ |
//  /    / _ `/ _ \/ / _ `/ __// // /
// /_/|_/\_,_/_.__/_/\_,_/____/____/
//
// Copyright (C) 2023 - Efflam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef NABLA2D_LOGGER_HPP
#define NABLA2D_LOGGER_HPP

#include <memory>
#include <exception>
#include <spdlog/spdlog.h>

namespace nabla2d
{
  class Logger
  {
  public:
    typedef enum
    {
      LOG_TRACE = spdlog::level::trace,
      LOG_DEBUG = spdlog::level::debug,
      LOG_INFO = spdlog::level::info,
      LOG_WARN = spdlog::level::warn,
      LOG_ERROR = spdlog::level::err,
      LOG_CRITICAL = spdlog::level::critical,
      LOG_OFF = spdlog::level::off
    } Level;

    static void setLevel(Level level);

    template <typename... Args>
    static void log(Level level, const char *fmt, Args... args)
    {
      sLogger->log(static_cast<spdlog::level::level_enum>(level), fmt, args...);
    }

    template <typename... Args>
    static void trace(const char *fmt, Args... args)
    {
      log(LOG_TRACE, fmt, args...);
    }

    template <typename... Args>
    static void debug(const char *fmt, Args... args)
    {
      log(LOG_DEBUG, fmt, args...);
    }

    template <typename... Args>
    static void info(const char *fmt, Args... args)
    {
      log(LOG_INFO, fmt, args...);
    }

    template <typename... Args>
    static void warn(const char *fmt, Args... args)
    {
      log(LOG_WARN, fmt, args...);
    }

    template <typename... Args>
    static void error(const char *fmt, Args... args)
    {
      log(LOG_ERROR, fmt, args...);
    }

    template <typename... Args>
    static void critical(const char *fmt, Args... args)
    {
      log(LOG_CRITICAL, fmt, args...);
    }

    static void logException(const std::exception &exception, Level level = LOG_CRITICAL)
    {
      log(level, "Exception: {}", exception.what());
    }

  private:
    static const std::shared_ptr<spdlog::logger> sLogger;
  };

} // namespace nabla2d

#endif // NABLA2D_LOGGER_HPP

// くコ:彡