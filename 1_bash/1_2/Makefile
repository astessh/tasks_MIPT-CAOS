#
# Created by astesh on 25.09.22.
#


EXE_NAME = program.exe
CONFIG = compile.conf
SOURCE = solution.c

NUMS = $(shell grep -E -v '\#' $(CONFIG) | sed -E -n 's/.*=(.*)/+\1/p' | grep -E '+[0-9]')
STRS = $(shell grep -E -v '\#' $(CONFIG) | sed -E -n 's/.*=(.*)/\#\1/p' | grep -E -v '\#[0-9]')

all:
	$(shell sed -i 's/strs="";/strs="$(STRS)\t";/' $(SOURCE))
	$(shell sed -i 's/nums=0;/nums=$(NUMS);/' $(SOURCE))
	gcc -o $(EXE_NAME) $(SOURCE)
clean:
	rm -f $(EXE_NAME) 
