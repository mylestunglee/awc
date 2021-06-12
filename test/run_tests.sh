RED='\033[0;31m'
GREEN='\033[0;32m'
CLEAR='\033[0m' # No Color

for test; do
	if ./$test &> /dev/null; then
		echo -e $GREEN$test: "SUCCESS"$CLEAR
	else
		echo -e $RED$test "FAILURE"$CLEAR
	fi
done
