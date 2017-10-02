CXXFLAGS:=-std=c++11

check: tests/test_toollock
	tests/test_toollock

docs:
	./node_modules/.bin/coffee doc/states.coffee > doc/states.json
	./node_modules/.bin/finito dot doc/states.json > doc/states.dot
	dot -Tpng doc/states.dot -o doc/states.png

.PHONY: docs check
