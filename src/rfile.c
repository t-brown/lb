
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
 * \file rfile.c
 * Routines to read the command file.
 *
 * \ingroup rfile
 * \{
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <locale.h>
#include <unistd.h>
#include "gettext.h"
#include "defs.h"
#include "mem.h"
#include "rfile.h"

#ifndef LINE_MAX
#define LINE_MAX          sysconf(_SC_LINE_MAX)
#endif

/**
 * Read the command file.
 *
 * \param[in] cmd_file The file containing all the commands.
 * \param[out] cmds The commands to run.
 * \param[out] n The number of commands to run.
 *
 * \retval 0 If there were no errors.
 * \retval 1 If an error was encounted.
 **/
int
read_file(const char *cmd_file, char ***cmds, size_t *n)
{

	int i  = 0;                    /* Temporary loop indexer */
	int len = 0;                   /* String length */
	FILE *ifd = NULL;              /* File descriptor */
	char line[LINE_MAX];           /* Read line from file */

	/* fail silently in case the user does not have an rc file */
	if ((ifd = fopen(cmd_file, "r")) == NULL) {
		return(EXIT_FAILURE);
	}

	*n = 0;
	while (fgets(line, LINE_MAX, ifd) != NULL) {
		++(*n);
	}
	rewind(ifd);

	*cmds = (char **)xmalloc((*n) * sizeof(char *));
	while (fgets(line, LINE_MAX, ifd) != NULL) {
		len = strlen(line);
		(*cmds)[i] = xmalloc((len +1)* sizeof(char));
		if (line[len-1] == '\n') {
			strncpy((*cmds)[i], line, len-1);
		} else {
			strncpy((*cmds)[i], line, len);
		}
		++i;
	}

	if (fclose(ifd)) {
		warn(_("Unable to close %s"), cmd_file);
	}

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
