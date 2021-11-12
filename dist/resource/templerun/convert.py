import json
import sys
import os

filename = sys.argv[1]
fileprefix, file_extension = os.path.splitext(filename)
# Opening JSON file
f = open(filename)
 
# returns JSON object as
# a dictionary
data = json.load(f)
 
# Iterating through the json
# list
with open(fileprefix + '.txt', 'w') as txt:
    for frame in data['frames']:
        txt.write(str(frame['frame']['x']) + ",")
        txt.write(str(frame['frame']['y']) + ",")
        txt.write(str(frame['frame']['w']) + ",")
        txt.write(str(frame['frame']['h']) + "\n")

# Closing file
f.close()