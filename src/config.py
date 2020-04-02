from json import load

def loadConfig(path: str = "configuration.json"):
    print("Loading configuration form: {}".format(path))
    with open(path, "r") as jsonFile:
        return load(jsonFile)
    print("Configuration loaded.")
