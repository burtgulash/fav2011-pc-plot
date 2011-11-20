file="test.eps"

make

./plot "$1" "$file" "$2"
evince "$file"

make clean
# rm "$file"
