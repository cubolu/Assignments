################################################################################
# wedi                                                                         #
# Autor:       Jakub Lukac                                                     #
# Login:       xlukac09                                                        #
# Rocnik:      1. rocnik BIT                                                   #
# Datum:       29.03.2015                                                      #
# e-mail:      xlukac09@stud.fit.vutbr.cz                                      #
################################################################################

# overenie nastaveni premenych prostredia: WEDI_RC, EDITOR a VISUAL
if [ -z "$WEDI_RC" ]; then
   echo "Premenna \$WEDI_RC nie je nastavena!" >&2
   exit 1
elif [ -z "$EDITOR" ]; then
   if [ -z "$VISUAL" ]; then
      echo "Premenna \$EDITOR ani premenna \$VISUAL nie su nastavene!" >&2
      exit 1
   else
      MY_EDITOR="$VISUAL"
   fi
else
   MY_EDITOR="$EDITOR"
fi

while getopts :mlb:a: opt; do
   case "$opt" in
      m)
         OPT_ID="m";;
      l)
         OPT_ID="l";;
      b)
         OPT_ID="b"
         if date -d "$OPTARG" >/dev/null 2>&1; then
            MY_DATE=$(date -d "$OPTARG" +"%Y%m%d")
         else
            echo "Format datumu prepinaca -$opt nie je validny!" >&2
            exit 1
         fi;;
      a)
         OPT_ID="a"
         if date -d "$OPTARG" >/dev/null 2>&1; then
            MY_DATE=$(date -d "$OPTARG" +"%Y%m%d")
         else
            echo "Format datumu prepinaca -$opt nie je validny!" >&2
            exit 1
         fi;;
      :)
         echo "Parameter -$OPTARG nebol zadany s hodnotou!" >&2
         exit 1;;
      ?)
         echo "Bol zadany neznamy parameter!" >&2
         exit 1;;
   esac
done
# odstranenie prepinacov
OPTIND=`expr $OPTIND - 1`
shift $OPTIND

# wedi bez argumentov
if [ -z "$1" ]; then
   MY_DIR="`pwd`/"
# wedi s existujucim suborom
elif [ -f "$1" ]; then
   echo "`date +%Y%m%d`:`readlink -f "$1"`" >> "$WEDI_RC"
   $MY_EDITOR "$1"
   exit $?
# wedi s existujucim adresarom
elif [ -d "$1" ]; then
   MY_DIR=`readlink -f "$1"`/
# wedi s neexistujucim suborom/adresarom
else
   if [ "`echo "$1"|head -c1`" = "/" ]; then
      MY_FILE=`pwd`"$1"
   else
      MY_FILE="`pwd`/$1"
   fi
   echo "`date +%Y%m%d`:$MY_FILE" >> "$WEDI_RC"
   "$MY_EDITOR" "$MY_FILE"
   exit $?
fi

# wedi bez prepinaca => spusti sa posledny editovany subor v adresari
if [ -z "$OPT_ID" ]; then
   MY_FILE_NAME=`grep "$MY_DIR[^/]*\$" "$WEDI_RC" | cut -c10- | awk 'system("test -f \"" $0 "\"")==0 { print $0 }' | tail -n 1 | grep -o '[^/]*$'`
   FILE_PATH="$MY_DIR$MY_FILE_NAME"
# wedi -m => spusti sa najcastejsie editovany subor
elif [ "$OPT_ID" = "m" ]; then
   MY_FILE_NAME=`grep "$MY_DIR[^/]*\$" "$WEDI_RC" | cut -c10- | awk 'system("test -f \"" $0 "\"")==0 { print $0 }' | sort | uniq -c | sort -nr | head -n 1 | grep -o '[^/]*$'`
   FILE_PATH="$MY_DIR$MY_FILE_NAME"
fi

if [ -d "$FILE_PATH" ]; then
   echo "V adresari $FILE_PATH nebol editovany ziaden subor!" >&2
   exit 1
fi

# statisticke fcie
# parameter -l
if [ "$OPT_ID" = "l" ]; then
   grep "$MY_DIR[^/]*\$" "$WEDI_RC" | cut -c10- | awk 'system("test -f \"" $0 "\"")==0 { print $0 }' | grep -o '[^/]*$' | sort | uniq
# parameter -b
elif [ "$OPT_ID" = "b" ]; then
   N_LINE=0
   for d in `grep "$MY_DIR[^/]*\$" "$WEDI_RC" | cut -c1-8 "$WEDI_RC"`
   do
      if [ $d -le $MY_DATE ] ; then
         N_LINE=`expr $N_LINE + 1`
      fi
   done
   sort -n "$WEDI_RC" | head -n $N_LINE | grep -o "$MY_DIR[^/]*\$" | awk 'system("test -f \"" $0 "\"")==0 { print $0 }' | grep -o '[^/]*$' | sort | uniq
# parameter -a
elif [ "$OPT_ID" = "a" ]; then
   N_LINE=0
   for d in `grep "$MY_DIR[^/]*\$" "$WEDI_RC" | cut -c1-8 "$WEDI_RC"`
   do
      if [ $d -ge $MY_DATE ] ; then
         N_LINE=`expr $N_LINE + 1`
      fi
   done
   sort -rn "$WEDI_RC" | head -n $N_LINE | grep -o "$MY_DIR[^/]*\$" | awk 'system("test -f \"" $0 "\"")==0 { print $0 }' | grep -o '[^/]*$' | sort | uniq

# otvorenie a editovanie vybraneho suboru
else
   echo "`date +%Y%m%d`:$FILE_PATH" >> "$WEDI_RC"
   "$MY_EDITOR" "$FILE_PATH"
   exit $?
fi
