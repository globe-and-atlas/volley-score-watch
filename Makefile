# VolleyKeeper local commands
.PHONY: test build clean help

test:
	@mkdir -p .tmp
	cc -std=c11 -Wall -Wextra -Werror -Iwatchapp/src/c tests/model_test.c watchapp/src/c/model.c -o .tmp/model_test
	.tmp/model_test

build: test
	python3 execution/build.py

clean:
	cd watchapp && pebble clean

help:
	@echo "make test  - host scoring checks"
	@echo "make build - test and clean Emery PBW"
	@echo "make clean - remove Pebble build output"
