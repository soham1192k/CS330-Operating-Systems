#cleanup
rm -f test/*.tar
rm -rf test/*Dump

#generate path of directory containing tar file
curDir=$(pwd)
tarDir="${curDir}/test"

#name of tar file
tarName="test.tar"

#generate path of tar file 
tarPath="${tarDir}/"
tarPath="$tarPath$tarName"

./myTar -c "$tarDir" $tarName


#extract tar file
./myTar -d "$tarPath"

#path of original files
of1Path="${tarDir}/1"
of2Path="${tarDir}/2"
of3Path="${tarDir}/3"
of4Path="${tarDir}/4"

#path of extracted files
ef1Path="${tarDir}/testDump/1"
ef2Path="${tarDir}/testDump/2"
ef3Path="${tarDir}/testDump/3"
ef4Path="${tarDir}/testDump/4"

#compare contents of original files and extracted files
diff "$of1Path" "$ef1Path"
if [ $? -eq 0 ]
then
	echo "File 1 extracted correctly"
else
	echo "File 1 extracted incorrectly"
fi


diff "$of2Path" "$ef2Path"
if [ $? -eq 0 ]
then
	echo "File 2 extracted correctly"
else
	echo "File 2 extracted incorrectly"
fi


diff "$of3Path" "$ef3Path"
if [ $? -eq 0 ]
then
	echo "File 3 extracted correctly"
else
	echo "File 3 extracted incorrectly"
fi


diff "$of4Path" "$ef4Path"
if [ $? -eq 0 ]
then
	echo "File 4 extracted correctly"
else
	echo "File 4 extracted incorrectly"
fi

