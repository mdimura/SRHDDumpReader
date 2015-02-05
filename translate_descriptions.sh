#!/bin/bash

#hulls, bonuses
CMD=""
while read ln; do
  NAMEEN=`echo "$ln" | awk -F$'\t' '{print $1}'`
  NAMERU=`echo "$ln" | awk -F$'\t' '{print $2}'`
  CMD=$CMD" -e "\''s/'$NAMEEN/$NAMERU'/g'\'
done < en-ru.txt
eval sed $CMD bonus_descriptions.json.en > bonus_descriptions.json.ru
eval sed $CMD bonus_descriptions_hulls.json.en > bonus_descriptions_hulls.json.ru



#micromodules
CMD=""
while read ln; do
  NAMERU=`echo "$ln" | awk -F$'\t' '{print $1}'`
  NAMEEN=`echo "$ln" | awk -F$'\t' '{print $2}'`
  CMD=$CMD" -e "\''s/'$NAMERU/$NAMEEN'/g'\'
done < micromodules_ru-en.txt
eval sed $CMD  bonus_descriptions_micromodules.json.ru > bonus_descriptions_micromodules.json.en

#fuse en and ru
for f in bonus_descriptions.json bonus_descriptions_hulls.json bonus_descriptions_micromodules.json; do
  echo $f
  head -n -1 ${f}.ru > ${f}
  echo "," >> ${f}
  tail -n +2 ${f}.en >> ${f}
done

rm bonus_descriptions.json.ru bonus_descriptions_hulls.json.ru bonus_descriptions_micromodules.json.en
#head -n -1 bonus_descriptions_micromodules_ru.json > bonus_descriptions_micromodules.json
#echo "," >> bonus_descriptions_micromodules.json
#tail -n +2 bonus_descriptions_micromodules_en.json >> bonus_descriptions_micromodules.json