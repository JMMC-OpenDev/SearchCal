mkdir test_text
rm -f test_text/*

cd ./test
for i in $(find . -name "*.vot"); do (echo "convert $i"; stilts tcopy $i ../test_text/$i.txt); done
