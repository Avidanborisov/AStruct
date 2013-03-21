#!/bin/sh

VALGRIND=$2

t=$1
if $VALGRIND ./$t 2>> tests/tests.log
then
	echo $t PASS
else
	echo "ERROR in test $t" 
	echo "tests.log:"
	tail tests/tests.log

	if [ ! -z "$VALGRIND" ];
	then
		echo "valgrind.log:"
		tail -n 15 tests/valgrind.log
	fi

	exit 1
fi

echo ""
