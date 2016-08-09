#!/usr/bin/env python

import datetime
import subprocess
import os

directory = '/home/john/git/winder-lcd/Winder'

os.chdir(directory)

git_data = subprocess.check_output('git rev-parse --short HEAD', shell=True)
git_data = git_data.rstrip('\n')


now = datetime.datetime.now()

HEADER = open(directory + "/version.h", "w")

HEADER.write("#ifndef VERSION_H\n#define VERSION_H\n")

HEADER.write("const char git_sha[] = \"%s\";\n" % git_data)

HEADER.write("const char build_date[] = \"%s\";\n" % now.strftime("%d/%m/%Y %H:%M:%S"))

HEADER.write("#endif\n")

HEADER.close()
