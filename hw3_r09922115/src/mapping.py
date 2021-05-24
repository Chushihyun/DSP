import sys

dic={'ㄅ':0,'ㄆ':1,'ㄇ':2,'ㄈ':3,'ㄉ':4,'ㄊ':5,'ㄋ':6,'ㄌ':7,\
'ㄍ':8,'ㄎ':9,'ㄏ':10,'ㄐ':11,'ㄑ':12,'ㄒ':13,'ㄓ':14,'ㄔ':15,\
'ㄕ':16,'ㄖ':17,'ㄗ':18,'ㄘ':19,'ㄙ':20,'ㄧ':21,'ㄨ':22,'ㄩ':23,\
'ㄚ':24,'ㄛ':25,'ㄜ':26,'ㄝ':27,'ㄞ':28,'ㄟ':29,'ㄠ':30,'ㄡ':31,\
'ㄢ':32,'ㄣ':33,'ㄤ':34,'ㄥ':35,'ㄦ':36}

input=sys.argv[1]
output=sys.argv[2]
f_in=open(input,"r",encoding='big5hkscs')
data=f_in.readlines()
f_in.close()
Zhu_Yin_list=[]
for i in range(37):
    Zhu_Yin_list.append([])

for i in range(len(data)):
    line=data[i].split(' ')
    word=line[0]
    zhuyins=line[1].split('/')
    for zhu in zhuyins:
        cap=zhu[0]
        if word in Zhu_Yin_list[dic[cap]]:
            continue
        else:
            Zhu_Yin_list[dic[cap]].append(word)
        


f_out=open(output,"w",encoding='big5hkscs')

total_set=set()

for key in dic:
    if len(Zhu_Yin_list[dic[key]])>0:
        word_list=Zhu_Yin_list[dic[key]]
        l=key+" "+" ".join(word_list)
        print(l,file=f_out)
        for word in word_list:
            total_set.add(word)
        
for word in list(total_set):
    print(word+" "+word,file=f_out)
        
f_out.close()


