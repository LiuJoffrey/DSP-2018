# coding=Big5

from argparse import ArgumentParser


def main():
    parser = ArgumentParser()
    parser.add_argument("infile", help="from Big5-ZhuYin file input")
    parser.add_argument("outfile", help="To ZhuYin-Big5 file output")

    args = parser.parse_args()
    
    zh_map = {}    

    with open(args.infile, 'rb') as f:
        for line in f:
            #data = line.decode('big5')
            data = line.decode('big5hkscs')
            data = data.strip().split(' ')
            ch = data[0]
            zh_list = data[1].split('/')

            for zh in zh_list:
                if zh[0] in zh_map:
                    zh_map[zh[0]] += [ch]
                else:
                    zh_map[zh[0]] = [ch]

            zh_map[ch] = [ch] 
            
    for zh in zh_map:
        zh_map[zh] = list(set(zh_map[zh]))

    with open(args.outfile, 'w', encoding='big5-hkscs') as f:
        for zh in zh_map: 
            f.write(zh + "	" + ' '.join(zh_map[zh]) + '\n')
            
    
        
            
        
    

if __name__ == '__main__':
    main()

