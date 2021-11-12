import json
import sys
import os

jsonfile = sys.argv[1]

# Opening JSON file
f = open(jsonfile)
 
# returns JSON object as
# a dictionary
data = json.load(f)
 
# animation name: list of frame 
storage = {}
w = data['meta']['size']['w']
h = data['meta']['size']['h']

# Iterating through the json
# list
for frame in data['frames']:
    path = os.path.normpath(frame['filename'])
    anim_clip_name = path.split(os.sep)[0] + ".txt"
    if anim_clip_name not in storage:
        storage[anim_clip_name] = []
    storage[anim_clip_name].append((
        str(frame['frame']['x']), 
        str(h - (frame['frame']['y'] + frame['frame']['h'])), 
        str(frame['frame']['w']), 
        str(frame['frame']['h'])
    ))

for anim_clip_name in storage:
    with open(anim_clip_name, 'w') as txt:
        for frame in storage[anim_clip_name]:
            txt.write(frame[0] + ",")
            txt.write(frame[1] + ",")
            txt.write(frame[2] + ",")
            txt.write(frame[3] + "\n")

# Closing file
f.close()