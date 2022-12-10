/*
 *  file.c
 *
 */
#include	<stdio.h>
#include	<string.h>
#include	"mkmf.h"

extern char *mkfname, *tmplfname;
extern struct flag_ flg;

FILE *fopen ();
char *getenv ( char * );

FILE *opennewmakefile ( void )
{
	FILE *fp;

	if ((fp = fopen (toyen (mkfname), "w")) == NULL) {
		printf ("mkmf: can't open '%s'\n", mkfname);
		abort_exit ();
	}
	return (fp);
}

FILE *opentemplate ( void )
{
	FILE *fp;
	char *path;

	if ((fp = fopen (toyen (mkfname), "r")) == NULL) {
		path = gettemplpath ();
		if ((fp = fopen (toyen (path), "r")) == NULL) {
			maketemplate();
			if ((fp = fopen (toyen (mkfname), "r")) == NULL)
				abort_exit ();
		}
		else
			if (!flg.c)
				printf ("mkmf: creating %s from template %s\n", mkfname, path);
	}
	return (fp);
}

void maketemplate (void)
{
	FILE *fp;

	if ((fp = fopen (toyen (mkfname), "w")) == NULL) abort_exit ();

fprintf (fp, "DEST          = .\n\n");
fprintf (fp, "EXTHDRS       =\n\n");
fprintf (fp, "HDRS          =\n\n");
fprintf (fp, "SRCS          =\n\n");
fprintf (fp, "OBJS          =\n\n");
fprintf (fp, "CC            = gcc\n\n");
fprintf (fp, "MAKEFILE      = Makefile\n\n");
if (flg.l) {
	fprintf (fp, "LIBRARY       = lib.a\n\n");
	fprintf (fp, "$(LIBRARY):	$(OBJS)\n");
	fprintf (fp, "		del $(LIBRARY) >nul\n");
	fprintf (fp, "		lib $(LIBRARY) $(OBJS);\n\n");
	fprintf (fp, "install:	$(LIBRARY)\n");
	fprintf (fp, "		copy $(LIBRARY) $(DEST) >nul\n");
}
else {
	fprintf (fp, "PROGRAM       = a.x\n\n");
	fprintf (fp, "$(PROGRAM):	$(OBJS)\n");
	fprintf (fp, "		$(CC) -o $(PROGRAM) $(OBJS)\n\n");
	fprintf (fp, "install:	$(PROGRAM)\n");
	fprintf (fp, "		copy $(PROGRAM) $(DEST) >nul\n");
}
fprintf (fp, "\n");

	fclose (fp);
	return;
}

void makebakfile (fname)
char *fname;
{
	int i;
	static char bakfname[64], buf[256];
	FILE *fp, *fbak;

	if ((fp = fopen (toyen (fname), "r")) == NULL) return;

	for (i = 0; *(fname + i) != (char)0 && *(fname + i) != '.'; i++)
		*(bakfname + i) = *(fname + i);
	*(bakfname + i) = (char)0;
	strcat (bakfname, ".bak");

	if ((fbak = fopen (toyen (bakfname), "w")) == NULL) {
		printf ("mkmf: cannot open file '%s'\n", bakfname);
		abort_exit ();
	}

	while (fgets (buf, 256, fp) != NULL)
		fputs (buf, fbak);

	fclose (fp);
	fclose (fbak);
	return;
}

int getline (fp, s)
FILE *fp;
char *s;
{
	int c, n;

	for (n = 0; (c = getc(fp)) != 0x0a && c != EOF; n++, *s++ = (char)(c));
	*s = EOT;
	if (c == EOF) return (EOF); else return (n);
}

void con_getline (msg, buf)
char *msg, *buf;
{
	printf ("%s", msg);
	gets (buf);
}

