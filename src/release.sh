#!/bin/bash

# Called with a single parameter in the form "alpha.XX" or "beta.A"
MINPARAMS=1

if [ $# -lt "$MINPARAMS" ]
then
  echo
  echo "This script needs at least $MINPARAMS command-line arguments!"
  exit 0
fi

VERSION="v5.0-$1"
echo $VERSION

git checkout master
rm LATEST_TAG.txt
echo $VERSION >> LATEST_TAG.txt
git add LATEST_TAG.txt
git commit -m "Tagged release $VERSION"
git push origin master
git tag -a $VERSION -m "Release $VERSION"
git push --tags
