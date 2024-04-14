'''
Bundle any number of files into one generated C include header.

Header will have one symbol per file. Symbol name is related to filename. Use
the --inpath to switch to input directory, this part of path will not be
included in symbol name.

'''

import argparse
import os
import sys

def symbol_name(filename):
    '''Convert filename into symbol name'''
    # Try to be compatible with xxd -i
    # There are potential ambiguities, e.g. between abc/def.txt and abc_def.txt
    # Not sure there is a better scheme than this though
    return filename.replace('/', '_').replace('.', '_')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Bundle sources into one generated output C header file')
    parser.add_argument('--inpath')
    parser.add_argument('--out', required=True)
    parser.add_argument('sources', nargs='+')
    args = parser.parse_args()
    with open(args.out, 'w') as fout:
        if args.inpath:
            os.chdir(args.inpath)
        out = []
        for source in args.sources:
            out.append(f'unsigned char {symbol_name(source)}[]' + ' = {\n')
            with open(source, 'rb') as fin:
                contents = fin.read()
                count = 0
                for b in contents:
                    count += 1
                    b_str = format(b, '#04x')
                    if count == 1:
                        out.append(f'  {b_str}')
                    else:
                        if count % 12 != 1:
                            out.append(f', {b_str}')
                        else:
                            out.append(f',\n  {b_str}')
                out.append('\n};\n')
                out.append(f'unsigned int {symbol_name(source)}_len = {len(contents)};\n')
        fout.write("".join(out))
