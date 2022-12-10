/*
 * mkmf.h :
 *                                  Version 1.0 Syuichi Takahashi, 1989.
 */

#define False       0
#define True        1
#define Off         0
#define On          1

#define EOT  ((char)0)
#define Match       1
#define NoMatch     0

#define Found       0
#define NotFound    1
#define Exist       2
#define NewMember   3

#define BufSize   256
#define MaxFiles   64

#define L_C         0 
#define L_ASM       1
#define L_PASCAL    2
#define L_BASIC     3
#define L_FORTRAN   4

#define S_SRC       1
#define S_INC       2
#define S_OBJ       3

struct source_ {
   char *name;
   char *sfx;
   int langtype;
   int incfcnt;
   char *incfn[MaxFiles];
};

struct macro_ {
   char *label;
   char *value;
};

struct flag_ { int c, d, i, l, f, F, s, S, e, D, L; };

/* ARGUMENT.C */
void parse_cflags ( char * );
void parse_envarg ( void );
void parse_arg ( int, char ** );
void setdefname ( void );
void put_usage ( void );
int check_equalchar ( char * );
void savemacro ( char * );
int checkmacro ( char * );
void version ( void );

/* DIR.C */
int getdir ( char ** );
int checkdir ( char * );

/* FILE.C */
FILE *opennewmakefile ( void );
FILE *opentemplate ( void );
void maketemplate ( void );
void makebakfile ( char * );
int getline ( FILE *, char * );
void con_getline ( char *, char * );

/* INCLUDE.C */
void scan_ASMinc ( int, char * );
void scan_Cinc ( int, char * );

/* MKMF.C */
void main ( int, char** );
int strnextlen ( char * );
int check_contchar ( char * );
void scan_srcfn ( void );
void update_srclist ( void );
void get_argsfn ( char * );
void get_dirsfn ( void );
void prompt ( void );
void put_depinf ( FILE * );
void put_respfile ( void );
void abort_exit ( void );
void CTRL_C_abort ( void );

/* MLIB.C */
char *changesepar ( char * );
char *toyen ( char * );
char *alloccopy ( char * );
char *allocncopy ( char *, int );
char *skipsp ( char * );
char *tolowers ( char * );
char *touppers ( char * );
char *gettemplpath ( void );
void getincpath ( char ** );
void gettmpfn ( char * );

/* SUFFIX.C */
void remake_suffix ( char * );
int get_srcftype ( char * );
char *get_objname ( char * );

