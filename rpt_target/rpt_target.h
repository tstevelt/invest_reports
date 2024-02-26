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
#define		SECTOR
#define		INDUSTRY
#define		HISTORY
#define		PORTFOLIO
#define		CASHBAL
#include	"fileinvest.h"
#include	"invlib.h"

TYPE	int		Debug;
TYPE	int		ReportCount;
TYPE	char	OutFileName[256];
TYPE	FILE	*fpOutput;
TYPE	long	ReportMember;
TYPE	char	MatchDomFor;

#define		CASH				'C'
#define		BOND_AND_CD			'B'
#define		DOMESTIC_EQUITY		'D'
#define		FOREIGN_EQUITY		'F'

typedef struct
{
	char	AllocationType;
	double	Percent;
	double	TargetInvest;
	double	CurrentInvest;
	double	TargetMarket;
	double	CurrentMarket;
} SMRY_RECORD;

#define		MAXRECS		200
TYPE	SMRY_RECORD	SummaryArray[MAXRECS];
TYPE	int		SummaryCount;

typedef struct
{
	char	Broker[12];
	char	AllocationType;
	double	Current;
} BRKR_RECORD;

TYPE	BRKR_RECORD		BrokerArray[MAXRECS];
TYPE	int				BrokerCount;

typedef struct
{
	char	Ticker[12];
	char	Name[32];
	char	Expires[12];
	double	Value;
} BOND_RECORD;

TYPE	BOND_RECORD		BondArray[MAXRECS];
TYPE	int				BondCount;

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
int cmprec ( BRKR_RECORD *a , BRKR_RECORD *b );

/* dorpt.c */
void dorpt ( char *DataFilename );

/* EachBond.c */
int EachBond ( void );

/* EachCash.c */
int EachCash ( void );

/* EachPortfolio.c */
int EachPortfolio ( void );

/* EachStock.c */
int EachStock ( void );

/* getargs.c */
void getargs ( int argc , char *argv []);

/* MapStype.c */
int MapStype ( void );

/* getdata.c */
void getdata ( void );

/* rpt_target.c */
int main ( int argc , char *argv []);
