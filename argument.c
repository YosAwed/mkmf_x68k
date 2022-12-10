/*
 * argument.c
 *
 */
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	"mkmf.h"

extern char *mkfname, *tmplfname, *linkfname;
extern struct flag_ flg;
extern int maccnt;
extern struct macro_ macro[];
extern int Dcnt, Slen;
extern char *Dtag[];

static char wkbuf[128];

char *getenv ( char * );
static char *extr_Slen ( char * );

void parse_cflags (cflags)
char *cflags;
{
	int i;
	int argc;
	static char *argv[16], *s, str[64];

	strcpy ((s = str), cflags);
	for (i = 0; *s != EOT; s++) {
		if (!strncmp (s, "-D", 2)) {
			argc = 1;
			argv[argc++] = s;
			for (; *s != EOT; s++) {
				if (*s == ' ') {
					*s = EOT;
					argv[argc++] = s + 1;
				}
			}
		 	parse_arg (argc, argv);
		}
	}
	return;
}

void parse_envarg ( void )
{
	int argc;
	static char *argv[16], *envarg;

	if ( (envarg=getenv("mkmf"))!=NULL || (envarg=getenv("MKMF"))!=NULL ) {
		argc = 1;
		argv[argc++] = envarg;
		for (; *envarg != EOT; envarg++) {
			if (*envarg == ';' || *envarg == ' ') {
				*envarg = EOT;
				argv[argc++] = envarg + 1;
			}
		}
	 	parse_arg (argc, argv);
	}
	return;
}

void parse_arg (argc, argv)
int argc;
char *argv[];
{
	int i;

	for (i = 1; i < argc; i++) {
		if (!strcmp (argv[i], "-f")) {
			if (++i < argc) {
				mkfname = alloccopy (argv[i]);
				flg.f = On;
			}
		}
		else
		if (!strcmp (argv[i], "-F")) {
			if (++i < argc) {
				tmplfname = alloccopy (argv[i]);
				flg.F = On;
			}
		}
		else
		if (!strcmp (argv[i], "-L")) {
			flg.L = On;
			if (++i < argc) {
				linkfname = alloccopy (argv[i]);
			}
		}
		else
		if (*argv[i] == '-') {
		   for (argv[i]++; *argv[i] != EOT; argv[i]++) {
		      switch (*argv[i]) {
		         case 'c': flg.c = On; break;
		         case 'd': flg.d = On; break;
		         case 'i': flg.i = On; break;
		         case 'l': flg.l = On; break;
		         case 's': flg.s = On; break;
		         case 'S': flg.S = On;
			    if (*(argv[i] + 1) != EOT) {
			       argv[i] = extr_Slen (++argv[i]) - 1;
			    }
			    break;
		         case 'e': flg.e = On; break;
		         case 'D': flg.D = On;
		            if (*(argv[i] + 1) != EOT) {
		               Dtag[Dcnt++] = ++argv[i];
		               argv[i] += strlen(argv[i]);
		            }
		            break;
		         case 'v':
		            version ();
		            put_usage ();
		            break;
		         default:
		            printf ("mkmf: bad option -%c\n", *argv[i]);
		            put_usage ();
		            break;
		      }
		      if (flg.D == On) {
		         for (i++; i < argc; Dcnt++, i++) {
		            Dtag[Dcnt] = argv[i];
		         }
		         break;
		      }
		   }
		}
		else
			if (check_equalchar (argv[i]))
				savemacro (argv[i]);
		else {
			printf ("mkmf: %s not a macro definition\n", argv[i]);
			put_usage ();
		}
	}
	return;
}

static char *extr_Slen (s)
char *s;
{
	int i, j;
	static int A[10] = { 1, 10, 100, 1000, 10000 };

	for (i = 0; isdigit((int)s[i]) && s[i] != EOT; i++);
	for (j = Slen = 0; j < i; j++)
		Slen += (s[j] - '0') * A[i - j - 1];
	return (s + i);
}

void setdefname ( void )
{
	if (tmplfname == (char *)0) {
		if (flg.l)
			tmplfname = alloccopy ("makefile.l"); 
		else
			tmplfname = alloccopy ("makefile.p"); 
	}

	if (mkfname == (char *)0)
		mkfname = alloccopy ("Makefile");
	else {
		sprintf (wkbuf, "MAKEFILE      = %s", mkfname);
		savemacro (wkbuf);
	}
	if (linkfname == (char *)0)
		linkfname = alloccopy ("linkfile");
	else {
		sprintf (wkbuf, "LINKFILE      = %s", linkfname);
		savemacro (wkbuf);
	}
	return;
}

void put_usage ( void )
{
	printf ("usage: mkmf [-cdilsSeDv][-f makefile][-F template][-L link-responsfile]\n");
	printf ("            [macroname=value...]\n");
	abort_exit ();
}

int check_equalchar(s)
char *s;
{
	for (; *s != EOT; s++)
		if (*s == '=') return (True);
	return (False);
}

void savemacro (s)
char *s;
{
	int i, n;

	for (n = 0; s[n] != EOT && s[n] != '=' && !isspace ((int)s[n]); n++);
	for (i = 0; i < maccnt && strncmp (s, macro[i].label, n); i++);
	if (i == maccnt) maccnt++;
	macro[i].label = allocncopy (s, n);

	for (; *s != EOT && *(s - 1) != '='; s++);
	macro[i].value = alloccopy (skipsp(s));
	return;
}

int checkmacro (s)
char *s;
{
	int i, n;

	for (n = 0; s[n] != EOT && s[n] != '=' && !isspace ((int)s[n]); n++);
	for (i = 0; i < maccnt && strncmp(s, macro[i].label, strlen(macro[i].label)); i++);
	if (i == maccnt) return (-1);

	return (i);
}

void version ( void )
{
	printf ("Mkmf version 1.2a   Dec, 1989 Syuichi Takahashi\n");
	printf ("    X68K Modified   Dec, 1990 Awed(Nankin-net)\n");
}

