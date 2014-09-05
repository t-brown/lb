# Makefile.am for the MPI serial load balancer.
#

AUTOMAKE_OPTIONS = nostdinc

bin_PROGRAMS     = lb

lb_SOURCES       = main.c			\
		   defs.h			\
		   options.h			\
		   master.c	master.h	\
		   mem.c	mem.h		\
		   rfile.c	rfile.h		\
		   tags.h			\
		   worker.c	worker.h

noinst_HEADERS   = gettext.h
dist_man_MANS    = lb.1
