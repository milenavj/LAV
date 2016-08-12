#!/bin/bash
name=$1
name_at="$1"_at.c
cp $name $name_at
CheckMain.exe $name_at -- | indent -br > tmp.c
mv tmp.c $name_at
