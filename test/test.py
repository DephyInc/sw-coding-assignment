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

def TESTCASE_CREATE():
    # create rest of the files
    clean()
    for i in range(1,12):
        expFile = f"veprom_{i-1}.map"
        out = run(["create", f"{i}"]) # also vary size!
        assert out.returncode == 0
        assert out.stdout == bytearray(expFile, 'latin-1')
        assert os.path.exists(expFile)
        with open(expFile, "rb") as file:
            assert file.read() == bytearray(1024 * i)    
    clean()

def main():
    if (len(sys.argv) > 1):
        if (sys.argv[1] == "all"):
            config() # optional: project clean and configure
    clean()
    build()
    TESTCASE_CREATE()
    clean()
    print("PASS!")

if __name__ == "__main__":
    main()