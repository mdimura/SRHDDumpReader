#!/bin/bash                                   
# -e s///g  -e s///g  -e s///g  -e s///g  -e s///g
CMD="-e s/Ultraalloy/Ультрасплавный/g -e s/Transcendental/Трансцендентальный/g 
-e s/Nanobrolite/Нанобролитовый/g  -e s/Pygamore/Пигамарный/g -e s/Myoplasmic/Миоплазменный/g 
-e s/Upgraded/Апгрейтинговый/g -e s/Fast/Быстрый/g -e s/Conscientious/Честный/g  -e s/Ironclad/Броненосец/g
-e s/Kindhearted/Добрый/g  -e s/Peaceful/Мирный/g -e s/Repair/Ремонтный/g -e s/Invincible/Неуязвимый/g
-e s/Dandy/Молния/g" 
sed $CMD bonus_descriptions.json.en > bonus_descriptions.json
sed $CMD bonus_descriptions_hulls.json.en > bonus_descriptions_hulls.json
