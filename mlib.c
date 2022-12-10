/*
 * mlib.c
 *
 */
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
/* #include	<malloc.h> */
#include	"mkmf.h"

extern char *extincpath[];
extern char *mkfname, *tmplfname;
extern struct flag_ flg;

char *getenv ( char * );

char *changesepar ( str )
char *str;
{
	char *p;

	if (flg.e) {
		for (p = str; *str != EOT; str++)
			if (*str == '/') *str = '\\';
	}
	else {
		for (p = str; *str != EOT; str++)
			if (*str == '\\') *str = '/';
	}
	return (p);
}

char *toyen ( s )
char *s;
{
	char *p;

	for (p = s; *s != EOT; s++)
		if (*s == '/') *s = '\\';

	return (p);
}

char *alloccopy ( src )
char *src;
{
	char *dst;

	dst = (char *)malloc (strlen (src) + 1);
	strcpy (dst, src);
	return (dst);
}

char *allocncopy ( src, n )
char *src;
int n;
{
	char *dst;

	dst = (char *)malloc (n + 1);
	strncpy (dst, src, n);
	dst[n] = EOT;
	return (dst);
}

char *skipsp ( str )
char *str;
{
	for (; isspace ((int)*str); str++);
	return (str);
}

char *tolowers ( str )
char *str;
{
	char *p;

	for (p = str; *str != EOT; str++)
		if (isupper ((int)*str))
			*str = (char)tolower ((int)*str);
	return (p);
}

char *touppers ( str )
char *str;
{
	char *p;

	for (p = str; *str != EOT; str++)
		if (islower ((int)*str))
			*str = (char)toupper ((int)*str);
	return (p);
}

char *gettemplpath ( void )
{
	int i;
	static char *path, fn[64], *tmppath;
	static char *LIBS[] = {"usrlib", "USRLIB", "lib", "LIB", "makepath", "MAKEPATH", ""};

	for (i = 0; *LIBS[i] != EOT; i++) {
		if ((path = getenv (LIBS[i])) != NULL) {
			if ((tmppath = strtok (path, ";")) != NULL) {
				do {	sprintf(fn,"%s\\%s",tmppath,tmplfname);
					if (checkdir (fn)) return (fn);
				} while ((tmppath = strtok(NULL,";")) != NULL);
			}
		}
	}
	sprintf (fn, "%s", tmplfname);
	return (fn);
}

void getincpath ( incpath )
char **incpath;
{
	int n;
	char *path;

	n = 0;
	if ( (path=getenv("include"))==NULL && (path=getenv("INCLUDE"))==NULL )
		incpath[n++] = alloccopy (".");
	else {
		incpath[n++] = path;
		for (; *path != EOT; path++) {
			if (*path == ';') {
				*path = EOT;
				incpath[n++] = path + 1;
			}
		}
	}
	incpath[n] = (char *)0;

	return;
}

void gettmpfn ( fname )
char *fname;
{
	char *tmppath, *p;

	if ( (tmppath=getenv("TEMP"))==NULL || (tmppath=getenv("temp"))==NULL || 
		(tmppath=getenv("tmp"))==NULL || (tmppath=getenv("TMP"))==NULL )
		tmppath = "";
/* 	p = ((tmppath = getenv("TMP")) !=NULL) ? tmppath : (tmppath = ""); */
	for (p += strlen(tmppath) - 1; !isspace ((int)*p) && p != tmppath; p--)
		if (*p == '\\' || *p == '/') *p = EOT;
	sprintf (fname, "%s\\mkmf.$$$", tmppath);
	return;
}

