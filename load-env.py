from os.path import isfile
assert isfile(".env")
print("Environment variables loaded from .env file")

env = DefaultEnvironment()
try:
    f = open(".env", "r")
    lines = f.readlines()
    envs = []
    for line in lines:
        if line.strip().startswith("#") or "=" not in line:
            continue
        name = line.strip().split("=")[0].strip()
        value = line.strip().split("=")[1].strip()
        envs.append("-D " + name + "='" + value + "'")
    print(" ".join(envs))
    env.Append(BUILD_FLAGS=envs)
except IOError:
    print("File .env not accessible")
finally:
    f.close()
    print("Finished loading .env file")