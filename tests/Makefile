# $Id: Makefile,v 1.1.1.1 2002/01/13 16:22:31 andrew Exp $

SRCS=	${PROG}.c
LDADD=	-lhash -lwrappers
NOMAN=	yes

PREFIX?=	/usr/local
BINDIR=	${PREFIX}/sbin
MANDIR=	${PREFIX}/man/man

.if defined(DEBUG)
CFLAGS=	-W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual \
	-Wchar-subscripts -Wmissing-prototypes -Wnested-externs \
	-Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes \
	-Wwrite-strings -g
.endif

CFLAGS+= -static -I.. -I- -I${PREFIX}/include -L.. -L${PREFIX}/lib

.include <bsd.prog.mk>
