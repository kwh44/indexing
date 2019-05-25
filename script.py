#!/usr/bin/python3.7

import os
import sys
import subprocess

prog = "indexing"
config = "config.dat"  # default path to config file
root_path = None

if os.name == "nt":
    prog = prog + ".exe."

for root, directories, file_list in os.walk('.'):
    if prog in file_list:
        if os.name == "nt":
            prog = root + '\\' + prog
            root_path = root + '\\'
        else:
            prog = root + '/' + prog
            root_path = root + '/'
        break

if len(sys.argv) < 2:
    print("Example of using: script.py <repetitions_number> <config>")
    exit(0)

if len(sys.argv) == 3:
    config = ".\\" + sys.argv[2]

if __name__ == "__main__":
    num = int(sys.argv[1])
    parallel_results = []

    with open(config, "r") as conf_file:
        conf_file.readline()
        res_a = conf_file.readline().split('"')[-2]
        res_n = conf_file.readline().split('"')[-2]

    parallel_res = subprocess.run([prog, config], shell=False, stdout=subprocess.PIPE)

    with open(res_a, "r") as par_file_a:
        parallel_res_a = par_file_a.read()
    with open(res_n, "r") as par_file_n:
        parallel_res_n = par_file_n.read()

    parallel_results.append(float(parallel_res.stdout.decode().strip().split()[-1]))

    for _ in range(num - 1):
        parallel_results.append(
            float(subprocess.run([prog, config], shell=False, stdout=subprocess.PIPE).
                  stdout.decode().strip().split()[-1]))

        with open(res_a, "r") as par_file_a1:
            parallel_res_a1 = par_file_a1.read()
        with open(res_n, "r") as par_file_n1:
            parallel_res_n1 = par_file_n1.read()

        if (parallel_res_a != parallel_res_a1) or (parallel_res_n != parallel_res_n1):
            raise ValueError("Results doesn`t match!")

    print("Best time is {0}.".format(min(parallel_results)))
