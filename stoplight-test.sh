# Runs sys161 kernel with a timeout, then evaluates the created file.
# This makes it straightforward to test the stoplight problem, without needing
# to manually tee the output of sys161 or manually run the stoplight-check
# program.
#
# Still, feel free to manually run stoplight-check, e.g. on a saved output.txt
# file, to get the errors back from it. (run ./stoplight-check help to get
# help).

OPT_COLOR=
# NOTE: If you find the bold / colors too hard to read, please feel free to
# uncomment the below line to disable them.
#OPT_COLOR="--no-color"

usage() {
    echo "usage: $0 <name of test>"
    echo "available tests:"
    echo "  sl_simple          : 30 cars each turning right from A"
    echo "  sl_random          : 100 vehicles with randomized turn direction, route, and type"
    echo "  sl_prio            : 15 trucks then 15 ambulances, each turning right from A"
    echo "  sl_custom <number> : your custom tests (must provide a number)"
}

TESTCASE="$*"
if [[ "x" == "x$TESTCASE" ]]; then
    usage
    exit 1
fi

if [[ "sl_custom" == "$1" ]]; then
    case "$2" in
        ''|*[!0-9]*) usage; exit 1;;
        *) ;;
    esac
else
    case "$1" in
        sl_simple) ;;
        sl_random) ;;
        sl_prio) ;;
        *) usage; exit 1;;
    esac
fi

# Make sure that the kernel is up to date.
if ! make ; then
    echo "make failed -- kernel does not compile -- refusing to run test"
    exit 1
fi

# Cd to the 'root' directory so that we can run the kernel.
cd root
# Execute the kernel using stoplight-timeout program for detecting deadlocks
# and creating the output.txt file.
../stoplight-timeout -o output.txt -t 3 sys161 kernel "$TESTCASE"
# Run the stoplight-check program to get error info about the output.
# stoplight-checkc will print a small number of errors to the user, plus every
# error to the output-errors.txt file.
../stoplight-check $OPT_COLOR -o output-errors.txt output.txt
# Fix any problems from sys161
stty sane