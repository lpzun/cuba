#####################################################
# This is used for reformat the POP operaton of CPDS
# old:
#   q1 l1 -> q2
# new
#   q1 l1 -> q2 -
# Symbol "-" indicate an epsilon
#####################################################
import fnmatch
import os

for root, dirnames, filenames in os.walk('benchmarks'):
    for filename in fnmatch.filter(filenames, '*.pds'):
        filename = os.path.join(root, filename)
        print(filename)
        #filename = './benchmarks/10_Stefan/stefan-2.pds'
        with open(filename) as ins:
            with open(filename + '.tmp', 'w+') as out:
                line = ins.readline()
                while line:
                    if "->" not in line:
                        print(line.strip())
                        out.write(line)
                    else:
                        ll = line.strip().split()
                        if len(ll) is 4:
                            ll.append('-')
                        print(' '.join(str(x) for x in ll))
                        out.write(' '.join(str(x) for x in ll))
                        out.write("\r\n")
                    line = ins.readline()
        # remove the old file