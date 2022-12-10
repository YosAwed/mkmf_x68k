/*
 *  suffix.c
 *
 */
#include	<stdio.h>
#include	<string.h>
#include	"mkmf.h"

/*
 * Suffix table
 *                       C      ASM   FORTRAN PASCAL  BASIC
 */
#define MaxSuffixCount 2
char *Suffix_Src[] = { ".c",   ".s", ".for", ".pas", ".bas" };
char *Suffix_Inc[] = { ".h",   ".mac", ".for", ".pas", ".bas" };
char *Suffix_Obj[] = { ".o", ".o", ".o", ".o", ".o" };

static void save_sfx (char **, char *, int);
static void seper_fname (char *, char *, char *);
static void reset_allsfx (char *);

void remake_suffix (s)
char *s;
{
	char *sfx;

	for (s = skipsp(s); *s != EOT; s++) {
		if (*s == '.') sfx = s;
		if (*s == ':') {
			*s++ = EOT;
			switch (*s) {
				case 'o':
					s++;
					save_sfx (Suffix_Obj, sfx, *s);
					break;
				case 's':
					s++;
					save_sfx (Suffix_Src, sfx, *s);
					break;
				case 'h':
					s++;
					save_sfx (Suffix_Inc, sfx, *s);
					break;
				default:
					if (*s == EOT || *s == ' ')
						reset_allsfx (sfx);
					break;
			}
		}
	}
	return;
}

static void reset_allsfx (sfx)
char *sfx;
{
	int i;

	for (i = 0; i < MaxSuffixCount; i++) {
		if (!strcmp (Suffix_Obj[i], sfx)) *Suffix_Obj[i] = EOT;
		if (!strcmp (Suffix_Src[i], sfx)) *Suffix_Src[i] = EOT;
		if (!strcmp (Suffix_Inc[i], sfx)) *Suffix_Inc[i] = EOT;
	}
	return;
}


static void save_sfx (sfxs, sfx, lang)
char *sfxs[], *sfx;
int lang;
{
	int i;

	switch (lang) {
		case 'C': sfxs[L_C] = sfx; break;
		case 'A': sfxs[L_ASM] = sfx; break;
/*
		case 'F': sfxs[L_FORTRAN] = sfx; break;
		case 'P': sfxs[L_PASCAL] = sfx; break;
		case 'B': sfxs[L_BASIC] = sfx; break;
*/
		default:
			if (lang == EOT || lang == ' ')
				for (i = 0; i < MaxSuffixCount; i++)
					sfxs[i] = sfx;
			break;
	}
	return;
}

int get_srcftype (fn)
char *fn;
{
	int i;

	for (; *fn != EOT && *fn != '.'; fn++);
	for (i = 0; i < MaxSuffixCount; i++)
		if (!strcmp (fn, Suffix_Src[i])) return (i);
	return (-1);
}

char *get_objname (path)
char *path;
{
	int i;
	static char objfname[64], sfx[64];

	seper_fname (path, objfname, sfx);
	for (i = 0; i < MaxSuffixCount; i++)
		if (!strcmp (sfx, Suffix_Src[i]))
	strcat (objfname, Suffix_Obj[i]);

	return (objfname);
}

static void seper_fname (path, fname, sfx)
char *path, *fname, *sfx;
{
	strcpy (fname, path);
	for (; *fname != EOT && *fname != '.'; fname++);
	*fname = EOT;
	for (; *path != EOT && *path != '.'; path++);
	strcpy (sfx, path);
	return;
}

