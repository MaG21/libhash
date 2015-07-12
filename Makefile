# $Id: Makefile,v 1.7 2005/02/06 17:19:51 andrew Exp $

LIB=	hash
INCS=	hash.h
SRCS=	${LIB}.c
MAN3=	lib${LIB}.3 lib${LIB}_convenience.3
MLINKS=	lib${LIB}.3 hash_initialise.3 \
		lib${LIB}.3 hash_insert.3 \
		lib${LIB}.3 hash_retrieve.3 \
		lib${LIB}.3 hash_delete.3 \
		lib${LIB}.3 hash_iterator_initialise.3 \
		lib${LIB}.3 hash_fetch_next.3 \
		lib${LIB}.3 hash_iterator_deinitialise.3 \
		lib${LIB}.3 hash_deinitialise.3 \
		lib${LIB}_convenience.3 hash_compare_int.3 \
		lib${LIB}_convenience.3 hash_compare_string.3 \
		lib${LIB}_convenience.3 hash_hash_int.3 \
		lib${LIB}_convenience.3 hash_hash_string.3 \
		lib${LIB}_convenience.3 hash_copy_int.3 \
		lib${LIB}_convenience.3 hash_copy_string.3

SHLIB_MAJOR=	1

PREFIX?=	/usr/local
INCLUDEDIR=	${PREFIX}/include
LIBDIR=	${PREFIX}/lib
MANDIR=	${PREFIX}/man/man

.if defined(DEBUG)
# lots of warnings
CFLAGS=	-W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual \
		-Wchar-subscripts -Wmissing-prototypes -Wnested-externs \
		-Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes \
		-Wwrite-strings
# include debugging symobls
CFLAGS+= -g
# only build static library when debugging
.undef SHLIB_MAJOR
.endif

CFLAGS+= -I${PREFIX}/include -L${PREFIX}/lib

.include <bsd.lib.mk>
