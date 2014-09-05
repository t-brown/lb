
/*
 * Copyright (C) 2014  Timothy Brown
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file main.c
 * Main entry point for the program.
 *
 * \ingroup main
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <locale.h>
#include "gettext.h"

#include <getopt.h>

#include <mpi.h>

#include "defs.h"
#include "mem.h"
#include "master.h"
#include "worker.h"


/* Internal functions */
static void print_usage(void);
static void print_version(void);
static int  parse_argv(int, char **, char **);
static const char *program_name(void);

/**
 * The main entry point of the program.
 *
 * \param argc   Number of command line arguments.
 * \param argv  Reference to the pointer to the argument array list.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
main(int argc, char **argv)
{

	int err = 0;		/* Error value */
	int rank = 0;		/* Local rank */
	char *cmd_file = NULL;	/* Filename with the commands to run */

	if (MPI_Init(&argc, &argv)) {
		return(EXIT_FAILURE);
	}

#ifdef HAVE_SETLOCALE
	setlocale(LC_ALL, "");
#endif
#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	if (parse_argv(argc, argv, &cmd_file)) {
		return(EXIT_FAILURE);
	}

	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank)) {
		return(EXIT_FAILURE);
	}

	if (rank == 0) {
		err = master(cmd_file);
	} else {
		err = worker();
	}

	if (cmd_file) {
		free(cmd_file);
		cmd_file = NULL;
	}

	/* Some MPI (mpiexec) implementations do not
	 * propergate the error code when just calling
	 * return().
	 */
#if 0
	MPI_Barrier(MPI_COMM_WORLD);
	if (err) {
		MPI_Abort(MPI_COMM_WORLD, err);
	}
#endif

	MPI_Finalize();
	return(err);
}

/**
 * Parse the command line arguments.
 *
 * \param[in] argc Number of command line arguments.
 * \param[in] argv Reference to the pointer to the argument array list.
 *
 * \retval 0 If there were no errors.
 **/
static int
parse_argv(int argc, char **argv, char **name)
{
	int opt = 0;
	int opt_index = 0;
	char *soptions = "hv";                  /* short options structure */
	static struct option loptions[] = {     /* long options structure */
		{"help",       no_argument,        NULL,  'h'},
		{"version",    no_argument,        NULL,  'v'},
		{NULL,         1,                  NULL,  0}
	};

	/* parse the arguments */
	while ((opt = getopt_long(argc, argv, soptions, loptions,
				  &opt_index)) != -1) {
		switch (opt) {
		case 'v':
			print_version();
			break;
		case 'h':
			print_usage();
			break;
		default:
			print_usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		warnx(_("Must specify a filename for command to run."));
		print_usage();
	}

	*name = (char *) xmalloc((strlen(argv[0]) +1) * sizeof(char));
	strcpy(*name, argv[0]);

	return(EXIT_SUCCESS);
}

/**
 * Prints a short program usage statement, explaining the
 * command line arguments and flags expected.
 **/
static void
print_usage(void)
{
	printf(_("\
usage: %s [-h] [-v] cmd_file\n\
  -h, --help         Display this help and exit.\n\
  -V, --version      Display version information and exit.\n\
  cmd_file           A file containing commands to be run.\n\
"), program_name());
	MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
}

/**
 * Prints the program version number, copyright information and
 * compile date.
 **/
static void
print_version(void)
{
	printf(_("%s (GNU %s) %s\n"), program_name(), PACKAGE, VERSION);
	printf(_("\
Copyright (C) %s Timothy Brown.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n"), "2014");
	printf(_("Compiled on %s at %s.\n\n"), __DATE__, __TIME__);
	MPI_Finalize();
	exit(EXIT_SUCCESS);
}


static const char *
program_name(void)
{
#if HAVE_GETPROGNAME
	return getprogname();
#else
#if HAVE_PROGRAM_INVOCATION_SHORT_NAME
	return program_invocation_short_name;
#else
	return "unknown";
#endif /* HAVE_PROGRAM_INVOCATION_SHORT_NAME */
#endif /* HAVE_GETPROGNAME */
}

/**
 * \}
 **/
