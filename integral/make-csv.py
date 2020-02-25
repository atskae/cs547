import sys
import os
import glob
import pandas as pd

if len(sys.argv) != 2:
    print('Usage: make-csv.py a-b-numSamples-all')
    sys.exit()

# Data parameters
path = sys.argv[1]
params = path.split('-')
a = int(params[0]) # upperLimit
b = int(params[1]) # lowerLimit
numSamples = int(params[2])

if path[-1] != '/':
    path += '/'

fileid = str(a) + '-' + str(b) + '-' + str(numSamples) + '-'
csv_file = fileid + 'all.csv'
filePattern = fileid + '[0-9]*.csv'

# Sort by number of threads
elapsed_t1 = 0.0 # time elapsed for 1 thread
all_data = []
for f in sorted(glob.glob(path + filePattern), key=lambda x: int(x.split('-')[-1].split('.')[0])):
    
    print(f)
    
    df = pd.read_csv(f, index_col=None, header=0)
    elapsed = float(df['elapsed(sec)'])
    numThreads = int(df['numThreads'])
    if numThreads == 1:
        elapsed_t1 = elapsed
        #print('elapsed_t1', elapsed_t1)

    # Compute speedup and efficiency
    speedup = elapsed_t1 / elapsed
    df['speedup'] = speedup
    df['efficiency']  = speedup / numThreads
    
    all_data.append(df)

if len(all_data) == 0:
    print('No files matching pattern %s were found in %s' % (filePattern, path))
    sys.exit()

# Combine data
df = pd.concat(all_data, axis=0, ignore_index=True)
print(df)

df.to_csv(csv_file)
print('Data saved to %s' % csv_file)
