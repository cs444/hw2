#!/usr/bin/env python
import string
import random
import os

#dir is not keyword
def makedir(dirname):
    try:
        os.makedirs(dirname)
    except OSError:
        pass
    # let exception propagate if we just can't
    # cd into the specified directory
    os.chdir(dirname)

# generate a random string as a dir name
def GenRandomletters(len):
    RandoString = random.choice(string.ascii_lowercase)
    for i in range(len):
        RandoString += (random.choice(string.ascii_lowercase)) #gen random char and append.
    return RandoString

#####Begin Program#####
makedir('test')

for i in range(random.randint(10,50)):#write file
    WritePtr = open('test'+str(i)+'.txt','w+')
    WritePtr.write(GenRandomletters(random.randint(100,10000)) + '\n')
    WritePtr.close()

#####End Program#######