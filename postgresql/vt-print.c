
#include <stdlib.h>
#include <string.h>
#include "vt-print.h"

#define bool int
#define false   0
#define true    1

static void
do_field(const PQprintOpt *po, const PGresult *res,
		 const int i, const int j, const int fs_len,
		 char **fields,
		 const int nFields, char const ** fieldNames,
		 unsigned char *fieldNotNum, int *fieldMax,
		 const int fieldMaxLen, FILE *fout)
{

	const char *pval,
			   *p;
	int			plen;
	bool		skipit;

	plen = PQgetlength(res, i, j);
	pval = PQgetvalue(res, i, j);

	if (plen < 1 || !pval || !*pval)
	{
		if (po->align || po->expanded)
			skipit = true;
		else
		{
			skipit = false;
			goto efield;
		}
	}
	else
		skipit = false;

	if (!skipit)
	{
		if (po->align && !fieldNotNum[j])
		{
			/* Detect whether field contains non-numeric data */
			char		ch = '0';

			for (p = pval; *p; p += PQmblen(p, PQenv2encoding()))
			{
				ch = *p;
				if (!((ch >= '0' && ch <= '9') ||
					  ch == '.' ||
					  ch == 'E' ||
					  ch == 'e' ||
					  ch == ' ' ||
					  ch == '-'))
				{
					fieldNotNum[j] = 1;
					break;
				}
			}

			/*
			 * Above loop will believe E in first column is numeric; also, we
			 * insist on a digit in the last column for a numeric. This test
			 * is still not bulletproof but it handles most cases.
			 */
			if (*pval == 'E' || *pval == 'e' ||
				!(ch >= '0' && ch <= '9'))
				fieldNotNum[j] = 1;
		}

		if (!po->expanded && (po->align || po->html3))
		{
			if (plen > fieldMax[j])
				fieldMax[j] = plen;
			if (!(fields[i * nFields + j] = (char *) malloc(plen + 1)))
			{
				fprintf(stderr, "out of memory\n");
				exit(1);
			}
			strcpy(fields[i * nFields + j], pval);
		}
		else
		{
			if (po->expanded)
			{
				if (po->html3)
					fprintf(fout,
							"<tr><td align=\"left\"><b>%s</b></td>"
							"<td align=\"%s\">%s</td></tr>\n",
							fieldNames[j],
							fieldNotNum[j] ? "left" : "right",
							pval);
				else
				{
					if (po->align)
						fprintf(fout,
								"%-*s%s %s\n",
								fieldMaxLen - fs_len, fieldNames[j],
								po->fieldSep,
								pval);
					else
						fprintf(fout,
								"%s%s%s\n",
								fieldNames[j], po->fieldSep, pval);
				}
			}
			else
			{
				if (!po->html3)
				{
					fputs(pval, fout);
			efield:
					if ((j + 1) < nFields)
						fputs(po->fieldSep, fout);
					else
						fputc('\n', fout);
				}
			}
		}
	}
}


static char *
do_header(FILE *fout, const PQprintOpt *po, const int nFields, int *fieldMax,
		  const char **fieldNames, unsigned char *fieldNotNum,
		  const int fs_len, const PGresult *res)
{

	int			j;				/* for loop index */
	char	   *border = NULL;

	if (po->html3)
		fputs("<tr>", fout);
	else
	{
		int			tot = 0;
		int			n = 0;
		char	   *p = NULL;

		for (; n < nFields; n++)
			tot += fieldMax[n] + fs_len + (po->standard ? 2 : 0);
		if (po->standard)
			tot += fs_len * 2 + 2;
		border = malloc(tot + 1);
		if (!border)
		{
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
		p = border;
		if (po->standard)
		{
			char	   *fs = po->fieldSep;

			while (*fs++)
				*p++ = '+';
		}
		for (j = 0; j < nFields; j++)
		{
			int			len;

			for (len = fieldMax[j] + (po->standard ? 2 : 0); len--; *p++ = '-');
			if (po->standard || (j + 1) < nFields)
			{
				char	   *fs = po->fieldSep;

				while (*fs++)
					*p++ = '+';
			}
		}
		*p = '\0';
		if (po->standard)
			fprintf(fout, "%s\n", border);
	}
	if (po->standard)
		fputs(po->fieldSep, fout);
	for (j = 0; j < nFields; j++)
	{
		const char *s = PQfname(res, j);

		if (po->html3)
		{
			fprintf(fout, "<th align=\"%s\">%s</th>",
					fieldNotNum[j] ? "left" : "right", fieldNames[j]);
		}
		else
		{
			int			n = strlen(s);

			if (n > fieldMax[j])
				fieldMax[j] = n;
			if (po->standard)
				fprintf(fout,
						fieldNotNum[j] ? " %-*s " : " %*s ",
						fieldMax[j], s);
			else
				fprintf(fout, fieldNotNum[j] ? "%-*s" : "%*s", fieldMax[j], s);
			if (po->standard || (j + 1) < nFields)
				fputs(po->fieldSep, fout);
		}
	}
	if (po->html3)
		fputs("</tr>\n", fout);
	else
		fprintf(fout, "\n%s\n", border);
	return border;
}



static void
output_row(FILE *fout, const PQprintOpt *po, const int nFields, char **fields,
		   unsigned char *fieldNotNum, int *fieldMax, char *border,
		   const int row_index)
{

	int			field_index;	/* for loop index */

	if (po->html3)
		fputs("<tr>", fout);
	else if (po->standard)
		fputs(po->fieldSep, fout);
	for (field_index = 0; field_index < nFields; field_index++)
	{
		char	   *p = fields[row_index * nFields + field_index];

		if (po->html3)
			fprintf(fout, "<td align=\"%s\">%s</td>",
					fieldNotNum[field_index] ? "left" : "right", p ? p : "");
		else
		{
			fprintf(fout,
					fieldNotNum[field_index] ?
					(po->standard ? " %-*s " : "%-*s") :
					(po->standard ? " %*s " : "%*s"),
					fieldMax[field_index],
					p ? p : "");
			if (po->standard || field_index + 1 < nFields)
				fputs(po->fieldSep, fout);
		}
		if (p)
			free(p);
	}
	if (po->html3)
		fputs("</tr>", fout);
	else if (po->standard)
		fprintf(fout, "\n%s", border);
	fputc('\n', fout);
}



/*
 * PQprint()
 *
 * Format results of a query for printing.
 *
 * PQprintOpt is a typedef (structure) that containes
 * various flags and options. consult libpq-fe.h for
 * details
 *
 * This function should probably be removed sometime since psql
 * doesn't use it anymore. It is unclear to what extent this is used
 * by external clients, however.
 */
void
vtPQprint(FILE *fout, const PGresult *res, const PQprintOpt *po, const int pTuple)
{
	int			nFields;

	nFields = PQnfields(res);

	if (nFields > 0)
	{							/* only print rows with at least 1 field.  */
		int			i,
					j;
		int			nTups;
		int		   *fieldMax = NULL;	/* in case we don't use them */
		unsigned char *fieldNotNum = NULL;
		char	   *border = NULL;
		char	  **fields = NULL;
		const char **fieldNames;
		int			fieldMaxLen = 0;
		int			numFieldName;
		int			fs_len = strlen(po->fieldSep);
		int			total_line_length = 0;
		int			usePipe = 0;
		char	   *pagerenv;

#if defined(ENABLE_THREAD_SAFETY) && !defined(WIN32)
		sigset_t	osigset;
		bool		sigpipe_masked = false;
		bool		sigpipe_pending;
#endif

#ifdef TIOCGWINSZ
		struct winsize screen_size;
#else
		struct winsize
		{
			int			ws_row;
			int			ws_col;
		}			screen_size;
#endif

		nTups = PQntuples(res);
		if (!(fieldNames = (const char **) calloc(nFields, sizeof(char *))))
		{
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
		if (!(fieldNotNum = (unsigned char *) calloc(nFields, 1)))
		{
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
		if (!(fieldMax = (int *) calloc(nFields, sizeof(int))))
		{
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
		for (numFieldName = 0;
			 po->fieldName && po->fieldName[numFieldName];
			 numFieldName++)
			;
		for (j = 0; j < nFields; j++)
		{
			int			len;
			const char *s = (j < numFieldName && po->fieldName[j][0]) ?
			po->fieldName[j] : PQfname(res, j);

			fieldNames[j] = s;
			len = s ? strlen(s) : 0;
			fieldMax[j] = len;
			len += fs_len;
			if (len > fieldMaxLen)
				fieldMaxLen = len;
			total_line_length += len;
		}

		total_line_length += nFields * strlen(po->fieldSep) + 1;

		if (fout == NULL)
			fout = stdout;
		if (po->pager && fout == stdout && isatty(fileno(stdin)) &&
			isatty(fileno(stdout)))
		{
			/*
			 * If we think there'll be more than one screen of output, try to
			 * pipe to the pager program.
			 */
#ifdef TIOCGWINSZ
			if (ioctl(fileno(stdout), TIOCGWINSZ, &screen_size) == -1 ||
				screen_size.ws_col == 0 ||
				screen_size.ws_row == 0)
			{
				screen_size.ws_row = 24;
				screen_size.ws_col = 80;
			}
#else
			screen_size.ws_row = 24;
			screen_size.ws_col = 80;
#endif
			pagerenv = getenv("PAGER");
			if (pagerenv != NULL &&
				pagerenv[0] != '\0' &&
				!po->html3 &&
				((po->expanded &&
				  nTups * (nFields + 1) >= screen_size.ws_row) ||
				 (!po->expanded &&
				  nTups * (total_line_length / screen_size.ws_col + 1) *
				  (1 + (po->standard != 0)) >= screen_size.ws_row -
				  (po->header != 0) *
				  (total_line_length / screen_size.ws_col + 1) * 2
				  - (po->header != 0) * 2		/* row count and newline */
				  )))
			{
				fout = popen(pagerenv, "w");
				if (fout)
				{
					usePipe = 1;

/*
#ifndef WIN32
#ifdef ENABLE_THREAD_SAFETY
					if (pq_block_sigpipe(&osigset, &sigpipe_pending) == 0)
						sigpipe_masked = true;
#else
					oldsigpipehandler = pqsignal(SIGPIPE, SIG_IGN);
#endif
#endif
*/
                                }
				else
					fout = stdout;
			}
		}

		if (!po->expanded && (po->align || po->html3))
		{
			if (!(fields = (char **) calloc(nFields * (nTups + 1), sizeof(char *))))
			{
				fprintf(stderr, "out of memory\n");
				exit(1);
			}
		}
		else if (po->header && !po->html3)
		{
			if (po->expanded)
			{
				if (po->align)
					fprintf(fout, "%-*s%s Value\n",
							fieldMaxLen - fs_len, "Field", po->fieldSep);
				else
					fprintf(fout, "%s%sValue\n", "Field", po->fieldSep);
			}
			else
			{
				int			len = 0;

				for (j = 0; j < nFields; j++)
				{
					const char *s = fieldNames[j];

					fputs(s, fout);
					len += strlen(s) + fs_len;
					if ((j + 1) < nFields)
						fputs(po->fieldSep, fout);
				}
				fputc('\n', fout);
				for (len -= fs_len; len--; fputc('-', fout));
				fputc('\n', fout);
			}
		}
		if (po->expanded && po->html3)
		{
			if (po->caption)
				fprintf(fout, "<center><h2>%s</h2></center>\n", po->caption);
			else
				fprintf(fout,
						"<center><h2>"
						"Query retrieved %d rows * %d fields"
						"</h2></center>\n",
						nTups, nFields);
		}
		for (i = 0; i < nTups; i++)
		{
			if (po->expanded)
			{
				if (po->html3)
					fprintf(fout,
							"<table %s><caption align=\"top\">%d</caption>\n",
							po->tableOpt ? po->tableOpt : "", i);
				else
					fprintf(fout, "-- RECORD %d --\n", i);
			}
			for (j = 0; j < nFields; j++)
				do_field(po, res, i, j, fs_len, fields, nFields,
						 fieldNames, fieldNotNum,
						 fieldMax, fieldMaxLen, fout);
			if (po->html3 && po->expanded)
				fputs("</table>\n", fout);
		}
		if (!po->expanded && (po->align || po->html3))
		{
			if (po->html3)
			{
				if (po->header)
				{
					if (po->caption)
						fprintf(fout,
						   "<table %s><caption align=\"top\">%s</caption>\n",
								po->tableOpt ? po->tableOpt : "",
								po->caption);
					else
						fprintf(fout,
								"<table %s><caption align=\"top\">"
								"Retrieved %d rows * %d fields"
								"</caption>\n",
						   po->tableOpt ? po->tableOpt : "", nTups, nFields);
				}
				else
					fprintf(fout, "<table %s>", po->tableOpt ? po->tableOpt : "");
			}
			if (po->header)
				border = do_header(fout, po, nFields, fieldMax, fieldNames,
								   fieldNotNum, fs_len, res);

/* P3k: added an ability to print a single tuple only */
                        if (pTuple > -1) {
                		output_row(fout, po, nFields, fields,
						   fieldNotNum, fieldMax, border, pTuple);
                        }
                        else
/* P3k: until here */
                            for (i = 0; i < nTups; i++)
				output_row(fout, po, nFields, fields,
						   fieldNotNum, fieldMax, border, i);


                        free(fields);
			if (border)
				free(border);
		}
		if (po->header && !po->html3)
			fprintf(fout, "(%d row%s)\n\n", PQntuples(res),
					(PQntuples(res) == 1) ? "" : "s");
		free(fieldMax);
		free(fieldNotNum);
		free((void *) fieldNames);
		if (usePipe)
		{
#ifdef WIN32
			_pclose(fout);
#else
			pclose(fout);

/*
#ifdef ENABLE_THREAD_SAFETY
			// we can't easily verify if EPIPE occurred, so say it did
			if (sigpipe_masked)
				pq_reset_sigpipe(&osigset, sigpipe_pending, true);
#else
			pqsignal(SIGPIPE, oldsigpipehandler);
#endif
*/
#endif 
		}
		if (po->html3 && !po->expanded)
			fputs("</table>\n", fout);
	}
}