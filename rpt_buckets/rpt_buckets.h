//     Invest report
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<ctype.h>

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <libgen.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <sys/types.h>

#include	"shslib.h"

#include	"dbylib.h"
#include	"rptlib.h"

#define		MEMBER
#define		STOCK
#define		FUNDAMENTAL
#define		HISTORY
#define		PORTFOLIO
#define		CASHBAL
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		Debug;
TYPE	DATEVAL		dvToday;
TYPE	int		ReportCount;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	long	ReportMember;

typedef struct
{
	short	Bucket;
	char	Broker[32];
	char	Ticker[22];
	char	Name[32];
	char	Type;
	char	DomesticForeign;
	int		Days;
	double	Invest;
	double	Market;
	double	Yield;
	double	Shares;
} RECORD;

#define		MAXRECS		200
TYPE	RECORD	Array[MAXRECS];
TYPE	int		Count;

TYPE	REPORT_OPTIONS	ReportOptions;

/*----------------------------------------------------------
	mysql and dbylib stuff
----------------------------------------------------------*/
TYPE  MYSQL   MySql;
TYPE	DBY_OPTIONS	DbyOptions;
TYPE	char		WhereClause[1024];
TYPE	char		OrderByClause[1024];
#ifdef MAIN
char	*LogFileName = "/var/local/invest.log";
#else
TYPE	char	*LogFileName;
#endif

/*------------------------------
:.,$d
:r ! mkproto -p *.c
------------------------------*/

/* cmprec.c */
int cmprec ( RECORD *a , RECORD *b );

/* dorpt.c */
void dorpt ( char *DataFilename );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* getdata_cashbal.c */
int getdata_cashbal ( void );

/* getdata_portfolio.c */
int getdata_portfolio ( void );

/* putdata.c */
int putdata ( void );

/* rpt_buckets.c */
int main ( int argc , char *argv []);
