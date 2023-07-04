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
