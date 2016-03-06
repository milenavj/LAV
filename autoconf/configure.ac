dnl **************************************************************************
dnl * Initialize
dnl **************************************************************************
AC_INIT([[LAV]],[[0.01]],[milena@matf.bg.ac.rs])
AC_CONFIG_MACRO_DIR([m4])

dnl Identify where LLVM source tree is 
LLVM_SRC_ROOT=.

dnl Tell autoconf that the auxilliary files are actually located in
dnl the LLVM autoconf directory, not here.
AC_CONFIG_AUX_DIR($LLVM_SRC_ROOT/autoconf)

dnl Tell autoconf that this is an LLVM project being configured
dnl This provides the --with-llvmsrc and --with-llvmobj options
#LLVM_CONFIG_PROJECT("","")

dnl Verify that the source directory is valid
AC_CONFIG_SRCDIR(["Makefile.config.in"])

dnl Configure a common Makefile
AC_CONFIG_FILES(Makefile.config)

dnl Configure project makefiles
dnl List every Makefile that exists within your source tree
AC_CONFIG_HEADER(include/config.h)


AC_LANG([C++])

dnl **************************************************************************
dnl Verify that we can find llvm
dnl **************************************************************************

dnl --with-llvm is a shortcut for setting srcdir and objdir.
AC_ARG_WITH(llvm,
  AS_HELP_STRING([--with-llvm],
    [Location of LLVM Source and Object code]),,)

AC_MSG_CHECKING([llvm source dir])

if test X${with_llvm} != X; then
    dnl Verify that --with-llvm{src,obj} were not given.
    if test X${with_llvmsrc} != X; then
       AC_MSG_ERROR([--with-llvmsrc cannot be specified when using --with-llvm])
    fi   
    if test X${with_llvmobj} != X; then
       AC_MSG_ERROR([--with-llvmobj cannot be specified when using --with-llvm])
    fi   
    with_llvmsrc=$with_llvm
    with_llvmobj=$with_llvm
fi

dnl If one of with_llvmsrc or with_llvmobj was given, we must have both.
if (test X${with_llvmsrc} != X || test X${with_llvmobj} != X); then
    dnl Verify that with_llvmobj was given as well.
    if test X${with_llvmsrc} = X; then
       AC_MSG_ERROR([--with-llvmsrc must be specified when using --with-llvmobj])
    fi      
    if test X${with_llvmobj} = X; then
       AC_MSG_ERROR([--with-llvmobj must be specified when using --with-llvmsrc])
    fi      
else
    dnl Otherwise try and use llvm-config to find.
    llvm_version=`llvm-config --version`
    if test X${llvm_version} = X; then
       AC_MSG_ERROR([unable to find llvm, use --with-llvmsrc and --with-llvmobj])
    fi
    
    with_llvmsrc=`llvm-config --src-root`
    with_llvmobj=`llvm-config --obj-root`
fi

dnl Try to validate directories
if test ! -f ${with_llvmsrc}/Makefile.rules; then
   AC_MSG_ERROR([invalid llvmsrc directory: ${with_llvmsrc}])
fi
if test ! -f ${with_llvmobj}/Makefile.config; then
   AC_MSG_ERROR([invalid llvmobj directory: ${with_llvmobj}])
fi

dnl Make the paths absolute
llvm_src=`cd $with_llvmsrc 2> /dev/null; pwd`
llvm_obj=`cd $with_llvmobj 2> /dev/null; pwd`
   
AC_MSG_RESULT([$llvm_src])

dnl Report obj dir as well.
AC_MSG_CHECKING([llvm obj dir])
AC_MSG_RESULT([$llvm_obj])

AC_SUBST(LLVM_SRC,$llvm_src)
AC_SUBST(LLVM_OBJ,$llvm_obj)


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP


dnl **************************************************************************
dnl Check for solvers
dnl **************************************************************************
yices=no
AC_ARG_ENABLE([yices], AC_HELP_STRING([--enable-yices], [enable YICES solver]),
       [test "x$enableval" = "xyes" &&  yices=yes])
if test "x${yices}" = "xyes" ; then
       AC_DEFINE(YICES, 1, [YICES solver])
       AC_SUBST(ENABLE_YICES,[[1]])
       echo "-----------------------"
       echo "---  YICES enabled  ---"
       echo "-----------------------"
else
       AC_SUBST(ENABLE_YICES,[[0]])
fi

boolector=no
AC_ARG_ENABLE([boolector], AC_HELP_STRING([--enable-boolector], [enable BOOLECTOR solver]),
       [test "x$enableval" = "xyes" &&  boolector=yes])
if test "x${boolector}" = "xyes" ; then
       AC_DEFINE(BOOLECTOR, 1, [BOOLECTOR solver])
       AC_SUBST(ENABLE_BOOLECTOR,[[1]])
       echo "---------------------------"
       echo "---  BOOLECTOR enabled  ---"
       echo "---------------------------"
else
       AC_SUBST(ENABLE_BOOLECTOR,[[0]])
fi

mathsat=no
AC_ARG_ENABLE([mathsat], AC_HELP_STRING([--enable-mathsat], [enable MATHSAT solver]),
       [test "x$enableval" = "xyes" &&  mathsat=yes])
if test "x${mathsat}" = "xyes" ; then
       AC_DEFINE(MATHSAT, 1, [MATHSAT solver])
       AC_SUBST(ENABLE_MATHSAT,[[1]])
       echo "---------------------------"
       echo "---  MATHSAT enabled    ---"
       echo "---------------------------"
else
       AC_SUBST(ENABLE_MATHSAT,[[0]])
fi


z3=no
AC_ARG_ENABLE([z3], AC_HELP_STRING([--enable-z3], [enable Z3 solver]),
       [test "x$enableval" = "xyes" &&  z3=yes])
if test "x${z3}" = "xyes" ; then
       AC_DEFINE(Z3, 1, [Z3 solver])
       AC_SUBST(ENABLE_Z3,[[1]])
       echo "---------------------------"
       echo "---  Z3 enabled         ---"
       echo "---------------------------"
       # FIXME: Replace `main' with a function in `-lrt':
       AC_CHECK_LIB([rt], [main], [], [AC_MSG_RESULT([no])
                                    AC_MSG_ERROR([-lrt not found! ...])
                    ])
else
       AC_SUBST(ENABLE_Z3,[[0]])
fi

AC_CHECK_LIB([fl], [main], [], [])
AC_PROG_RANLIB
	
AC_CHECK_LIB([gmp], [main], [], [AC_MSG_RESULT([no])
  AC_MSG_ERROR([gmp not found! ...])
])
AC_CHECK_LIB([gmpxx], [main], [], [AC_MSG_RESULT([no])
  AC_MSG_ERROR([gmpxx not found! ...])
])


dnl **************************************************************************
dnl Checks for header files.

dnl NOTE: This is mostly just to force autoconf to make CFLAGS defines
dnl for us.
AC_LANG_PUSH([C])
AC_CHECK_HEADERS([sys/acl.h])
AC_LANG_POP([C])

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([sys/acl.h])
AC_LANG_POP([C++])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([inttypes.h libintl.h limits.h malloc.h memory.h stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit memset pow select strerror strstr])


dnl **************************************************************************
dnl * Create the output files
dnl **************************************************************************

dnl This must be last
AC_OUTPUT
