files_no_main:= $(shell find ./src -name "*.c")
files:= $(files_no_main) proy.c
args:=-lm -larchive

wc:
	find . -name "*.c" -type f -exec wc -l -w {} +

compile: $(files) 
	gcc -o proy $(args) $(files)

compile-debug: $(files)
	gcc -g -o proy $(args) $(files)

run: compile
	./proy

mem-test: compile-debug
	valgrind --leak-check=yes -s ./proy

clean:
	rm -frv ~/.local/share/proy
	rm -frv ~/.config/proy/
	rm -frv ~/proy

clean-tests:
	rm -v ./vgcore.*

check-files:
	tree -fi ~/.local/share/proy | grep / | xargs file
	tree -fi ~/.config/proy | grep / | xargs file

compile-hash-test:
	gcc -g -o hash_test $(args) ./src/hash_map.c ./src/single-linked-list.c ./src/extras.c ./tests/helper/helper.c ./tests/hash_test.c

compile-template-test:
	gcc -g -o tmplt_test $(args) $(files_no_main) ./tests/template_test.c ./tests/helper/helper.c

hash-test: compile-hash-test
	valgrind --leak-check=yes ./hash_test
	rm -v ./hash_test

tmplt-test: compile-template-test
	valgrind --leak-check=yes ./tmplt_test
	rm -v ./tmplt_test

install:
	install ./proy_run ~/.local/bin/proy
	install ./proy ~/.local/bin/proy_no_cd

uninstall:
	rm ~/.local/bin/proy
	rm ~/.local/bin/proy_no_cd
