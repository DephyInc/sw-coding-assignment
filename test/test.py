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
    filenameContext = "veprom_context.map"
    if os.path.exists(filenameContext):
        os.remove(filenameContext)

def run(args: list)-> subprocess.CompletedProcess: # run exe and return proc result
    return subprocess.run([EXE] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

def byteArrayToInt(arr: bytes):
    ret = 0
    factor = 1
    for i in range(len(arr)):
        ret += arr[i] * factor
        factor *= 256
    return ret

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

def TESTCAST_WRITE_RAW(): # test the write_raw method
    assert run(["write_raw", "0", "NoContext"]).returncode != 0
    outCreate = run(["create", "1"]).stdout
    assert run(["write_raw", "0", "StillNoContext"]).returncode != 0
    assert(isinstance(outCreate, bytes))
    filename = outCreate.decode('latin-1')
    run(["load", filename])
    assert run(["write_raw", "0", "Hello World!"]).returncode == 0
    assert run(["write_raw", "1024", "EOF"]).returncode != 0
    assert run(["write_raw", "1023", "EOF"]).returncode != 0
    assert run(["write_raw", "1022", "EOF"]).returncode != 0
    assert run(["write_raw", "1021", "EOF"]).returncode == 0
    
def TESTCASE_READ_RAW(): # test the read_raw method
    assert run(["read_raw", "0", "32"]).returncode != 0 # no context
    outCreate = run(["create", "1"]).stdout
    assert(isinstance(outCreate, bytes))
    filename = outCreate.decode('latin-1')
    run(["load", filename])
    data = "Hello world!"
    assert run(["write_raw", "0", data]).returncode == 0
    outReadRaw = run(["read_raw", "0", f"{len(data)}"])
    assert outReadRaw.returncode == 0
    assert outReadRaw.stdout == data.encode('latin-1')
    assert run(["read_raw", "1024", "1"]).returncode != 0

def TESTCASE_WRITE(): # test the write method
    outCreate = run(["create", "256"]).stdout
    assert(isinstance(outCreate, bytes))
    veprom = outCreate.decode('latin-1')
    run(["load", veprom])
    data = "Hello world! " * 512
    filename = "myfile.map"
    with open(filename, "w") as file:
        file.write(data)
    outWrite = run(["write", filename])
    assert outWrite.returncode == 0
    outRead = run(["read_raw", "0", "128"]) # check filename in drive
    assert outRead.stdout == bytes(filename, 'latin-1') + bytearray(128 - len(filename))
    outRead = run(["read_raw", "128", "8"]) # check file size in drive
    assert byteArrayToInt(outRead.stdout) == len(data)
    os.remove(filename)

def TESTCASE_LIST(): # test the write method
    outCreate = run(["create", "256"]).stdout
    assert(isinstance(outCreate, bytes))
    veprom = outCreate.decode('latin-1')
    run(["load", veprom])
    data1 = "Hello world! " * 512
    filename1 = "myfile1.map"
    with open(filename1, "w") as file:
        file.write(data1)
    data2 = "Howdy! " * 512
    filename2 = "myfile2.map"
    with open(filename2, "w") as file:
        file.write(data2)
    run(["write", filename1])
    outList = run(["list"])
    assert outList.returncode == 0
    # assert outList.stdout == bytes(filename1, 'latin-1')
    run(["write", filename2])
    outList = run(["list"])
    assert outList.returncode == 0
    assert outList.stdout == (
        bytes(filename1, 'latin-1') + b"\r\n" + 
        bytes(filename2, 'latin-1') + b"\r\n"
    )
    os.remove(filename1)
    os.remove(filename2)

def main(): # run through all test cases
    if (len(sys.argv) > 1):
        if (sys.argv[1] == "all"):
            config() # optional: project clean and configure
    build()
    for test in [
        TESTCASE_CREATE,
        TESTCASE_LOAD,
        TESTCAST_WRITE_RAW,
        TESTCASE_READ_RAW,
        TESTCASE_WRITE,
        TESTCASE_LIST,
    ]:
        clean()
        test()
        clean()
    print("<<< PASS >>>")

if __name__ == "__main__":
    main()