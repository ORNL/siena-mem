# Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.

dnl Process this file with autoconf to produce configure.

## ---------------------------------------------------------------------------
## Programmer: Jeremy Meredith
## Date:       June 17, 2015
##
## Modifications:
##
## ---------------------------------------------------------------------------

AC_INIT()
AC_CONFIG_HEADER(config/AspenConfig.h)

TOPDIR=`pwd`
AC_SUBST(TOPDIR)

m4_include([config/ax_cxx_compile_stdcxx_11_mod_jsm.m4])
m4_include([config/ax_python_devel.m4])

## ---------------------------------------------------------------------------
## Load the OS configuration file if present.
## ---------------------------------------------------------------------------
UNAME=`uname 2>/dev/null`
SHORT_UNAME=`uname -s | tr A-Z a-z | tr -d 0-9`
site_conf=config-site/$UNAME.conf
AC_MSG_CHECKING([for OS configuration file])
if test -r $site_conf; then
    AC_MSG_RESULT($site_conf)
    . $site_conf
else
    AC_MSG_RESULT(none found)
fi


## ---------------------------------------------------------------------------
## Load the site configuration file if present.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(for site configuration file)

hostname=`hostname -f 2>/dev/null`
site_conf=config-site/$hostname.conf
if test -r $site_conf; then
   AC_MSG_RESULT($site_conf)
   . $site_conf
else
   ## Check for hostname without the -f
   hostnamenodashf=`hostname 2>/dev/null`
   site_conf=config-site/$hostnamenodashf.conf
   if test -r $site_conf; then
      AC_MSG_RESULT($site_conf)
      . $site_conf
   else
      ## Also check for hostnames where the first field is a node name.
      hostminusnodename=`echo $hostname | cut -d. -f2-`
      site_conf=config-site/$hostminusnodename.conf
      if test -r $site_conf; then
         AC_MSG_RESULT($site_conf)
         . $site_conf
      else
         ## Or a hostname where the first chunk has an ignore-able number.
         hostminusnodenumber=`echo $hostname | sed 's/^\([[_a-zA-Z\-]]\+\)[[0-9]]\+\./\1./'`
         site_conf=config-site/$hostminusnodenumber.conf
         if test -r $site_conf; then
            AC_MSG_RESULT($site_conf)
            . $site_conf
         else
            AC_MSG_RESULT(none found)
         fi
      fi
   fi
fi

## ---------------------------------------------------------------------------
## Load any manually specified configuration file.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(for manually specified configuration file)
AC_ARG_WITH(config,[
config-site Options:
  --with-config=path      specify location of config-site file manually],
        CONFIGSITE="$withval",
        CONFIGSITE="")
if test -z "$CONFIGSITE"; then
   AC_MSG_RESULT(none specified)
elif test -f "$CONFIGSITE"; then
   . $CONFIGSITE
   AC_MSG_RESULT($CONFIGSITE)
else
   AC_MSG_RESULT(not found)
fi


## ---------------------------------------------------------------------------
## Macro needed for runtime linker paths.
## ---------------------------------------------------------------------------
AC_DEFUN([rpathize],
         [if test "$UNAME" = "Darwin"; then
             $1="$$1"
          else
             $1="$$1 -Wl,-rpath=$2"
          fi])


## ---------------------------------------------------------------------------
## Check for programs.
## ---------------------------------------------------------------------------
AC_PROG_CXX(g++ c++ icc CC)
AC_PROG_CXXCPP
AC_PROG_CC(gcc icc cc)
AC_PROG_CPP
AC_PROG_MAKE_SET
AC_CHECK_PROGS(YACC, 'bison -y' byacc yacc, [])
AC_CHECK_PROGS(LEX, flex lex, [])

YACCLEX="yes"
if test -z "$YACC" || test -z "$LEX"; then
   YACCLEX="no"
   AC_MSG_WARN(================================================================)
   AC_MSG_WARN(YOU WILL NEED LEX/FLEX and YACC/BISON IF YOU ARE MODIFYING ASPEN)
   AC_MSG_WARN(================================================================)
fi
AC_SUBST(YACCLEX)

AC_LANG(C++)
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)


## ---------------------------------------------------------------------------
## Static or dynamic build.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(for static or dynamic library build)
AC_ARG_ENABLE(dynamic,[
Library Build Options:
  --enable-dynamic        enable dynamic library build, not static
  --disable-dynamic       enable static library build, not dynamic (DEFAULT)],
        DYNAMIC="$enableval",
        DYNAMIC="no")
if test "$DYNAMIC" = "no"; then
   AC_MSG_RESULT(static)
   LIBRARY_OR_ARCHIVE="$""(ARCHIVE)"
   LIB_NAME="libaspen.a"
elif test "$DYNAMIC" = "yes"; then
   AC_MSG_RESULT(dynamic)
   LIBRARY_OR_ARCHIVE="$""(LIBRARY)"
   LIB_NAME="libaspen.so"
else
   AC_MSG_RESULT(unknown value; assuming static)
   LIBRARY_OR_ARCHIVE="$""(ARCHIVE)"
   LIB_NAME="libaspen.a"
   #DYNAMIC="no"
fi
AC_SUBST(LIBRARY_OR_ARCHIVE)
AC_SUBST(LIB_NAME)


## ---------------------------------------------------------------------------
## Determine a dependency mode.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(for dependency mode)
if test "`${MAKE-make} --version -f /dev/null 2>/dev/null|sed -n 1p|cut -c1-8`" = "GNU Make"; then
   if test -n "$GCC"; then
      # Darwin's universal build doesn't like the make-depend-gnu
      if test "$UNAME" = "Darwin"; then
         DEPMODE=cpp
         MG=""
      else
         DEPMODE=gcc3
      fi
   elif test -n "$MAKEDEPEND"; then
      DEPMODE=cpp
   else
      DEPMODE=none
   fi
else
   if test -n "$MAKEDEPEND"; then
      DEPMODE=cpp
   else
      DEPMODE=none
   fi
fi
AC_MSG_RESULT($DEPMODE)
AC_SUBST(DEPMODE)


## ---------------------------------------------------------------------------
## MPI check.
## ---------------------------------------------------------------------------

# first, find a compiler to get flags from
MPICC=""
AC_ARG_WITH(mpi,[
MPI Options:
  --with-mpi              search for MPI
  --with-mpi=mpicc        specify MPI compiler to extract flags from
  --without-mpi           (DEFAULT) disable MPI],
        MPI="$withval",
        if test -z "$MPI"; then MPI="no"; fi)

detect_mpi_flags="no"
if test "$MPI" != "no"; then
   if test -n "$MPI_CPPFLAGS" -o -n "$MPI_LDFLAGS" -o -n "$MPI_LIBS"; then
      MPI="manually configured"
   else
      detect_mpi_flags="yes"
      if test "$MPI" = "yes"; then
         # search for mpi compiler, too
         MPI=""
         for mpicompiler in mpic++ mpicxx; do
            if test -z "$MPI"; then
               AC_PATH_PROG([MPI], [$mpicompiler], [], [])
            fi
         done
         if test -z "$MPI"; then
            AC_MSG_WARN(could not find MPI C++ compiler; disabling MPI)
            detect_mpi_flags="no"
            MPI="no"
         fi
      fi
   fi
fi

AC_MSG_CHECKING(for MPI)
AC_MSG_RESULT($MPI)

if test "$detect_mpi_flags" = "yes"; then
   # extract MPI compile info from compiler
   if test "$MPI" != "no"; then
      # option 1: -compile-info
      MPISHOW_C=`$MPI -compile-info 2>&AS_MESSAGE_LOG_FD`
      # option 2: -show
      if test "$?" -ne "0"; then
         MPISHOW_C=`$MPI -show 2>&AS_MESSAGE_LOG_FD`
      fi
      if test "$?" -ne "0"; then
         MPISHOW_C=`$MPI --cray-print-opts=cflags 2>&AS_MESSAGE_LOG_FD`
         if test "$?" -ne "0"; then
            MPI="no"
            AC_MSG_WARN(could not extract compile flags from MPI compiler; disabling MPI)
         fi
      fi
      
   fi

   # extract MPI link info from compiler
   if test "$MPI" != "no"; then
      # option 1: -link-info
      MPISHOW_L=`$MPI -link-info 2>&AS_MESSAGE_LOG_FD`
      # option 2: -show
      if test "$?" -ne "0"; then
         MPISHOW_L=`$MPI -show 2>&AS_MESSAGE_LOG_FD`
      fi
      if test "$?" -ne "0"; then
         MPISHOW_L=`$MPI --cray-print-opts=libs 2>&AS_MESSAGE_LOG_FD`
         if test "$?" -ne "0"; then
            MPI="no"
            AC_MSG_WARN(could not extract link flags from MPI compiler; disabling MPI)
         fi
      fi
   fi

   # turn raw MPI info into actual flags
   if test "$MPI" != "no"; then
      MPI_CPPFLAGS=`echo $MPISHOW_C | perl -e '$_=<>;print join(" ",grep(/^-I/,split(/\s+/)));'`
      #MPI_LDFLAGS=`echo $MPISHOW_L | perl -e '$_=<>;print join(" ",grep(!/^-I|^-l|^\w/,split(/\s+/)));'`
      MPI_LDFLAGS=`echo $MPISHOW_L | perl -e '$_=<>;print join(" ",grep(/^-L/,split(/\s+/)));'`
      MPI_LIBS=`echo $MPISHOW_L | perl -e '$_=<>;print join(" ",grep(/^-l/,split(/\s+/)));'`
   fi
fi

# now actually try MPI...
if test "$MPI" != "no"; then
   old_cppflags=$CPPFLAGS
   old_ldflags=$LDFLAGS
   old_libs=$LIBS
   CPPFLAGS="$CPPFLAGS $MPI_CPPFLAGS";
   LDFLAGS="$LDFLAGS $MPI_LDFLAGS";
   LIBS="$LIBS $MPI_LIBS"

   AC_MSG_CHECKING(if MPI works)
   AC_TRY_LINK([#include <mpi.h>],
               [int a; MPI_Bcast(&a,1,MPI_INT,0,MPI_COMM_WORLD);],
               AC_MSG_RESULT(yes),
               AC_MSG_RESULT(no);MPI="no";AC_MSG_WARN(could not get MPI to compile/link; disabling MPI))

   CPPFLAGS=$old_cppflags
   LDFLAGS=$old_ldflags
   LIBS=$old_libs

   AC_MSG_CHECKING(for MPI CPPFLAGS)
   AC_MSG_RESULT($MPI_CPPFLAGS)
   AC_MSG_CHECKING(for MPI LDFLAGS)
   AC_MSG_RESULT($MPI_LDFLAGS)
   AC_MSG_CHECKING(for MPI LIBS)
   AC_MSG_RESULT($MPI_LIBS)
fi

# set preprocessor definition
if test "$MPI" != "no"; then
   AC_DEFINE(HAVE_MPI,[1])
fi

AC_SUBST(MPI_CPPFLAGS)
AC_SUBST(MPI_LDFLAGS)
AC_SUBST(MPI_LIBS)


## ---------------------------------------------------------------------------
## OpenMP check.
## ---------------------------------------------------------------------------


OPENMP=""
AC_ARG_WITH(openmp,[
OpenMP Options:
  --with-openmp           enable OpenMP
  --with-openmp=flags     specify flags needed to enable OpenMP
  --without-openmp        (DEFAULT) disable OpenMP],
        OPENMP="$withval",
        if test -z "$OPENMP"; then OPENMP="no"; fi)

if test "$OPENMP" = "yes"; then
   OPENMP=""
   for ompflags in "-fopenmp" "-openmp"; do
      old_cxxflags=$CXXFLAGS
      # We usually get a warning if there's an unrecognized
      # pragma; with icc and gcc, forcing warnings to errors
      # actually ensures we're compiling with OpenMP.
      CXXFLAGS="$CXXFLAGS -Werror $ompflags";
      if test -z "$OPENMP"; then
         AC_MSG_CHECKING(if OpenMP support is enabled by $ompflags)
         AC_TRY_LINK([#include <omp.h>],
                     [int *x=new int[1024], *y=new int[1024];
                      #pragma omp parallel for
                      for (int i=0; i<1024; ++i) {x[i]=y[i]*2;}
                     ],
                     AC_MSG_RESULT(yes);OPENMP="$ompflags",
                     AC_MSG_RESULT(no))
      fi
      CXXFLAGS=$old_cxxflags
   done
   if test -z "$OPENMP"; then
      AC_MSG_WARN(attempts to enable OpenMP failed; disabling);
      OPENMP="no";
   fi
elif test "$OPENMP" != "no"; then
   old_cxxflags=$CXXFLAGS
   # Again, add -Werror to make check successful (see above).
   CXXFLAGS="$CXXFLAGS -Werror $OPENMP"
   AC_MSG_CHECKING(if OpenMP support is enabled by $OPENMP)
   AC_TRY_LINK([#include <omp.h>],
               [int *x=new int[1024], *y=new int[1024];
                #pragma omp parallel for
                for (int i=0; i<1024; ++i) {x[i]=y[i]*2;}
               ],
               AC_MSG_RESULT(yes),
               AC_MSG_RESULT(no);OPENMP="no";AC_MSG_WARN(given flags did not enable OpenMP; disabling))
   CXXFLAGS=$old_cxxflags
fi

# We're going to use CXXFLAGS for the OpenMP flags
if test "$OPENMP" != "no"; then
   AC_DEFINE(HAVE_OPENMP,[1])
   CXXFLAGS="$CXXFLAGS $OPENMP"
fi

AC_MSG_CHECKING(for OpenMP)
AC_MSG_RESULT($OPENMP)

## ---------------------------------------------------------------------------
## Python check.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(if we should attempt to build the Python module)

AC_ARG_WITH(python,[
Python Options:
  --with-python            (DEFAULT) assume compiler knows about Python
  --with-python=path       specify location of Python binary
  --without-python         disable Python support],
        PYTHON="$withval",
        if test -z "$PYTHON"; then PYTHON="yes"; fi)
AC_MSG_RESULT($PYTHON)

if test "$PYTHON" != "no"; then 
   old_path=$PATH
   if test "$PYTHON" != "yes"; then 
      PATH=`dirname $PYTHON`:$PATH
   fi
   AX_PYTHON_DEVEL
   PATH=$old_path
   if test "$pythonexists" != "yes"; then
      AC_MSG_WARN(detected problem with python; disabling python)
      PYTHON="no"
   fi     
fi

if test "$PYTHON" == "no"; then 
   AC_MSG_NOTICE(disabling check for Boost)
   BOOST="no"
fi

## ---------------------------------------------------------------------------
## Boost check.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(for Boost)

AC_ARG_WITH(boost,[
Boost Options:
  --with-boost             (DEFAULT) assume compiler knows about Boost
  --with-boost=path        specify location of Boost installation
  --without-boost          disable Boost support],
        BOOST="$withval",
        if test -z "$BOOST"; then BOOST="yes"; fi)
AC_MSG_RESULT($BOOST)

if test "$BOOST" != "no"; then
   old_cppflags=$CPPFLAGS
   old_ldflags=$LDFLAGS
   old_libs=$LIBS
   
   # cppflags and basic headers
   BOOST_CPPFLAGS=""
   if test "$BOOST" != "yes"; then
      BOOST_CPPFLAGS="-I${BOOST}/include"
   fi
   
   AC_MSG_CHECKING(if Boost basic headers work)
   CPPFLAGS="$old_cppflags $BOOST_CPPFLAGS"
   AC_TRY_COMPILE([#include <boost/logic/tribool.hpp>],
                  [boost::tribool tb(false);],
                  AC_MSG_RESULT(yes),
                  AC_MSG_RESULT(no);PYTHON="no";AC_MSG_WARN(boost::python required for Python module; disabling))
   
   # ldflags and some libraries
   BOOST_LDFLAGS=""
   if test "$BOOST" != "yes"; then
      BOOST_LDFLAGS="-L${BOOST}/lib"
      rpathize(BOOST_LDFLAGS, ${BOOST}/lib)
   fi
   LDFLAGS="$old_ldflags $BOOST_LDFLAGS"
   
   BOOST_LIBS=""
   
   # Boost python requires python
   if test "$PYTHON" != "no"; then
      AC_MSG_CHECKING(if Boost python library works)
      tmplib="-lboost_python"
      CPPFLAGS="$CPPFLAGS $PYTHON_CPPFLAGS"
      LDFLAGS="$LDFLAGS $PYTHON_LDFLAGS"
      LIBS="$old_libs $tmplib"
      AC_TRY_COMPILE([#include <boost/python.hpp>
                      using namespace boost::python;
                      int DoSomething() { return 17; }
                      BOOST_PYTHON_MODULE(testmodule) {
                         def("DoSomething", &DoSomething);
                      }],,
                      AC_MSG_RESULT(yes);BOOST_LIBS="$BOOST_LIBS $tmplib",
                      AC_MSG_RESULT(no);PYTHON="no";AC_MSG_WARN(boost::python required for Python module; disabling))
   fi
   
   CPPFLAGS=$old_cppflags
   LDFLAGS=$old_ldflags
   LIBS=$old_libs
fi

AC_SUBST(BOOST)
AC_SUBST(BOOST_CPPFLAGS)
AC_SUBST(BOOST_LDFLAGS)
AC_SUBST(BOOST_LIBS)

## ---------------------------------------------------------------------------
## Boost check.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(if we should attempt to support NLopt)

NLOPT_CPPFLAGS=""
NLOPT_LDFLAGS=""
NLOPT_LIBS=""

AC_ARG_WITH(nlopt,[
NLopt Options:
  --with-nlopt             (DEFAULT) assume compiler knows about NLopt
  --with-nlopt=path        specify location of NLopt installation
  --without-nlopt          disable NLopt support],
        NLOPT="$withval",
        if test -z "$NLOPT"; then NLOPT="yes"; fi)
AC_MSG_RESULT($NLOPT)
# make sure it works with those flags
if test "$NLOPT" != "no"; then

   if test "$NLOPT" != "yes"; then
      # specified path
      NLOPT_CPPFLAGS="-I""$NLOPT""/include"
      NLOPT_LDFLAGS="-L""$NLOPT""/lib"
      rpathize(NLOPT_LDFLAGS, $NLOPT/lib)
   fi

   old_cppflags=$CPPFLAGS
   old_ldflags=$LDFLAGS
   old_libs=$LIBS
   CPPFLAGS="$CPPFLAGS $NLOPT_CPPFLAGS";
   LDFLAGS="$LDFLAGS $NLOPT_LDFLAGS";

   # library name #1 attempt
   NLOPT_LIBS="-lnlopt"
   LIBS="$old_libs $NLOPT_LIBS"
   success="no"
   AC_MSG_CHECKING(if NLOPT works with -lnlopt)
   AC_TRY_LINK([#include <nlopt.hpp>],
               [nlopt::opt nopt(nlopt::GN_ISRES,1);],
               AC_MSG_RESULT(yes);success="yes",
               AC_MSG_RESULT(no))

   # library name #2 attempt
   if test "$success" = "no"; then
      NLOPT_LIBS="-lnlopt_cxx"
      LIBS="$old_libs $NLOPT_LIBS"
      AC_MSG_CHECKING(if NLOPT works with -lnlopt_cxx)
      AC_TRY_LINK([#include <nlopt.hpp>],
                  [nlopt::opt nopt(nlopt::GN_ISRES,1);],
                  AC_MSG_RESULT(yes);success="yes",
                  AC_MSG_RESULT(no))
   fi

   # nope, nothing
   if test "$success" = "no"; then
      NLOPT="no"
      AC_MSG_WARN(could not get NLOPT to compile/link; disabling NLOPT)
   fi

   CPPFLAGS=$old_cppflags
   LDFLAGS=$old_ldflags
   LIBS=$old_libs
fi

# set preprocessor definition
if test "$NLOPT" != "no"; then
   AC_DEFINE(HAVE_NLOPT,[1])
else
   NLOPT_CPPFLAGS=""
   NLOPT_LDFLAGS=""
   NLOPT_LIBS=""
fi


AC_MSG_CHECKING(for NLopt)
AC_MSG_RESULT($NLOPT)
AC_SUBST(NLOPT)
AC_SUBST(NLOPT_CPPFLAGS)
AC_SUBST(NLOPT_LDFLAGS)
AC_SUBST(NLOPT_LIBS)

## ---------------------------------------------------------------------------
## Java check.
## ---------------------------------------------------------------------------
AC_MSG_CHECKING(for Java Native Interface (JNI))
AC_ARG_WITH(java,[
Java Options:
  --with-java              (DEFAULT) try to find Java
  --with-java=path         specify location of Java root
  --without-java           disable Java support],
        JAVA="$withval",
        if test -z "$JAVA"; then JAVA="yes"; fi)
AC_MSG_RESULT($JAVA)
 
if test "$JAVA" = "yes"; then
   AC_CHECK_PROGS(JRE, java, [])
   AC_CHECK_PROGS(JAVAC, javac, [])
   AC_CHECK_PROGS(JAVAH, javah, [])

   if test -z "$JRE" -o  -z "$JAVAC" -o  -z "$JAVAH"; then
      AC_MSG_WARN(Couldn't find Java binaries automatically; disabling)
      JAVA="no"
   else
      javapath=`which $JRE`
      if test -h "$javapath"; then javapath=`readlink $javapath`; fi
      if test -h "$javapath"; then javapath=`readlink $javapath`; fi
      if test -h "$javapath"; then javapath=`readlink $javapath`; fi
      JAVAROOT=`dirname $javapath` # get rid of binary name
      JAVAROOT=`dirname $JAVAROOT` # parent dir
      if ! test -f "$JAVAROOT/include/jni.h"; then
         JAVAROOT=`dirname $JAVAROOT` # parent dir again
      fi
      if ! test -f "$JAVAROOT/include/jni.h"; then
         AC_MSG_WARN(Couldn't find include dir near "java" binary; disabling)
         JAVA="no"
      fi
   fi
elif test "$JAVA" != "no"; then
   JRE=$JAVA/bin/java
   JAVAC=$JAVA/bin/javac
   JAVAH=$JAVA/bin/javah
   if ! test -f "$JRE" -a  -f "$JAVAC" -a  -f "$JAVAH"; then
      AC_MSG_WARN(Couldn't find Java binaries in $JAVA/bin; disabling)
      JAVA="no"
   else
      JAVAROOT="$JAVA"
   fi
fi

if test "$JAVA" != "no"; then
   JAVA_CPPFLAGS="-I$JAVAROOT/include"
   old_cppflags=$CPPFLAGS
   CPPFLAGS="$CPPFLAGS $JAVA_CPPFLAGS";
   AC_MSG_CHECKING(if we can compile using JAVA)
   AC_TRY_COMPILE([#include <jni.h>],
                  [jlong value;],
                  AC_MSG_RESULT(yes),
                  AC_MSG_RESULT(no, disabling Java);JAVA="no")
   CPPFLAGS=$old_cppflags
fi

if test "$JAVA" = "no"; then
   JAVA_CPPFLAGS=""
fi

AC_SUBST(JAVA)
AC_SUBST(JAVA_CPPFLAGS)

## ---------------------------------------------------------------------------
## Add some directories to search for includes and libraries
## ---------------------------------------------------------------------------

# Fill these out when we have something to put here.
# It just causes warnings until then....
#CPPFLAGS="-I$TOPDIR/common $CPPFLAGS"
#LDFLAGS="-L$TOPDIR/lib $LDFLAGS"

## ---------------------------------------------------------------------------
## Check some data type sizes
## ---------------------------------------------------------------------------
AC_CHECK_SIZEOF(int,0)
AC_CHECK_SIZEOF(long,0)
AC_CHECK_SIZEOF(long long,0)
AC_CHECK_SIZEOF(void *,0)

AC_C_BIGENDIAN

## ---------------------------------------------------------------------------
## Generate configuration files
## ---------------------------------------------------------------------------
DEPENDENCIES=config/make-dependencies
AC_SUBST_FILE(DEPENDENCIES)
VARIABLES=config/make-variables
AC_SUBST_FILE(VARIABLES)
TARGETS=config/make-targets
AC_SUBST_FILE(TARGETS)

## ---------------------------------------------------------------------------
## Do the standard flags and a few nonstandard ones
## ---------------------------------------------------------------------------
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)

## ---------------------------------------------------------------------------
## Subdirectories are separated by capability
## ---------------------------------------------------------------------------
SUBDIRS="$""(SUBDIRS_COMMON)"
if test "$NLOPT" != "no"; then
  SUBDIRS="$SUBDIRS $""(SUBDIRS_NLOPT)"
fi
if test "$PYTHON" != "no"; then
  SUBDIRS="$SUBDIRS $""(SUBDIRS_PYTHON)"
fi
if test "$JAVA" != "no"; then
  SUBDIRS="$SUBDIRS $""(SUBDIRS_JAVA)"
fi
AC_SUBST(SUBDIRS)


## ---------------------------------------------------------------------------
## Final makefile transformations; note order can be important here
## ---------------------------------------------------------------------------

AC_OUTPUT(config/make-dependencies
          config/make-variables
          config/make-targets
          Makefile
          aspen/Makefile
)
