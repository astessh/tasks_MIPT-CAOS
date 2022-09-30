#
# Created by astesh on 21.09.22.
#
#Реализуйте скрипт для командного интерпретатора bash, который выполняет поиск имен
# /#include-файлов для заданных имен Си-функций, используя в качестве источника данных третий раздел man-страниц Linux.
#
#В переменной окружения MANPATH перечислены директории, разделённые символом : (аналогично переменной PATH).
# В этих директориях располагается каталог man3, устроенный аналогично /usr/share/man/man3. Ответ должен быть составлен
# с использованием этих директорий, использование man и manpath запрещено.


IFS=":"

function filter() {
  grep -E "\#include <" | sed -En 's/.*#include <(.*)>/\1/p'
}

while read line; do
  result="---"
  for path in $MANPATH; do
    path_to_man="$path/man3/$line.3"

    if [ -f "$path_to_man" ]; then
      result=$(cat $path_to_man | filter)
      break
    fi

    if [ -f "$path_to_man.gz" ]; then
      result=$(gunzip -c -d "$path_to_man.gz" | filter)
      break
    fi
  done

  echo $result

done
