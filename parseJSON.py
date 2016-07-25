import json
from time import time

t0 = time()
with open('autodump20160612-170517.json') as data_file:
  sz=data_file.read()
  t1=time()
  data = json.loads(sz)
  t2=time()
  out=open('out.json','w')
  json.dump(data,out)
  print('time: {} {}'.format(t1-t0,t2-t1))
  