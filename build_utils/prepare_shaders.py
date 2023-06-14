import os
import json

content = ""
pref = "./src/shaders"

for file_name in os.listdir(pref):
    if os.path.isfile(os.path.join(pref, file_name)):
        with open(os.path.join(pref, file_name)) as f:
            content += f"const std::string {file_name.split('.')[0]} = {json.dumps(f.read())};\n"

with open("./include/shaders.h", "w") as f:
    f.write(content)
