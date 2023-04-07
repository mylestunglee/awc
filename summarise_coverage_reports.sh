print_coverage() {
    if [ -f "$2" ]
    then
        PERCENTAGE=$(xmllint --html "$2" --xpath "/html/body/table/tr/td/table/tr/td[@class='headerCovTableEntryHi' or @class='headerCovTableEntryLo']/text()" | head -n 1)
        echo "$1" "${PERCENTAGE% %}"
    fi
}

print_directory_coverage() {
    FILE=${1%_test_coverage}
    REPORT="$1"/report/src/index.html
    print_coverage "$FILE" "$REPORT"
}

print_directory_coverage all_test_coverage
print_directory_coverage command_test_coverage

for DIRECTORY in *_test_coverage
do
    FILE=${DIRECTORY%_test_coverage}
    REPORT=$(find "$DIRECTORY"/report -name "${FILE%_fake}".c.gcov.html)
    print_coverage "$FILE" "$REPORT"
done
