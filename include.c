/*
 * include.c
 *
 */
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	"mkmf.h"

extern char *extincpath[];
extern struct flag_ flg;

extern int lochc;
extern char *lochdrs[];

extern int exthc; 
extern char *exthdrs[];

extern int srcfcnt; 
extern struct source_ srcfn[];

extern int Dcnt;
extern char *Dtag[];

static void cut_Cremarks (char *, char *, int *);
static void cut_ASMremarks (char *, char *);
static int check_inctxt (char *, char **, char *);
static int save_fname (int, int, char **, char *);
static int comp_word (char *, char *);
static void extract_word (char *, char *);
static char *skip_word (char *);
static int istoken (int);
static char *check_extinc (char *);
static void parse_ifdefmac (char *, int *, int *);
static int tagcomp (char *, char *);

static char *syntax_Cusrinc[] = { "#include", "\"", "*", "\"", "" };
static char *syntax_Csysinc[] = { "#include", "<", "*", ">", "" };
static char *syntax_ASMinc[]  = { "include", "*", "" };

void scan_Cinc (idx, fname)
int idx;
char *fname;
{
	FILE *fp, *fopen ();

	int i;
	int ifdef_stackcnt, ifdef_flg[16];
	int lines, remark_flg;
	char buf[256], buf2[256], *extpath, incname[64];

	ifdef_stackcnt = 0;
	for (i = 0; i < 16; i++) ifdef_flg[i] = On;

	remark_flg = Off;
	if ((fp = fopen (fname, "r")) == NULL) return;

	for (lines = 1; getline (fp, buf) != EOF; lines++) {
		cut_Cremarks (buf2, buf, &remark_flg);
		if (flg.D) parse_ifdefmac (buf2, &ifdef_stackcnt, ifdef_flg);
		if (!flg.s && check_inctxt (buf2, syntax_Csysinc, incname) == True) {
			if ((extpath = check_extinc (incname)) != (char *)0) {
				scan_Cinc (idx, extpath);
				exthc = save_fname (idx, exthc, exthdrs, extpath);
			}
			else
				printf ("mkmf: \"%s\", line %d: can't find <%s>\n", fname, lines, incname);
		}
		if (check_inctxt (buf2, syntax_Cusrinc, incname) == True) {
			lochc = save_fname (idx, lochc, lochdrs, incname);
			if (checkdir (incname) == True)
				scan_Cinc (idx, incname);
			else
				printf ("mkmf: \"%s\", line %d: can't find \"%s\"\n", fname, lines, incname);
		}
	}
	fclose (fp);
	return;
}

void scan_ASMinc (idx, fname)
int idx;
char *fname;
{
	FILE *fp, *fopen ();
	int lines;
	char buf[256], buf2[256], incname[64];

 	if ((fp = fopen (fname, "r")) == NULL) return;

	for (lines = 1; getline (fp, buf) != EOF; lines++) {
		cut_ASMremarks (buf2, buf);
		if (check_inctxt (touppers (buf2), syntax_ASMinc, incname) == True) {
			lochc = save_fname (idx, lochc, lochdrs, incname);
			if (checkdir (incname) == True)
				scan_ASMinc (idx, incname);
			else
				printf ("mkmf: \"%s\", line %d: can't find %s\n", fname, lines, incname);
		}
	}
	fclose (fp);
	return;
}

static void cut_Cremarks (d, s, f)
char *d, *s;
int *f;
{
	while (*s != EOT) {
		if (!strncmp (s, "/*", 2)) {
			*f = On;
			s += 2;
		}
		else
      		if (!strncmp (s, "*/", 2)) {
			*f = Off;
			s += 2;
		}
		else
		if (*f == Off)
			*d++ = *s++;
		else
			s++;
	}
	*d = EOT;
	return;
}

static void cut_ASMremarks (d, s)
char *d, *s;
{
	for (; *s != ';' && *s != EOT; *d++ = *s++);
	*d = EOT;
	return;
}

static int save_fname (idx, n, fns, fn)
int idx, n;
char *fns[], *fn;
{
	int i, j;

	if ( fn ) {
		for (i = 0; i < n && fns[i] && strcmp (fns[i], fn); i++)
			;
	} else
		i=0;
	if (i == n)
		fns[n++] = alloccopy (fn);

	for (j = 0; srcfn[idx].incfn[j] && strcmp (srcfn[idx].incfn[j], fn)
							&& j < srcfn[idx].incfcnt; j++)
		;
	if (j == srcfn[idx].incfcnt) {
		srcfn[idx].incfn[srcfn[idx].incfcnt] = fns[i];
		srcfn[idx].incfcnt++;
	}
	return (n);
}

static int check_inctxt (s1, s2, fname)
char *s1, **s2, *fname;
{
	for (s1 = skipsp (s1); **s2 != EOT; s1 = skip_word (s1), s2++) {
		if (**s2 == '*')
			extract_word (fname, s1);
		else
			if (comp_word (s1, *s2) == NoMatch)
				return (NoMatch);
	}
	return (Match);
}

static int comp_word (s1, s2)
char *s1, *s2;
{
	if (istoken (*s1) && *s1 == *s2) return (Match);

	for (;; s1++, s2++) {
		if ((istoken (*s1) || isspace ((int)*s1)) && *s2 == EOT)
			return (Match);
		if (*s1 != *s2) break;
	}
	return (NoMatch);
}

static void extract_word (word, s)
char *word, *s;
{
	while (!istoken(*s) && !isspace ((int)*s) && *s != EOT)
		*word++ = *s++;
	*word = EOT;
	return;
}

static char *skip_word (s)
char *s;
{
	if (istoken(*s))
		return (skipsp(++s));
	else
		for (; !istoken (*s) && !isspace ((int)(*s)); s++);

	return (skipsp (s));
}

static int istoken (c)
int c;
{
	static char Token[] = "<>()\"\'";
	int i;

	for (i = 0; *(Token + i) != EOT; i++)
		if (c == *(Token + i)) return (True);
	return (False);
}

static char *check_extinc(fn)
char *fn;
{
	int i;
	static char path[64];

	for (i = 0; extincpath[i] != (char *)0; i++) {
		sprintf (path, "%s\\%s", extincpath[i], fn);
		if (checkdir (path) == True)
			return (path);
	}
	return ((char *)0);
}

static void parse_ifdefmac (buf, n, flg)
char *buf;
int *n, flg[];
{
	int i;

	buf = skipsp (buf);
	if (tagcomp (buf, "#ifdef") || tagcomp (buf, "#if")) {
		flg[++*n] = Off;
		for (i = 0; i < Dcnt; i++)
			if (tagcomp (skipsp (buf + 6), Dtag[i]))
				flg[*n] = On;
	}
	if (!flg[*n] && tagcomp (buf, "#elif")) {
		flg[*n] = Off;
		for (i = 0; i < Dcnt; i++)
			if (tagcomp (skipsp (buf + 6), Dtag[i]))
				flg[*n] = On;
	}
	if (tagcomp (buf, "#ifndef")) {
		flg[++*n] = On;
		for (i = 0; i < Dcnt; i++)
			if (tagcomp (skipsp (buf + 6), Dtag[i]))
				flg[*n] = Off;
	}
	if (tagcomp (buf, "#else")) {
		if (flg[*n] == On)
			flg[*n] = Off;
		else
			flg[*n] = On;
	}
	if (tagcomp (buf, "#endif")) flg[(*n)--] = On;

	if (flg[*n] == Off) strcpy (buf, "");

	return;
}

static int tagcomp (s1, s2)
char *s1, *s2;
{
	for (;; s1++, s2++) {
		if ((isspace ((int)*s1) || *s1 == EOT) && *s2 == EOT)
			return (True);
		if (*s1 != *s2 || *s2 == EOT) break;
	}
	return (False);
}

