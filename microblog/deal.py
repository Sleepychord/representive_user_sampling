#-*- encoding:utf-8 -*-
from __future__ import print_function

import sys
try:
    reload(sys)
    sys.setdefaultencoding('utf-8')
except:
    pass

import codecs
from textrank4zh import TextRank4Keyword, TextRank4Sentence

import os

err_out = open("log.txt","w")
def ReportBug(filename, linenum, content):
    err_out.write(filename+"\nline "+str(linenum)+"\n"+str(content)+"\n")
def sort_by_value(d): 
    items=d.items() 
    backitems=[[v[1],v[0]] for v in items] 
    backitems.sort() 
    backitems.reverse()
    return [ backitems[i][1] for i in range(0,len(backitems))] #200

key_list = {}
key_dict = {}

root_dir = raw_input()
dirs = os.listdir(root_dir)
LIMIT = 1
limit_cnt = 0

for subdir in dirs:
    subdir_full = os.path.join(root_dir, subdir)
    if(os.path.isdir(subdir_full)):
        if(limit_cnt >= LIMIT):
            break
        limit_cnt += 1
        for microblogfile in os.listdir(subdir_full):
            print ("dealing with "+os.path.join(subdir_full, microblogfile))
            f = open(os.path.join(subdir_full, microblogfile), 'r')
            cnt = 0
            for line in f.readlines():
                cnt += 1
                print("line ", cnt)
                tmp_arr = line.split()
                if(len(tmp_arr) < 11):
                    ReportBug(os.path.join(subdir_full, microblogfile), cnt, tmp_arr)
                    continue
                user = tmp_arr[2]
                text = ''.join(tmp_arr[7:-3])
                if (text.strip()[:6] == "你好" or text.strip()[:9] == "尊敬的"):
                    continue
                tr4w = TextRank4Keyword()
                tr4w.analyze(text=text, lower=True, window=2)
                num = len(text) / 50 + 1;
                tmp_key_list = []
                for item in tr4w.get_keywords(num, word_min_len=1):
                    tmp_key_list.append(item.word)
                    if(key_dict.get(item.word)):
                        key_dict[item.word] += 1
                    else:
                        key_dict[item.word] = 1
                if(key_list.get(user)):
                    key_list[user].extend(tmp_key_list)
                else:
                    key_list[user] = tmp_key_list
print("begin sorting")
frequent_key = sort_by_value(key_dict)[:300]
fout = open('user.txt', "w")
fout.write('QQ\t')
for key in frequent_key:
    fout.write(key + '\t')
fout.write('\n')
for (user, key_arr) in key_list.items():
    fout.write(user + '\t')
    key_set = set(key_arr)
    for key in frequent_key:
        if(key in key_set):
            fout.write('1\t')
        else:
            fout.write('0\t')
    fout.write('\n')





