#!/bin/sh

#      Copyright (C) 2013 Team XBMC
#      http://xbmc.org
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, see
# <http://www.gnu.org/licenses/>.

# Original author: Karlson2k (Evgeny Grin)

cppdataFile="$1"
digitsFileUrl='ftp://ftp.unicode.org/Public/6.3.0/ucd/extracted/DerivedNumericValues.txt'
codeLineTemplate='  m.insert(digitsMapElement(0x\1, \2)); //'
digitsFile='DerivedNumericValues.txt'
sedScript='digitsSedScript.sed'

if [ -z "$cppdataFile" ]; then
  echo 'No output file specified!'
  exit 2
fi
if [ ! -f "$cppdataFile" ]; then
  echo 'File "'"$cppdataFile"'" do not exist!'
  exit 2
fi

gen_expand_sed () {
echo '# Expand codes'
for codes in $@
do
  xcodestart=${codes%..*}
  xcodeend=${codes#*..}
  printf '/%s\.\.%s/ {\nh; ' $xcodestart $xcodeend
  codestart=`printf "%i" 0x$xcodestart` || return 1
  codeend=`printf "%i" 0x$xcodeend` || return 1
  for i in $(seq $codestart $codeend)
  do
     [ "$i" != "$codestart" ] && printf "G; "
     printf 's/%s\.\.%s/%X/\n' $xcodestart $xcodeend $i || return 1
  done
  printf "}\n"  || return 1
done
return 0
}

echo 'Downloading data file...'
wget $digitsFileUrl -O $digitsFile || exit 1

echo 'Extracting series of codes...'
codesToExpand=`grep -o '^[0-9A-F]\{4,5\}\.\.[0-9A-F]\{4,5\}' $digitsFile` || exit 1
cat << _EOF_ > $sedScript || exit 1
# remove full line comments and folowing empty lines
/^#/ { 
s/#.*//
:al
N
s/\n\$//
t al
D
}
_EOF_

echo 'Generating code series replacement script...'
gen_expand_sed $codesToExpand >> $sedScript || exit 1

sourceFilename=$(head -n 1 $digitsFile)
sourceFilename=${sourceFilename#\# }

echo 'Writing .cpp data file prefix...'
cat << _EOF_ >> $cppdataFile || exit 1


// generated by "${0##*/}"
CUtf32Utils::digitsMap CUtf32Utils::digitsMapFiller(void)
{
  digitsMap m;

  // source: $sourceFilename
  // see $digitsFileUrl
_EOF_

echo 'Generating .cpp data file...'
# expand XXXX..XXXX codes, append ".0" to values
sed -f $sedScript $digitsFile | \
  sed -e 's/; \([0-9]\{1,\}\) #/; \1.0 #/' -e 's|; \(-\{0,1\}[0-9]\{1,\}\)/\([0-9]\{1,\}\) #|;  \1.0/\2.0 #|' | \
  sed -e 's@^\([0-9A-F]\{4,5\}\) *; .* ; ; *\([^ ]*\) *#@'"${codeLineTemplate}"'@' >> $cppdataFile || exit 1

echo 'Writing .cpp data file suffix...'
cat << _EOF_ >> $cppdataFile || exit 1
  return m;
}
_EOF_


rm $sedScript
rm $digitsFile
  
exit 0