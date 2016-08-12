#!/bin/bash
name=$1
name_at="$1"_at.c
cp $name $name_at
sed -i '1i #define __RET_UNDEF -1' $name_at
sed -i 's/\b[0-9a-zA-Z_]*strcspn[0-9a-zA-Z_]*\b/mystrcspn/g' $name_at
sed -i 's/\b[0-9a-zA-Z_]*strlen[0-9a-zA-Z_]*\b/mystrlen/g' $name_at
CommentMain.exe $name_at -- | indent -br > tmp.c
mv tmp.c $name_at
until Recurser.exe $name_at -- | indent -br > tmp.c ; (exit ${PIPESTATUS[0]})
do
    mv tmp.c $name_at
    echo "."
    sleep 1
done
echo "#include \"../check.h\"" >> $name_at
