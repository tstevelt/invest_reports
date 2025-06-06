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


#define		INCLUDE_AD_PERCENT
#undef		INCLUDE_AD_PERCENT

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
#define		AVERAGE
#define		PORTHIST
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		Debug;
TYPE	int		ReportStyle;
TYPE	int		ReportCount;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	long	ReportMember;

#define		MODE_REPORT		'R'
TYPE	int		RunMode;
TYPE	char	OneTicker[22];

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

/* dorpt.c */
void dorpt ( char *DataFilename );

/* dorpt_fundamental.c */
void dorpt_fundamental ( char *DataFilename );

/* getdata.c */
int getdata ( void );

/* sortdata.c */
void sortdata ( void );

/* EachFundamental.c */
int EachFundamental ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* rpt_porthist.c */
int main ( int argc , char *argv []);
