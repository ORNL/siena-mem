// Copyright 2013-2017 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef ASPEN_EXCEPTION_H
#define ASPEN_EXCEPTION_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstdlib>

#if defined(__unix__) || defined(__APPLE__)
# include <execinfo.h>
#endif

///\todo: These HAVE_XXXXX_H macros will be automated with Autotools or CMake in future.
///       For now it is directly written in Makefile.
#if defined(HAVE_CXXABI_H)
# include <cxxabi.h>
#endif

// copied from boost/current_function.hpp
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
# define ASPEN_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
# define ASPEN_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
# define ASPEN_CURRENT_FUNCTION __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
# define ASPEN_CURRENT_FUNCTION __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
# define ASPEN_CURRENT_FUNCTION __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
# define ASPEN_CURRENT_FUNCTION __func__
#else
# define ASPEN_CURRENT_FUNCTION "(unknown)"
#endif


// -----------------------------------------------
// Variadic macros in MSVC are not the same as GCC
// -----------------------------------------------
#ifdef _MSC_VER
///\todo: MS C++ code is not checked.
# define THROW(E , ...) do                      \
  {                                             \
    E exc(#E, __VA_ARGS__, #E);                 \
    exc.SetFile(__FILE__);                      \
    exc.SetLine(__LINE__);                      \
    exc.SetFunction(ASPEN_CURRENT_FUNCTION);    \
    throw(exc);                                 \
  } while(0)
#else
# define THROW(E , args...) do                  \
  {                                             \
    E exc(args);                                \
    exc.SetFile(__FILE__);                      \
    exc.SetLine(__LINE__);                      \
    exc.SetFunction(ASPEN_CURRENT_FUNCTION);    \
    exc.SetClassName(#E);                       \
    throw(exc);                                 \
  } while(0)
#endif

namespace {

  // Demangle C++ function names using libc abi::__cxa_demangle.
  // This feature is maybe limited to gcc.
  // HAVE_CXXABI_H flags is now defined manually in Makefile, but
  // it will be provided by Automake/CMake in future.
  inline std::string _Demangle(const std::string& s)
  {
#ifdef HAVE_CXXABI_H
    int status;
    char *demangled = abi::__cxa_demangle(s.c_str(), NULL, NULL, &status);
    if (!demangled) {
      return s;
    } else {
      std::string ret(demangled);
      free(demangled);
      return ret;
    }
#else
    return s;
#endif
  }

  // Demangle C++ function names
  // Parameter line can contain multiple words/identifiers which are not necessarily
  // mangled names. Non-mangled names are just passed through.
  std::string _DemangleAll(const std::string& line)
  {
    std::string ret = "", tmp = "";
    for (std::string::const_iterator itr = line.begin(); itr != line.end(); itr++) {
      if (isspace(*itr) || *itr=='(' || *itr==')' || *itr=='+') {
        if (!tmp.empty()) {
          ret += _Demangle(tmp);
          tmp.clear();
        }
        ret += *itr;
      } else {
        tmp += *itr;
      }
    }

    if (!tmp.empty()) ret += _Demangle(tmp);

    return ret;
  }
}

class AspenException
{
  int line_;
  std::string file_;
  std::string func_;
  std::string msg_;
  std::string cls_name_;

  ///\todo: we don't need a backtrace every time, so these code should only be compiled with -g or other debug flag.
  std::vector<std::string> backtrace_;

public:
  explicit AspenException(const char * msg) throw()
    : line_(-1), file_("(unknown)"),
      func_("(unknown)"), msg_(msg),
      cls_name_("AspenException"), backtrace_()
  {
    GenerateBacktrace();
  }

  explicit AspenException(const std::string &msg) throw()
    : line_(-1), file_("(unknown)"),
      func_("(unknown)"), msg_(msg),
      cls_name_("AspenException"), backtrace_()
  {
    GenerateBacktrace();
  }

  virtual ~AspenException() throw() { }
  
  const std::vector<std::string> &Backtrace() const throw()
  {
    return backtrace_;
  }

  const int         GetLine()      const throw() { return line_; }
  const std::string GetFile()      const throw() { return file_; }
  const std::string GetFunction()  const throw() { return func_; }
  const std::string GetMessage()   const throw() { return msg_; }
  const std::string GetClassName() const throw() { return cls_name_; }

  void SetLine(int line)             throw() { line_ = line; }
  void SetFile(const char *file)     throw() { file_ = file; }
  void SetFunction(const char *func) throw() { func_ = func; }
  void SetMessage(const char *msg)   throw() { msg_  = msg; }
  void SetClassName(const char* cls) throw() { cls_name_ = cls; }
  
  // Returns a multi-line string that contains all information of this exception.
  // 
  std::string PrettyString() const throw()
  {
    std::stringstream ss;
    ss << file_ <<  ":" << line_ <<  " " << func_ << " : "
       << cls_name_ << " " << msg_;
    for (size_t i = 0; i < backtrace_.size(); i++) {
      ss << "\n" << _DemangleAll(backtrace_[i]);
    }
    return ss.str();
  }

 protected:

  // Get a backtrace at the point of the exception is thrown using a glibc function backtrace() 
  void GenerateBacktrace()
  {
#if defined(__unix__) || defined(__APPLE__)
    // get the backtrace information via backtrace() system call.
    void *trace[128];
    int n = backtrace(trace, sizeof(trace) / sizeof(trace[0]));
    char **syms = backtrace_symbols(trace, n);

    // Translate the void* information to human-readable array of strings.
    for (int i = 0; i < n; i++) {
      backtrace_.push_back(std::string(syms[i]));
    }

    free(syms); // we have to call free().
#endif
  }
};

inline std::string TOSTRING(std::string x) {return x;}

// A utility macro to define a subclass of AspenException.
// Defined subclasses have a series of convenient constructors and
// virtual destructors.
#define DEFINE_EXCEPTION(ErrorClass)                              \
  class ErrorClass : public AspenException {                      \
    public:                                                       \
    ErrorClass(const char *msg) throw()                           \
      : AspenException(msg)                                       \
      {                                                           \
      }                                                           \
                                                                  \
    template<typename T1, typename T2>                            \
    ErrorClass(T1 msg1, T2 msg2)                                  \
      : AspenException(TOSTRING(msg1) + TOSTRING(msg2))           \
      {                                                           \
      }                                                           \
                                                                  \
    template<typename T1, typename T2, typename T3>               \
    ErrorClass(T1 msg1, T2 msg2, T3 msg3)                         \
      : AspenException(TOSTRING(msg1) +                           \
                       TOSTRING(msg2) +                           \
                       TOSTRING(msg3))                            \
      {                                                           \
      }                                                           \
                                                                  \
    template<class T1, class T2, class T3, class T4>              \
    ErrorClass(T1 msg1, T2 msg2, T3 msg3, T4 msg4)                \
      : AspenException(TOSTRING(msg1) +                           \
                       TOSTRING(msg2) +                           \
                       TOSTRING(msg3) +                           \
                       TOSTRING(msg4))                            \
      {                                                           \
      }                                                           \
                                                                  \
    template<class T1, class T2, class T3, class T4, class T5>    \
    ErrorClass(T1 msg1, T2 msg2, T3 msg3, T4 msg4, T5 msg5)       \
      : AspenException(TOSTRING(msg1) +                           \
                       TOSTRING(msg2) +                           \
                       TOSTRING(msg3) +                           \
                       TOSTRING(msg4) +                           \
                       TOSTRING(msg5))                            \
      {                                                           \
      }                                                           \
                                                                  \
    ErrorClass(const std::string &msg) throw()                    \
      : AspenException(msg)                                       \
      {                                                           \
      }                                                           \
                                                                  \
    virtual ~ErrorClass() throw() { }                             \
  };                                                              \
  

DEFINE_EXCEPTION(GrammaticalError);
DEFINE_EXCEPTION(ModelError);
DEFINE_EXCEPTION(LogicError);
DEFINE_EXCEPTION(TestError);
DEFINE_EXCEPTION(InputError);

#endif // ASPEN_EXCEPTION_H
