#!/bin/bash                                                                                                                
#
 
for r in `seq 0 255`; do
    for g in `seq 0 255`; do
        for b in `seq 0 255`; do

            #echo $r $g $b
            #echo -en "$EINS \033[$FG\033[$BG  $T  \033[0m";
            rgb = rgb:$r/$g/$b
            echo $rgb
            echo -en '\e]4;60;$(rgb)\e\\\e[38;5;60m = $r $g $b\e[m\n'

        done
    done
done

#echo -en '\e]4;60;rgb:ff/ff/ff\e\\\e[38;5;60m = ██ \e[m\n'



