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
	echo "Test.sh member"
	exit 1
fi

MEMBER=$1


echo "select Ssector, SCsector, format(sum(Pprice*Pshares),2)" > script
echo "  from stock, sector, portfolio" >> script
echo " where Ssector = SCid " >> script
echo "   and Pticker = Sticker" >> script
echo "   and Pmember = $MEMBER" >> script
echo " group by Ssector " >> script
echo " order by SCsector;" >> script

echo "select 'TOTAL', format(sum(Pprice*Pshares),2)" >> script
echo "  from stock, sector, portfolio" >> script
echo " where Ssector = SCid " >> script
echo "   and Pticker = Sticker" >> script
echo "   and Pmember = $MEMBER;" >> script

mysql < script

echo "select Sindustry, INDname, format(sum(Pprice*Pshares),2)" > script
echo "  from stock, industry, portfolio" >> script
echo " where Sindustry = industry.id " >> script
echo "   and Pticker = Sticker" >> script
echo "   and Pmember = $MEMBER" >> script
echo " group by Sindustry " >> script
echo " order by INDname;" >> script

echo "select 'TOTAL', format(sum(Pprice*Pshares),2)" >> script
echo "  from stock, industry, portfolio" >> script
echo " where Sindustry = industry.id " >> script
echo "   and Pticker = Sticker" >> script
echo "   and Pmember = $MEMBER;" >> script

mysql < script

echo "select Ssector, SCsector, Sindustry, INDname, format(sum(Pprice*Pshares),2)" > script
echo "  from stock, sector, industry, portfolio" >> script
echo " where Sindustry = industry.id " >> script
echo "   and Ssector = SCid " >> script
echo "   and Pticker = Sticker" >> script
echo "   and Pmember = $MEMBER" >> script
echo " group by Ssector, Sindustry " >> script
echo " order by SCsector, INDname;" >> script

echo "select 'TOTAL', format(sum(Pprice*Pshares),2)" >> script
echo "  from stock, sector, industry, portfolio" >> script
echo " where Sindustry = industry.id " >> script
echo "   and Ssector = SCid " >> script
echo "   and Pticker = Sticker" >> script
echo "   and Pmember = $MEMBER;" >> script

mysql < script

