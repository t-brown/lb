/*
 * Copyright (C) 2014 Timothy Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file static.c
 * Static load balancing routines.
 *
 * \ingroup static
 * \{
 **/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <mpi.h>

#include "gettext.h"
#include "defs.h"
#include "mem.h"
#include "master.h"
#include "rfile.h"
#include "tags.h"

/**
 * The static load balancer master.
 *
 * \param[in] cmdfile The name of the command file.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 *
 **/
int
master(const char *cmd_file)
{
	int tasks = 0;		/* Worker task */
	int n_tasks = 0;	/* Number of MPI tasks */
	int len = 0;		/* Lenght of the command */
	int i = 0;		/* Temporary loop indexer */
	size_t n_cmds = 0;	/* Number of jobs/commands */
	char **cmds = NULL;	/* Commands from the file */
	MPI_Status status = {0};

	if (read_file(cmd_file, &cmds, &n_cmds)) {
		fprintf(stderr, "Unable to prase the command file.");
		MPI_Abort(MPI_COMM_WORLD, EX_DATAERR);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);

	for (i = 0; i < n_cmds; ++i) {
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
			  &status);
		if (status.MPI_TAG == TAG_READY) {
			++tasks;
			len = strlen(cmds[i]);
			MPI_Recv(NULL, 0, MPI_BYTE, status.MPI_SOURCE,
				 TAG_READY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(cmds[i], len, MPI_CHAR, status.MPI_SOURCE,
				 TAG_DATA, MPI_COMM_WORLD);
		}
	}

	/* If we have more workers than commands */
	for (tasks; tasks < n_tasks; ++tasks) {
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
			  &status);
		if (status.MPI_TAG == TAG_READY) {
			MPI_Recv(NULL, 0, MPI_BYTE, status.MPI_SOURCE,
				 TAG_READY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	for (i = 1; i < n_tasks; ++i) {
		MPI_Send(NULL, 0, MPI_BYTE, i, TAG_FINI, MPI_COMM_WORLD);
	}

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
