#!/bin/sh
#     Invest report
# 
#     Copyright (C)  2019 - 2024 Tom Stevelt
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License as
#     published by the Free Software Foundation, either version 3 of the
#     License, or (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.
# 
#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

if [ "$1" = '' ]
then
	echo "USAGE: run.sh now"
	echo "USAGE: run.sh year"
	exit 1
fi

if [ $1 = 'now' ]
then
	NOW=`date +%Y-%m-%d`
	START=`date --date='last Year' +%Y-%m-%d`
	regress $START $NOW W -fmt html -member 501 -d
else
	regress $1-01-01 $1-12-31 W -fmt html -member 501 -d
fi

