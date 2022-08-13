#!/bin/bash
#this example prints line count for all found files
total=0
find Source/ -type f -name "CMakeSources.cmake" -o -name "CMakeLists.txt" | while read FILE; do
    #you see use grep instead wc ! for properly counting
    count=$(grep -c ^ < "$FILE")
    echo "$FILE has $count lines"
    let total=total+count #in bash, you can convert this for another shell
    echo "$total"
	echo "$total" > cmake.txt
done
find Source/CMake -type f -name "*.cmake" | while read FILE; do
    #you see use grep instead wc ! for properly counting
    count=$(grep -c ^ < "$FILE")
    echo "$FILE has $count lines"
    let total=total+count #in bash, you can convert this for another shell
    echo "$total"
    echo "$total" > cmake_conf.txt
done
total=0
find Data/ -type f -name "*.hlsl" -o -name "*.txt" -o -name "*.cpp" | while read FILE; do
    #you see use grep instead wc ! for properly counting
    count=$(grep -c ^ < "$FILE")
    echo "$FILE has $count lines"
    let total=total+count #in bash, you can convert this for another shell
    echo "$total"
    echo "$total" > other.txt
done
total=0
find Source/ -type f -name "*.cpp" -o -name "*.h" | while read FILE; do
    #you see use grep instead wc ! for properly counting
    count=$(grep -c ^ < "$FILE")
    echo "$FILE has $count lines"
    if (echo "$FILE" | egrep "(.*)\/Te([a-zA-Z0-9]+)\.(.*)"); then
        let total=total+count #in bash, you can convert this for another shell
        echo "$total"
        echo "$total" > code.txt
    fi
done
read touche