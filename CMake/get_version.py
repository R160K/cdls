import json
with open("../config.json") as f:
    data = json.load(f)
    print(data["VERSION"], end="")
    
    # g = open("gvlog", "w")
    # g.write(data["VERSION"])
    # g.close()