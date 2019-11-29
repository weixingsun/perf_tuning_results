#!/bin/sh

FILE="stream.log"
KEY="Triad"
until [ `grep $KEY $FILE|wc -l` -gt 0 ]; do
    sleep 5
done

SCORE=`grep $KEY $FILE |awk '{print $2}'`
echo "Found Triad $SCORE"
