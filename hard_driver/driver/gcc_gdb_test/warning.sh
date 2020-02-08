echo "gcc -ansi"
gcc warning.c -o warning -ansi

echo 
echo "gcc -pedantic"
gcc warning.c -o warning -pedantic

echo
echo "gcc -Wall"
gcc warning.c -o warning -Wall

echo
echo "gcc -Wall -pedantic"
gcc warning.c -o warning -Wall -pedantic

echo 
echo "gcc -w"
gcc warning.c -o warning -w

echo
echo "gcc -Werror"
gcc warning.c -o warning -Werror

