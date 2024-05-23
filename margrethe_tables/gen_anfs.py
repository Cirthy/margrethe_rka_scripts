import sys
sys.path.append("./DahuHunting")
from BF import BF


# Many possible optimization, but writing and reading the initial ANF is not time consuming.
# The tts file needs to be initialized with gen_tts.py 

def anfs_from_tts(path_tts='../data/tts', path_anfs='../data/anf_'):
	n = 18 # Would be better to read it from the tts, but w/e
	f = BF(n)
	cnt = 0
	with open(path_tts, 'r') as fd:
		char = fd.read(1)
		while(char):
			tt = []
			while(char != '\n'):
				tt.append(int(char))
				char = fd.read(1)
			f.set_TT(tt)
			f.update_ANF()
			write_anf(f, path_anfs + str(cnt))
			cnt += 1
			char = fd.read(1)


def write_anf(f, path):
	with open(path, 'w') as fd:
		for (i, m) in reversed(list(enumerate(f.ANF))):
			if(m):
				fd.write(hex(i)[2:] + '\n')

anfs_from_tts()
