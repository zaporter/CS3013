Project #1
Zachary Porter
464701947

--------
To compile: 
    make all
    or (make boring or make custom or make multi)
To run boring:
    ./boring
To run custom:
    ./custom
To run multi:
    ./multi [list of processes to be run in the background (Must be ascending order)]
    Ex:  ./multi 1 3 5

-------
Important files:
custom.txt
    list of commands to be run by custom commander
multi.txt
    list of commands to be run by multi commander
customTestOutput.txt
    Output from a test of custom
muliTestOutput.txt
    Output from a test of multi


-------
Design choice for Multicommander:
Background processes are given an ID between 0 and 31
Process names are stored in a 2d array of length 32 x 128 (max length is 128 charcters for a command). 
An int is then shared between all processes. If that processes bit is set then it is still running. After it finishes, it clears its bit. 
The index loops back around to zero after reaching 32 background processes

-------
People who I talked with when working on this problem: Nour Elmanaki

-------
RESOURCES CONSULTED:
https://ia.wpi.edu/cs3013-walls/resources.php?page=show_project&id=6
man system
man execl
man fork
man getrusage
man waitpid
man cut
man /usr/bin/time
http://www.cplusplus.com/reference/cstdio/sprintf/
https://stackoverflow.com/questions/5769734/what-are-the-different-versions-of-exec-used-for-in-c-and-c
https://gist.github.com/tkuchiki/4b77005cc64426b28c3d
https://stackoverflow.com/questions/13274786/how-to-share-memory-between-process-fork
https://askubuntu.com/questions/385528/how-to-increment-a-variable-in-bash
https://stackoverflow.com/questions/6958689/running-multiple-commands-with-xargs/6958957#6958957
https://stackoverflow.com/questions/4235519/counting-number-of-occurrences-of-a-char-in-a-string-in-c
https://stackoverflow.com/questions/1258550/why-should-you-use-strncpy-instead-of-strcpy


