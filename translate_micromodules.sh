#!/bin/bash

while read ln; do
  NAMERU=`echo "$ln" | awk -F$'\t' '{print $1}'`
  NAMEEN=`echo "$ln" | awk -F$'\t' '{print $2}'`
  CMD=$CMD" -e "\''s/'$NAMERU/$NAMEEN'/g'\'
done < micromodules_ru-en.txt
eval sed $CMD  bonus_descriptions_micromodules_ru.json > bonus_descriptions_micromodules_en.json

head -n -1 bonus_descriptions_micromodules_ru.json > bonus_descriptions_micromodules.json
echo "," >> bonus_descriptions_micromodules.json
tail -n +2 bonus_descriptions_micromodules_en.json >> bonus_descriptions_micromodules.json