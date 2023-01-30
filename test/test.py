import subprocess
import shutil
import sys
import os

EXE = r".\build\Debug\veprom.exe"

def config(): # configure cmake project
    shutil.rmtree("build")
    out = subprocess.run([
        "cmake", "-S", "source", "-B", "build"
    ])
    assert out.returncode == 0, "cmake config error"

def build(): # build cmake project
    out = subprocess.run([
        "cmake", "--build", "build"
    ])
    assert out.returncode == 0, "cmake build error"

def clean(): # get rid of all veprom map files
    for i in range(256):
        filename = f"veprom_{i}.map"
        if os.path.exists(filename):
            os.remove(filename)

def run(args: list)-> subprocess.CompletedProcess: # run exe and return proc result
    return subprocess.run([EXE] + args,
        stdout=subprocess.PIPE
    )

def TESTCASE_CREATE(): # test the create method
    for i in range(0,16):
        expFile = f"veprom_{i}.map"
        size = (i % 2) + 1 # vary size a bit
        out = run(["create", f"{size}"])
        assert out.returncode == 0
        assert out.stdout == bytearray(expFile, 'latin-1')
        assert os.path.exists(expFile)
        with open(expFile, "rb") as file:
            assert file.read() == bytearray(1024 * size)
    out = run(["create", "1"]) # no more allowed
    assert out.returncode != 0

def TESTCASE_LOAD(): # test the load method
    expFile = "veprom_context.map"
    outCreate = run(["create", "1"]).stdout
    assert(isinstance(outCreate, bytes))
    filename = outCreate.decode('latin-1')
    outLoad = run(["load", filename])
    assert outLoad.returncode == 0
    assert os.path.exists(expFile)
    with open(expFile, "rb") as file:
        assert file.read() == filename.encode('latin-1')
    outLoadFail = run(["load", "doesnotexist"])
    assert outLoadFail.returncode != 0

def main():
    if (len(sys.argv) > 1):
        if (sys.argv[1] == "all"):
            config() # optional: project clean and configure
    build()
    for test in [
        TESTCASE_CREATE,
        TESTCASE_LOAD,
    ]:
        clean()
        test()
        clean()
    print("<<< PASS >>>")

if __name__ == "__main__":
    main()