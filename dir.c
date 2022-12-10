/*
 *  dir.c :
 *                                  Version 1.1 Syuichi Takahashi, 1989.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<doslib.h>

#define	Match		1
#define	NoMatch		0
#define	True		1
#define	False		0
#define More		1
#define NoMore		0

#define	Attr_arc	0x20
#define	Attr_dir	0x10
#define	Attr_vol	0x08
#define	Attr_sys	0x04
#define	Attr_hdn	0x02	/* ‰B‚µƒtƒ@ƒCƒ‹ */
#define	Attr_rdo	0x01
#define	Attr_all	0x3f

struct FILBUF dp;

getdir (fn)
char *fn[];
{
	int n;
	char *alloccopy ();

	n = 0;
	if ( FILES(&dp,"*.*",Attr_all)==0 ) {
		do {
			fn[n++] = alloccopy (tolowers (dp.name));
		} while ( NFILES(&dp)==0 );
	}
	return(n);
}

checkdir (path)
char *path;
{
	if ( FILES(&dp,path,Attr_all)==0 && !(dp.atr & Attr_hdn))
		return (True);
	return (False);
}

