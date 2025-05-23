// This file is part of the dune-xt project:
//   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
// Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2014, 2016 - 2017)
//   René Fritze     (2009 - 2020)
//   Tobias Leibner  (2014, 2018, 2020)

#ifndef DUNE_XT_COMMON_LOGGING_HH
#define DUNE_XT_COMMON_LOGGING_HH

#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <dune/common/visibility.hh>

#include <dune/xt/common/logstreams.hh>

namespace Dune::XT::Common {


class Logging;
//! global Logging instance
inline Logging& Logger();

/** \brief handles all logging
 **/
class Logging
{
private:
  Logging();
  //! cleanup stream and flag containers
  void deinit();

public:
  ~Logging();

  // satisfy stricter warnings wrt copying
  Logging(const Logging&) = delete;
  Logging& operator=(const Logging&) = delete;

  /** \brief setup loglevel, logfilename
   *  \param logflags any OR'd combination of flags
   *  \param logfile filename for log, can contain paths, but creation will fail if dir is non-existant
   **/
  void create(int logflags = LogDefault,
              const std::string& logfile = "dune_xt_common_log",
              const std::string& datadir = "data",
              const std::string& _logdir = std::string("log"));

  //! \attention This will probably not do wht we want it to!
  void set_prefix(std::string prefix);
  void set_stream_flags(int streamID, int flags);
  int get_stream_flags(int streamID) const;

  /** \name forwarded Log functions
   * \{
   */
  template <class T>
  void log(T c, int streamID)
  {
    get_stream(streamID) << c;
  } // Log

  /** \}
   */

  LogStream& get_stream(int streamId);
  LogStream& error()
  {
    return get_stream(LOG_ERROR);
  }
  LogStream& info()
  {
    return get_stream(LOG_INFO);
  }
  LogStream& debug()
  {
    return get_stream(LOG_DEBUG);
  }
  LogStream& devnull()
  {
    return emptyLogStream_;
  }

  //! flush all active streams
  void flush();
  //! creates a new LogStream with given flags, returns new ID
  int add_stream(int flags);

  //! re-enable all logging below given priority level
  void resume(LogStream::PriorityType prio = LogStream::default_suspend_priority);
  //! (temporarily) disable all logging below given priority level
  void suspend(LogStream::PriorityType prio = LogStream::default_suspend_priority);

  struct SuspendLocal
  {
    LogStream::PriorityType prio_;
    explicit SuspendLocal(LogStream::PriorityType prio = LogStream::default_suspend_priority)
      : prio_(prio)
    {
      Logger().suspend(prio_);
    }

    ~SuspendLocal()
    {
      Logger().resume(prio_);
    }
  };

  struct ResumeLocal
  {
    LogStream::PriorityType prio_;
    explicit ResumeLocal(LogStream::PriorityType prio = LogStream::default_suspend_priority)
      : prio_(prio)
    {
      Logger().resume(prio_);
    }

    ~ResumeLocal()
    {
      Logger().suspend(prio_);
    }
  };

private:
  boost::filesystem::path filename_;
  boost::filesystem::path filenameWoTime_;
  boost::filesystem::ofstream logfile_;
  using FlagMap = std::map<int, int>;
  FlagMap flagmap_;
  using StreamMap = std::map<int, std::unique_ptr<LogStream>>;
  StreamMap streammap_;
  using IdVec = std::vector<int>;
  IdVec streamIDs_;
  int logflags_{LOG_NONE};
  EmptyLogStream emptyLogStream_;

  friend Logging& Logger();
};

DUNE_EXPORT inline Logging& Logger()
{
  static Logging log;
  return log;
}


} // namespace Dune::XT::Common

#define DXTC_LOG Dune::XT::Common::Logger()
#define DXTC_LOG_INFO DXTC_LOG.info()
#define DXTC_LOG_DEBUG DXTC_LOG.debug()
#define DXTC_LOG_ERROR DXTC_LOG.error()
#define DXTC_LOG_DEVNULL DXTC_LOG.devnull()

#define DXTC_LOG_INFO_0 (Dune::MPIHelper::getCommunication().rank() == 0 ? DXTC_LOG.info() : DXTC_LOG.devnull())
#define DXTC_LOG_DEBUG_0 (Dune::MPIHelper::getCommunication().rank() == 0 ? DXTC_LOG.debug() : DXTC_LOG.devnull())
#define DXTC_LOG_ERROR_0 (Dune::MPIHelper::getCommunication().rank() == 0 ? DXTC_LOG.error() : DXTC_LOG.devnull())

#endif // DUNE_XT_COMMON_LOGGING_HH
