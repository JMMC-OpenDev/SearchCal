mkdir ref_text
rm -f ref_text/*

cd ./ref
for i in $(find . -name "*.vot"); do (echo "convert $i"; stilts tcopy $i ../ref_text/$i.txt); done
