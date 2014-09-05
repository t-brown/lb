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
 * \file worker.c
 * Worker load balancing routines.
 *
 * \ingroup worker
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
#include "rfile.h"
#include "tags.h"

/**
 * The load balancer worker.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 *
 **/
int
worker(void)
{
	int err = 0;
	int terr = 0;
	int len = 0;
	int rank = 0;
	int done = 0;
	char *cmd = NULL;
	const char msg[] = "WARNING: %s\nWARNING: Exited with %d\n";
	MPI_Status status = {0};

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Send(NULL, 0, MPI_BYTE, 0, TAG_READY, MPI_COMM_WORLD);

	while(!done) {
		MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == TAG_DATA) {
			MPI_Get_count(&status, MPI_CHAR, &len);
			cmd = xmalloc((len+1)*sizeof(char));
			MPI_Recv(cmd, len, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD,
				 &status);
			err = system(cmd);
			if (err) {
				fprintf(stderr, _(msg), cmd, err);
				terr += err;
			}
			if (cmd) {
				free(cmd);
				cmd = NULL;
			}
			MPI_Send(NULL, 0, MPI_BYTE, 0, TAG_READY,
				 MPI_COMM_WORLD);
		} else if (status.MPI_TAG == TAG_FINI) {
			MPI_Recv(NULL, 0, MPI_INT, 0, TAG_FINI, MPI_COMM_WORLD,
				 MPI_STATUS_IGNORE);
			done = 1;
		}
	}
	return(terr);
}

/**
 * \}
 **/
