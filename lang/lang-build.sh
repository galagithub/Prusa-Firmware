#!/bin/bash
#
# lang-build.sh - multi-language support script
#  generate lang_xx.bin (language binary file)
#
# Input files:
#  lang_en.txt or lang_en_xx.txt
#
# Output files:
#  lang_xx.bin
#
# Temporary files:
#  lang_xx.tmp
#  lang_xx.dat
#
# Config:
#startup message
echo "lang-build.sh started" >&2

if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi

if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "lang-build languages:$LANGUAGES" >&2

#awk code to format ui16 variables for dd
awk_ui16='{ h=int($1/256); printf("\\x%02x\\x%02x\n", int($1-256*h), h); }'


#exiting function
finish()
{
 if [ $1 -eq 0 ]; then
  echo "lang-build.sh finished with success" >&2
 else
  echo "lang-build.sh finished with errors!" >&2
 fi
 exit $1
}

#returns hexadecial data for lang code
lang_code_hex_data()
# $1 - language code ('en', 'cz'...)
{
 case "$1" in
  *en*) echo '\x6e\x65' ;;
  *cz*) echo '\x73\x63' ;;
  *de*) echo '\x65\x64' ;;
  *es*) echo '\x73\x65' ;;
  *fr*) echo '\x72\x66' ;;
  *it*) echo '\x74\x69' ;;
  *pl*) echo '\x6c\x70' ;;
#Community language support
#Dutch
  *nl*) echo '\x6c\x6e' ;;
#Swedish
  *sv*) echo '\x76\x73' ;;
#Danish
  *da*) echo '\x61\x64' ;;
#Slovanian
  *sl*) echo '\x6c\x73' ;;
#Hungarian
  *hu*) echo '\x75\x68' ;;
#Luxembourgish
  *lb*) echo '\x62\x6c' ;;
#Croatian
  *hr*) echo '\x72\x68' ;;
#Lithuanian
  *lt*) echo '\x74\x6c' ;;
#Romanian
  *ro*) echo '\x6f\x72' ;;
#Use the 2 lines below as a template and replace 'qr' and `\x71\x72`
##New language
#  *qr*) echo '\x71\x72' ;;
 esac
 echo '??'
}

write_header()
# $1 - lang
# $2 - size
# $3 - count
# $4 - checksum
# $5 - signature
{
 /bin/echo -n -e "\xa5\x5a\xb4\x4b" |\
  dd of=lang_$1.bin bs=1 count=4 seek=0 conv=notrunc 2>/dev/null
 /bin/echo -n -e $(echo -n "$(($2))" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=4 conv=notrunc 2>/dev/null
 /bin/echo -n -e $(echo -n "$(($3))" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=6 conv=notrunc 2>/dev/null
 /bin/echo -n -e $(echo -n "$(($4))" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=8 conv=notrunc 2>/dev/null
 /bin/echo -n -e "$(lang_code_hex_data $1)" |\
  dd of=lang_$1.bin bs=1 count=2 seek=10 conv=notrunc 2>/dev/null
 sig_h=$(($5 / 65536))
 /bin/echo -n -e $(echo -n "$sig_h" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=14 conv=notrunc 2>/dev/null
 sig_l=$(($5 - $sig_h * 65536))
 /bin/echo -n -e $(echo -n "$sig_l" | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=12 conv=notrunc 2>/dev/null
}

generate_binary()
# $1 - language code ('en', 'cz'...)
{
 echo "lang="$1 >&2
 #remove output and temporary files
 rm -f lang_$1.bin
 rm -f lang_$1.tmp
 rm -f lang_$1.dat
 LNG=$1
 #check lang dictionary
 ./lang-check.py $1 #--no-warning
 #create lang_xx.tmp - different processing for 'en' language
 if [ "$1" = "en" ]; then
  #remove comments and empty lines
  cat lang_en.txt | sed '/^$/d;/^#/d'
 else
  #remove comments and empty lines, print lines with translated text only
  cat lang_en_$1.txt | sed '/^$/d;/^#/d' | sed -n 'n;p'
 fi | sed 's/^\"\\x00\"$/\"\"/' > lang_$1.tmp
 #create lang_xx.dat (binary text data file)
# cat lang_$1.tmp | sed 's/^\"/\/bin\/echo -e \"/;s/"$/\\x00\"/' > lang_$1.shx
 cat lang_$1.tmp | sed 's/^\"/\/bin\/echo -e -n \"/;s/"$/\\x00\"/' | sh >lang_$1.dat
 #calculate number of strings
 count=$(grep -c '^"' lang_$1.tmp)
 echo "count="$count >&2
 #calculate text data offset
 offs=$((16 + 2 * $count))
 echo "offs="$offs >&2
 #calculate text data size
 size=$(($offs + $(wc -c lang_$1.dat | cut -f1 -d' ')))
 echo "size="$size >&2
 #write header with empty signature and checksum
 write_header $1 $size $count 0x0000 0x00000000
 #write offset table
 offs_hex=$(cat lang_$1.tmp | sed 's/^\"//;s/\"$//' |\
  sed 's/\\x[0-9a-f][0-9a-f]/\./g;s/\\[0-7][0-7][0-7]/\./g;s/\ /\./g' |\
  awk 'BEGIN { o='$offs';} { h=int(o/256); printf("\\x%02x\\x%02x",int(o-256*h), h); o+=(length($0)+1); }')
 /bin/echo -n -e "$offs_hex" | dd of=./lang_$1.bin bs=1 seek=16 conv=notrunc 2>/dev/null
 #write binary text data
 dd if=./lang_$1.dat of=./lang_$1.bin bs=1 seek=$offs conv=notrunc 2>/dev/null
 #write signature
 if [ "$1" != "en" ]; then
  dd if=lang_en.bin of=lang_$1.bin bs=1 count=4 skip=6 seek=12 conv=notrunc 2>/dev/null
 fi
 #calculate and update checksum
 chsum=$(cat lang_$1.bin | xxd | cut -c11-49 | tr ' ' "\n" | sed '/^$/d' | awk 'BEGIN { sum = 0; } { sum += strtonum("0x"$1); if (sum > 0xffff) sum -= 0x10000; } END { printf("%x\n", sum); }')
 /bin/echo -n -e $(echo -n $((0x$chsum)) | awk "$awk_ui16") |\
  dd of=lang_$1.bin bs=1 count=2 seek=8 conv=notrunc 2>/dev/null
}

if [ -z "$1" ]; then set 'all'; fi

if [ "$1" = "all" ]; then
 generate_binary 'en'
 for lang in $LANGUAGES; do
  echo " Running : $lang" >&2
  generate_binary $lang
 done
else
 generate_binary $1
fi

finish 0
