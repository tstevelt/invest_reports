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



# echo "select Arsi from average, stock where Sticker = Aticker and Sdj = 'Y' and Adate = '2023-09-21'" > script

echo "select Arsi from average where Adate = '2023-09-21'" > script

mysql -D invest < script | grep -v '+' | sed 's/[| ]//g' > x1.csv

vi x1.csv

distchart -r 25 -in x1.csv -h


