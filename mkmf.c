/*
 *  mkmf.c : makefile editor (Version 1.2a)
 *                                           Oct, 1989 Syuichi Takahashi.
 *                            x68k modified  Dec, 1990 Yoshiharu Dewa(Awed)
 */
#include	<ctype.h>
#include	<stdlib.h>
#include	<io.h>
#include	<stdio.h>
#include	<string.h>
#include	<signal.h>
#include	"mkmf.h"

int lochc;
char *lochdrs[MaxFiles];

int exthc;
char *exthdrs[MaxFiles];

int srcfcnt;
struct source_ srcfn[MaxFiles];

int maccnt;
struct macro_ macro[MaxFiles];

struct flag_ flg = { Off, Off, Off, Off, Off, Off, Off, Off, Off, Off, Off};
char *mkfname, *tmplfname, *linkfname;
char *extincpath[16];

int Dcnt, Slen;
char *Dtag[16];

static char libname[64], progname[64], destdirname[64];
static FILE *fp, *ftmp;

FILE *fopen ();
void separ_putlist (FILE *, char *);

void main (argc, argv)
int argc;
char *argv[];
{
	int i, n;
	static char buf[BufSize];
	static char tmpfname[64];

	signal( SIGINT, CTRL_C_abort);
	Dcnt = maccnt = 0;
	mkfname = tmplfname = linkfname = (char *)0;

	getincpath (extincpath);
 	parse_envarg ();
 	parse_arg (argc, argv);
	setdefname ();
	makebakfile (mkfname);

	fp = opentemplate ();

	gettmpfn (tmpfname);
	if ((ftmp = fopen (toyen (tmpfname), "w+")) == NULL) {
		printf ("mkmf: can't open working file '%s'\n", tmpfname);
		exit (8);
	}

	while (getline (fp, buf) != EOF) {
		if (!strncmp (skipsp (buf), "PROGRAM", 7))
			if (checkmacro ("PROGRAM") == -1) savemacro (buf);

		if (!strncmp (skipsp (buf), "CFLAGS", 6))
			if (checkmacro ("CFLAGS") == -1) savemacro (buf);

		if (!strncmp (skipsp (buf), "LDFLAGS", 7))
			if (checkmacro ("LDFLAGS") == -1) savemacro (buf);

		if (!strncmp (skipsp (buf), "LIBS", 4))
			if (checkmacro ("LIBS") == -1) savemacro (buf);

		if (!strncmp (skipsp (buf), "SUFFIX", 6))
			if (checkmacro ("SUFFIX") == -1) savemacro (buf);

		fprintf (ftmp, "%s\n", buf);
	}

	fclose (fp);
	rewind (ftmp);

	if (flg.i) prompt ();

	if ((i = checkmacro ("SUFFIX")) != -1)
		remake_suffix (macro[i].value);
	if ((i = checkmacro ("CFLAGS")) != -1)
		parse_cflags (macro[i].value);

	scan_srcfn ();
	update_srclist ();

	fp = opennewmakefile ();

	while (getline (ftmp, buf) != EOF) {
		if ((i = checkmacro (skipsp (buf))) != -1) {

			for (n = 0; buf[n] != EOT && buf[n] != '='; n++)
				fputc ((int)buf[n], fp);

			if (flg.S) {
				if (!strncmp (buf, "SRCS", 4))
					separ_putlist (fp, macro[i].value);
				else
				if (!strncmp (buf, "OBJS", 4))
					separ_putlist (fp, macro[i].value);
				else
				if (!strncmp (buf, "HDRS", 4))
					separ_putlist (fp, macro[i].value);
				else
				if (!strncmp (buf, "EXTHDRS", 7))
					separ_putlist (fp, macro[i].value);
				else
					fprintf (fp, "= %s\n", macro[i].value);
			}
			else
				fprintf (fp, "= %s\n", macro[i].value);

			while (check_contchar (buf))
				getline (ftmp, buf);
 		}
		else
			if (!strncmp (buf, "###", 3)) break;
		else
			fprintf (fp, "%s\n", buf);
	}

	if (flg.d == Off)
		put_depinf (fp);

	fclose (fp);
	fclose (ftmp);
	unlink (tmpfname);

	if (flg.L) {
		makebakfile (linkfname);
		put_respfile ();
	}

	exit (EXIT_SUCCESS);
}

void separ_putlist (fp, buf)
FILE *fp;
char *buf;
{
	int fstflg;
	static char buf2[256], *p;

	if (*buf == EOT) {
		fprintf (fp, "=\n");
		return;
	}
	for (fstflg = On, p = buf2; *buf != EOT; *p++ = *buf++) {
		if (isspace ((int)*buf)) {
			if (((int)(p - buf2) + strnextlen(buf)) > Slen) {
			   *p = EOT;
			   if (fstflg) {
			      fprintf (fp, "= %s \\\n", skipsp(buf2));
			      fstflg = Off;
			   }
			   else
			      fprintf (fp, "		%s \\\n",skipsp(buf2));
			   p = buf2;
			}
		}
	}
	if (p != buf2) {
		*p = EOT;
		if (fstflg)
			fprintf (fp, "= %s\n", skipsp(buf2));
		else
			fprintf (fp, "		%s\n", skipsp(buf2));
	}
	return;
}

int strnextlen(s)
char *s;
{
	int i;

	for (i = 0, s = skipsp(s); *s != EOT && !isspace ((int)*s); s++, i++);
	return (i);
}

int check_contchar (buf)
char *buf;
{
	for (buf += strlen (buf) - 1;; buf--) {
		if (*buf == '\\') return (Match);
		if (!isspace ((int)*buf)) break;
	}
	return (NoMatch);
}

void scan_srcfn (void)
{
	int i;

	lochc = exthc = srcfcnt = 0;

	if ((i = checkmacro ("SRCS")) != -1)
		get_argsfn (macro[i].value);
	else
		get_dirsfn ();
	for (i = 0; i < srcfcnt; i++) {
		if (checkdir (srcfn[i].name))
			switch (srcfn[i].langtype) {
				case L_C:
					scan_Cinc (i, srcfn[i].name);
					break;
				case L_ASM:
					scan_ASMinc (i, srcfn[i].name);
					break;
			}
		else
			printf ("%s: No such file or directory\n", srcfn[i].name);
	}
	return;
}

void update_srclist (void)
{
	int i;
	static char buf[512];

	if (checkmacro ("SRCS") == -1) {
	   for (strcpy (buf, "SRCS="), i = 0; i < srcfcnt; i++) {
	      strcat (buf, " ");
	      strcat (buf, tolowers (changesepar (srcfn[i].name)));
	   }
	   savemacro (buf);
	}

	if (checkmacro ("OBJS") == -1) {
	   for (strcpy (buf, "OBJS="), i = 0; i < srcfcnt; i++) {
	      strcat (buf, " ");
              strcat (buf, tolowers(changesepar(get_objname(srcfn[i].name))));
	   }
	   savemacro (buf);
	}

	if (checkmacro ("HDRS") == -1) {
	   for (strcpy (buf, "HDRS="), i = 0; i < lochc; i++) {
	      strcat (buf, " ");
	      strcat (buf, tolowers (changesepar (lochdrs[i])));
	   }
	   savemacro (buf);
	}

	if (checkmacro ("EXTHDRS") == -1) {
	   for (strcpy (buf, "EXTHDRS="), i = 0; i < exthc; i++) {
	      strcat (buf, " ");
	      strcat (buf, tolowers (changesepar (exthdrs[i])));
	   }
	   savemacro (buf);
	}
	return;
}

void get_argsfn (s)
char *s;
{
	char *buf;

	srcfcnt = 0;
	buf = skipsp (alloccopy(s));
	if (*buf == EOT) return;

	for (srcfn[srcfcnt].name = buf; *buf != EOT; buf++) {
		if (isspace ((int)*buf)) {
			*buf++ = EOT;
			srcfn[++srcfcnt].name = buf;
		}
	}
	srcfcnt++;
	return;
}

void get_dirsfn (void)
{
	int i, n, ltype;
	static char *fnlist[256];

	n = getdir (fnlist);
	for (srcfcnt = i = 0; i < n; i++) {
		if ((ltype = get_srcftype (fnlist[i])) != -1) {
			srcfn[srcfcnt].name = fnlist[i];
			srcfn[srcfcnt].langtype = ltype;
			srcfcnt++;
		}
	}
	return;
}

void prompt (void)
{
	static char buf[BufSize];

	if (flg.l) {
		do {
			con_getline ("library name ? ", libname);
		} while (strlen (libname) == 0);
		sprintf (buf, "LIBRARY=%s", tolowers(changesepar(libname)));
		savemacro (buf);
	}
	else {
		do {
			con_getline ("program name ? ", progname);
		} while (strlen (progname) == 0);
		sprintf (buf, "PROGRAM=%s", tolowers(changesepar(progname)));
		savemacro (buf);
	}
	con_getline ("destination directory ? ", destdirname);
	if (strlen (destdirname) > 0) {
		sprintf (buf, "DEST=%s", tolowers(changesepar(destdirname)));
		savemacro (buf);
	}
	return;
}

void put_depinf (fp)
FILE *fp;
{
	int i, j;

	fprintf (fp, "###\n");
	for (i = 0; i < srcfcnt; i++) {
		if (srcfn[i].incfcnt > 0) {
			fprintf (fp, "%s:", tolowers (changesepar (get_objname (srcfn[i].name))));
			for (j = 0; j < srcfn[i].incfcnt; j++)
				fprintf (fp, " %s", tolowers (changesepar (srcfn[i].incfn[j])));
         		fprintf (fp, "\n");
		}
	}
	return;
}

void put_respfile (void)
{
	FILE *fp;
	int i;

	if ((fp = fopen(linkfname, "w")) == NULL) {
	   printf ("mkmf:cannot open link response file '%s'\n", linkfname);
	   exit (8);
	}

	for (i = 0; i < srcfcnt; i++) {
		fprintf(fp, "%s",tolowers(changesepar(get_objname(srcfn[i].name))));
		if (i + 1 < srcfcnt) fprintf (fp, "+\n");
	}
	fprintf (fp, "\n");
	if ((i = checkmacro ("PROGRAM")) != -1)
		fprintf (fp, "%s", macro[i].value);
	if ((i = checkmacro ("LDFLAGS")) != -1)
		fprintf (fp, "%s\n", macro[i].value);
	fprintf (fp, "NUL\n", macro[i].value);
	if ((i = checkmacro ("LIBS")) != -1)
		fprintf (fp, "%s;\n", macro[i].value);

	fclose (fp);
	return;
}

void abort_exit ( void )
{
	exit (8);
}

void CTRL_C_abort ( void )
{
	fprintf (stderr, "aborted.\n");
	exit (8);
}
