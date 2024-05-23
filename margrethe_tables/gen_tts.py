import hashlib
import pickle
import random, os


str_seed = "Welcome to Margrethe"

# Compute str hash used as a seed

h = hashlib.new('sha256')
h.update(str_seed.encode('UTF-8'))
strex = h.hexdigest()
random.seed(strex)

lut = []
for _ in range(1<<18):
    lut.append(random.randrange(16))

def sum_tt(tt1, tt2):
    ret_tt = []
    for (t1, t2) in zip(tt1, tt2):
        ret_tt.append(t1^t2)
    return ret_tt

tts = [[] for _ in range(4)]
for im in lut:
    for i in range(4):
        tts[i].append((im>>i) & 0x1)
tts.append(sum_tt(tts[0], tts[1]))
tts.append(sum_tt(tts[0], tts[2]))
tts.append(sum_tt(tts[0], tts[3]))
tts.append(sum_tt(tts[1], tts[2]))
tts.append(sum_tt(tts[1], tts[3]))
tts.append(sum_tt(tts[2], tts[3]))
tts.append(sum_tt(sum_tt(tts[0], tts[1]), tts[2]))
tts.append(sum_tt(sum_tt(tts[0], tts[1]), tts[3]))
tts.append(sum_tt(sum_tt(tts[0], tts[2]), tts[3]))
tts.append(sum_tt(sum_tt(tts[1], tts[2]), tts[3]))
tts.append(sum_tt(sum_tt(sum_tt(tts[0], tts[1]), tts[2]), tts[3]))

os.system("mkdir -p ../data")
with open("../data/tts", 'w') as fd:
    for tt in tts:
        for t in tt:
            fd.write(str(t))
        fd.write('\n')
