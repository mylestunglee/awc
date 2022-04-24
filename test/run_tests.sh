RED='\033[0;31m'
GREEN='\033[0;32m'
CLEAR='\033[0m'

for test; do
	if ./$test &> /dev/null; then
		echo -e $test: $GREEN"SUCCESS"$CLEAR
	else
		echo -e $test $RED"FAILURE"$CLEAR
	fi
done
