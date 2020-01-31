Project #1
Zachary Porter
464701947
--------
To compile: 
    make all
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

muliTestOutput.txt
    Output from a test of multi



I use an integer and a 2d character array to store the background processes

People who I talked with when working on this problem: Nour Elmanaki


RESOURCES CONSULTED:
https://ia.wpi.edu/cs3013-walls/resources.php?page=show_project&id=6
man system
man execl
man fork
man getrusage
man waitpid
http://www.cplusplus.com/reference/cstdio/sprintf/
https://stackoverflow.com/questions/5769734/what-are-the-different-versions-of-exec-used-for-in-c-and-c
https://gist.github.com/tkuchiki/4b77005cc64426b28c3d
man /usr/bin/time
https://stackoverflow.com/questions/13274786/how-to-share-memory-between-process-fork
https://askubuntu.com/questions/385528/how-to-increment-a-variable-in-bash
https://stackoverflow.com/questions/6958689/running-multiple-commands-with-xargs/6958957#6958957
https://stackoverflow.com/questions/4235519/counting-number-of-occurrences-of-a-char-in-a-string-in-c
man cut


