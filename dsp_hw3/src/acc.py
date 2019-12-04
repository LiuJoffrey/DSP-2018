import os

my_path = "result_my_tri"
lib_path = "result_srilm_tri"
# lib_path = "/media/cnrg-ntu2/HDD1TB/r07921052/dsp/Spring-2015-DSP-master/dsp_hw3/hw3_[b01902040]/result2"
all_my_file = sorted(os.listdir(my_path))
all_lib_file = sorted(os.listdir(lib_path))

for i in range(1,11)[:]:
    p_name = str(i)+"_out.txt"
    g_name = str(i)+"_out.txt"
    print(p_name)
    pre_f = os.path.join(my_path, p_name)
    gro_f = os.path.join(lib_path, g_name)
    
    pre_sen = []
    gro_sen = []
    with open(pre_f, 'r', encoding="big5hkscs") as f:
        for line in f:
            #line.decode("big5hkscs")
            pre_sen.append(line)

    with open(gro_f, 'r', encoding="big5hkscs") as f:
        for line in f:
            #line.decode("big5hkscs")
            gro_sen.append(line)

    for l in range(len(gro_sen)):
        if pre_sen[l] != gro_sen[l]:
            #print(pre_sen[l], " -> ", gro_sen[l])
            print(pre_sen[l])
            print(gro_sen[l])
            print("===========================")
        #print(pre_sen[l] == gro_sen[l])
