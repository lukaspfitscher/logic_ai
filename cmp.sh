# Clear the terminal screen
clear

# Compile code.c into an executable named app
# The -Werror flag treats all warnings as errors

#-O3 significantly increases speed!!
#-march=native uses znver3: gcc -march=native -Q --help=target


gcc -O3 -std=gnu11 -march=native -Werror -o app \
code.c            \
logic_ai/basics.c \
logic_ai/time.c   \
logic_ai/random.c \
logic_ai/print.c
#clang -O3 -std=c11 -march=native -Werror -o app code.c

# Check if the compilation was successful (no errors or warnings)
# $? holds the exit status of the last command (0 for success)
if [ $? -eq 0 ]; then
    # Execute the compiled program if compilation was successful
    ./app
fi
