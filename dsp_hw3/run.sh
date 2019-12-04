#./mydisambig test_data/1.txt ZhuYin-Big5.map bigram.lm result_my/1_out.txt
#./mydisambig test_data/2.txt ZhuYin-Big5.map bigram.lm result_my/2_out.txt
#./mydisambig test_data/3.txt ZhuYin-Big5.map bigram.lm result_my/3_out.txt
#./mydisambig test_data/4.txt ZhuYin-Big5.map bigram.lm result_my/4_out.txt
#./mydisambig test_data/5.txt ZhuYin-Big5.map bigram.lm result_my/5_out.txt
#./mydisambig test_data/6.txt ZhuYin-Big5.map bigram.lm result_my/6_out.txt
#./mydisambig test_data/7.txt ZhuYin-Big5.map bigram.lm result_my/7_out.txt
#./mydisambig test_data/8.txt ZhuYin-Big5.map bigram.lm result_my/8_out.txt
#./mydisambig test_data/9.txt ZhuYin-Big5.map bigram.lm result_my/9_out.txt
#./mydisambig test_data/10.txt ZhuYin-Big5.map bigram.lm result_my/10_out.txt

#for i in {1..10};
#do
#    perl separator_big5.pl test_data/${i}.txt > test_data/${i}_seg.txt
#    ./mydisambig test_data/${i}_seg.txt ZhuYin-Big5.map bigram.lm result_my/${i}_out.txt
#    rm test_data/${i}_seg.txt
#done

#for i in {1..10};
#do
#    perl separator_big5.pl test_data/${i}.txt > test_data/${i}_seg.txt
#    disambig -text test_data/${i}_seg.txt -map ZhuYin-Big5.map -lm bigram.lm -order 2 > result_srilm/${i}_out.txt
#    rm test_data/${i}_seg.txt
#done
############################################################# trigram
for i in {1..10};
do
    perl separator_big5.pl test_data/${i}.txt > test_data/${i}_seg.txt
    ./trigram test_data/${i}_seg.txt ZhuYin-Big5.map trigram.lm result_my_tri/${i}_out.txt
    rm test_data/${i}_seg.txt
done

#for i in {1..10};
#do
#    perl separator_big5.pl test_data/${i}.txt > test_data/${i}_seg.txt
#    disambig -text test_data/${i}_seg.txt -map ZhuYin-Big5.map -lm trigram.lm -order 3 > result_srilm_tri/${i}_out.txt
#    rm test_data/${i}_seg.txt
#done












