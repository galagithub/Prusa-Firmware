#!/bin/bash
#
# lang-add.sh - multi-language support script
#  add new texts from list (lang_add.txt) to all dictionary files
#
# Input files:
#  lang_add.txt
# Updated files:
#  lang_en.txt and all lang_en_xx.txt
#

# Config:
if [ -z "$CONFIG_OK" ]; then eval "$(cat config.sh)"; fi
if [ -z "$CONFIG_OK" ] | [ $CONFIG_OK -eq 0 ]; then echo 'Config NG!' >&2; exit 1; fi

if [ ! -z "$COMMUNITY_LANGUAGES" ]; then
  LANGUAGES+=" $COMMUNITY_LANGUAGES"
fi
echo "fw-clean languages:$LANGUAGES" >&2


# insert single text to english dictionary
# $1 - text to insert
insert_en()
{
	#replace '[' and ']' in string with '\[' and '\]'
	str=$(echo "$1" | sed "s/\[/\\\[/g;s/\]/\\\]/g")
	# extract english texts, merge new text, grep line number
	ln=$((cat lang_en.txt; echo "$1") | sed "/^$/d;/^#/d" | sort | grep -n "$str" | sed "s/:.*//")
	# calculate position for insertion
	ln=$((3*(ln-2)+1))
	# insert new text
	sed -i "$ln"'i\\' lang_en.txt
	sed -i "$ln"'i\'"$1"'\' lang_en.txt
	sed -i "$ln"'i\#\' lang_en.txt
}

# insert single text to translated dictionary
# $1 - text to insert
# $2 - sufix
insert_xx()
{
	#replace '[' and ']' in string with '\[' and '\]'
	str=$(echo "$1" | sed "s/\[/\\\[/g;s/\]/\\\]/g")
	# extract english texts, merge new text, grep line number
	ln=$((cat lang_en_$2.txt; echo "$1") | sed "/^$/d;/^#/d" | sed -n 'p;n' | sort | grep -n "$str" | sed "s/:.*//")
	# calculate position for insertion
	ln=$((4*(ln-2)+1))
	# insert new text
	sed -i "$ln"'i\\' lang_en_$2.txt
	sed -i "$ln"'i\"\x00"\' lang_en_$2.txt
	sed -i "$ln"'i\'"$1"'\' lang_en_$2.txt
	sed -i "$ln"'i\#\' lang_en_$2.txt
}

# check if input file exists
if ! [ -e lang_add.txt ]; then
	echo "file lang_add.txt not found"
	exit 1
fi

cat lang_add.txt | sed 's/^/"/;s/$/"/' | while read new_s; do
	if grep "$new_s" lang_en.txt >/dev/null; then
		echo "text already exist:"
		echo "$new_s"
		echo
	else
		echo "adding text:"
		echo "$new_s"
		echo
		#insert_en "$new_s"
		for lang in $LANGUAGES; do
			insert_xx "$new_s" "$lang"
		done

#Use the 2 lines below as a template and replace 'qr'
##New language
#		insert_xx "$new_s" 'qr'
	fi
done

read -t 5
exit 0
