# $FreeBSD$

# XXX: this should be defined in bsd.sys.mk
CXXSTD?=	c++11

GTESTS_CXXFLAGS+= -DGTEST_HAS_POSIX_RE=1
GTESTS_CXXFLAGS+= -DGTEST_HAS_PTHREAD=1
GTESTS_CXXFLAGS+= -DGTEST_HAS_STREAM_REDIRECTION=1
GTESTS_CXXFLAGS+= -frtti
GTESTS_CXXFLAGS+= -std=${CXXSTD}

# XXX: src.libnames.mk should handle adding this directory for libgtest's,
# libgmock's, etc, headers.
CXXFLAGS+=	-I${DESTDIR}${INCLUDEDIR}/private

NO_WTHREAD_SAFETY=
