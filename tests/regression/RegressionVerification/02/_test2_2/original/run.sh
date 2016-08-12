#!/bin/bash
name=$1
name_at="$1"_at.c
indent -br < $name > $name_at
sed -i '1i #define __RET_UNDEF -1' $name_at
CommentMain.exe $name_at -- | indent -br > tmp.c
mv tmp.c $name_at
until Recurser.exe $name_at -- | indent -br > tmp.c ; (exit ${PIPESTATUS[0]})
do
    mv tmp.c $name_at
    echo "."
    sleep 1
done
echo "#include \"../check.h\"" >> $name_at

sed -i 's/\bstrlen\b/mystrlen/g' $name_at
sed -i 's/\babs\b/myabs/g' $name_at
if egrep -q "mystrlen|myabs" "$name_at"; then
  sed -i '1i #include \"myfunctions.h\"' $name_at
fi
