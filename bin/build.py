#!python3
from distutils.log import info
import glob
import os
import sys
import time

total_time = 0

#####################################################################################
# CONFIGURE LOCATIONS OF THINGS
#####################################################################################

BINARY="a.out"

ROOT = "."
BISON = "bison"
FLEX = "flex"
CLANG = "clang"

if os.name != "posix":
    pass
    # ROOT = "C:/Users/matth/code/c2d/build"
    # BISON = "C:/Users/matth/code/winflexbison/win_bison.exe"
    # FLEX = "C:/Users/matth/code/winflexbison/win_flex.exe"
    # CLANG = "clang.exe"
    # CCACHE = "C:/Users/matth/code/ccache/ccache.exe"

def TIMEIT(func):
    def inner(*args, **kwargs):
        global total_time
        initial_time = time.time()
        result = func(*args, **kwargs)
        exec_time = time.time() - initial_time
        print(f"{func.__name__} EXECUTED IN {exec_time:.2f} seconds")
        total_time += exec_time
        return result
    return inner

def findFiles(path, extension, exclude=None):
    """Recursively search for files with a specified extension

    Args:
        path (str): Path to search
        extension (str): Extension to search
        exclude (list, optional): A list of filenames to exclude (without paths)

    Returns:
        list: List of files
    """
    if not path.endswith(os.sep):
        path = path + os.sep
    assert exclude is None or isinstance(exclude, list)
    if not exclude:
        exclude = []
    return [x for x in glob.glob(f"{path}**{os.sep}*.{extension}", recursive=True) if not any([x.endswith(y) or y in x for y in exclude])]

@TIMEIT
def clang(filename, standard, files, flags, libraries, linkDirs, includeDirs, verbose=True, compileCommands=False):
    """Generate an executable with the clang compiler

    Args:
        filename (str): The name of the binary to generate (w/o extension)
        standard (str): The c standard to compile to
        files (list): A list of files to compile
        flags (list): A list of optional compiler flags
        libraries (list): A list of libraries to link
        linkDirs (list): A list of folders to look for these libraries in
        includeDirs (list): A list of paths to relative import source code from
        verbose (bool, optional): Print out the compiler command
        compileCommands (bool, optional): Generate compile_commands.json for your IDE
    """
    
    # Prevents us from missing compile errors and accidentally running the last compiled version
    if os.path.exists(f"..{os.sep}build{os.sep}{filename}.exe"):
        if os.path.exists(f"..{os.sep}build{os.sep}{filename}.old.exe"):
            os.remove(f"..{os.sep}build{os.sep}{filename}.old.exe")
        os.rename(f"..{os.sep}build{os.sep}{filename}.exe", f"..{os.sep}build{os.sep}{filename}.old.exe")
    
    command = (f"{CLANG} {'-MJ compile_commands.json' if compileCommands else ''} "
                    f"-std=\"{standard}\" "
                    # f"-mavx "
                    f"-march=native "
                    # f"-O2 "
                    f"-g {' '.join(flags)} -o {filename}.exe "
                    f"{' '.join(['-l ' + p for p in libraries])} "
                    f"{' '.join(['-L ' + p for p in linkDirs])} "
                    f"{' '.join(['-I ' + p for p in includeDirs])} "
                    f"{' '.join([f for f in files])}")
    if verbose:
        print(command)
    os.system(command)
    
    if compileCommands:
        with open("compile_commands.json", "r") as rf:
            with open("../compile_commands.json", "w") as wf:
                wf.write("[\n")
                wf.write(rf.read().strip()[:-1])
                wf.write("\n]")

        with open("../compile_commands.json", "r") as rf:
            with open("compile_commands.json", "w") as wf:
                wf.write(rf.read())
       
@TIMEIT         
def clang_library(filename, standard, files, flags, libraries, linkDirs, includeDirs, debugSymbols=True, verbose=True):
    to_combine = []
    for file in files:
        file_filename = file.split(os.sep)[-1].replace(".c", ".o")
        to_combine.append(file_filename)
        command = (
                f"{CLANG} "
                f"{'-g ' if debugSymbols else ''}"
                f"-std=\"{standard}\" "
                f"-mavx "
                #TODO: -march=native instead of the above line once we remove stbi
                # f"-O3 "
                f"-mstackrealign "
                f"{' '.join(['-l ' + p for p in libraries])} "
                f"{' '.join(['-L ' + p for p in linkDirs])} "
                f"{' '.join(['-I ' + p for p in includeDirs])} "
                f"{' '.join(flags)} -o {file_filename} "
                "-fuse-ld=llvm-lib -c -Wall "
                f"{file} "
        )
        if verbose:
            print(command)
        os.system(command)
    
    command = f"llvm-ar rc {filename}.a {' '.join(f for f in to_combine)}"
    
    if verbose:
        print(command)
    os.system(command)
    
    if os.path.exists(f"{filename}.old.lib"):
        os.remove(f"{filename}.old.lib")
    if os.path.exists(f"{filename}.lib"):
        os.rename(f"{filename}.lib", f"{filename}.old.lib")
    
    os.rename(f"{filename}.a", f"{filename}.lib")
    
    return filename

@TIMEIT
def clDLLAlt(filename, standard, files, flags, libraries, linkDirs, includeDirs, verbose=True, compileCommands=False):
    # Use this for hot reloading...
    clDLL(filename + "_alt", standard, files, flags, libraries, linkDirs, includeDirs, verbose, compileCommands)

@TIMEIT
def clDLL(filename, standard, files, flags, libraries, linkDirs, includeDirs, verbose=True, compileCommands=False):
    command = (f"{CLANG} "
               f"--shared "
                f"-std=\"{standard}\" "
                f"-mavx "
                #TODO: -march=native instead of the above line for speed boost
                # f"-O3 "
                f"-g {' '.join(flags)} -o {filename}.dll "
                f"{' '.join(['-l ' + p for p in libraries])} "
                f"{' '.join(['-L ' + p for p in linkDirs])} "
                f"{' '.join(['-I ' + p for p in includeDirs])} "
                f"{' '.join([f for f in files])}")
    
    if verbose:
        print(command)
    os.system(command)


def parser(name, inFolder, outFolder, header=True, verbose=False, flexOnly=False):
    """Helper to compile a parser

    Args:
        name (str): Name of the parser
        inFolder (_type_): Where to look for .l and .y files
        outFolder (_type_): Where to put the compiled files
        header (bool, optional): Compile a header also
        verbose (bool, optional).
        flexOnly (bool, optional): _description_. There is no compiler, just a lexer.
    """
    if not flexOnly:
        os.system(f"{BISON} --locations {'-Wcounterexamples' if verbose else ''} {'-d' if header else ''} {inFolder}{name}.y -o {outFolder}{name}.tab.c")
    os.system(f"{FLEX} --nounistd -o {outFolder}{name}.yy.c {inFolder}{name}.l")

        
if __name__ == '__main__':
    os.chdir(ROOT)
    args = [x for x in sys.argv if not x.startswith("--")]
    builderFlags = [x for x in sys.argv if x.startswith("--")]
    
    #####################################################################################
    # REBUILD OUR PARSERS
    #####################################################################################
    
    if "--parsersOnly" in builderFlags:
        sys.exit()
    
    base_files = [
        f"..{os.sep}src{os.sep}base{os.sep}base_assert.c",
        f"..{os.sep}src{os.sep}base{os.sep}base_except.c",
        f"..{os.sep}src{os.sep}base{os.sep}base_mem.c",
        f"..{os.sep}src{os.sep}base{os.sep}base_os.c",
        f"..{os.sep}src{os.sep}base{os.sep}base_types.c",
    ]
    
    thirdPartyLibraries = [
        "legacy_stdio_definitions",
    ]

    #####################################################################################
    # WINDOWS SPECIFIC LIBRARIES
    #####################################################################################    
    
    if os.name == "nt":
        thirdPartyLibraries += [
            "Advapi32", 
            "Msvcrt", 
            "shell32", 
            "opengl32",
            "glu32",
            "gdi32", 
            "user32", 
        ]
        
    #####################################################################################
    # Rebuild Our LIBRARIES
    #####################################################################################
    
    ourLibraries = [
        clang_library("base", "c11", base_files, "", [], [], ["../includes", "../src"]),
    ]
    
    linkDirs = ["../libraries/", "../build/"]
    includeDirs = ["../includes", "../src"]
    
    #####################################################################################
    # BUILD MAIN EXECUTABLE
    #####################################################################################
    
    ignoreFilesMain = ["test.c", "test.h", "test_", "mem_production.c", "import_test.c"]
    ignoreFilesLibraries = [x.split(os.sep)[-1] for x in base_files]
    allFilesMain = findFiles(f"..{os.sep}src", "c", ignoreFilesMain + ignoreFilesLibraries)
    
    clang("C2D", "c11", allFilesMain, [], thirdPartyLibraries + ourLibraries, linkDirs, includeDirs, True, True)
    
    #####################################################################################
    # BUILD TESTS
    #####################################################################################
    
    if "--test" in builderFlags:
        ignoreFilesTest = ["mem_production.c", "main.c"]
        ignoreFilesLibraries = [x.split(os.sep)[-1] for x in base_files+graphics_files + ["controller_camera.c", "noise.c"]]
        allFilesTest = findFiles(f"..{os.sep}src", "c", ignoreFilesTest + ignoreFilesLibraries)
        clang("tests", "c11", allFilesTest, [], thirdPartyLibraries + ourLibraries, linkDirs, includeDirs, True, True)
        os.system(f"..{os.sep}build{os.sep}tests.exe")
    
    #####################################################################################
    # INTERACTIVITY FLAGS
    #####################################################################################
    
    if "--testsOnly" in builderFlags:
        sys.exit()
    
    # Optionally run
    if "--run" in builderFlags:
        os.system(f"..{os.sep}build{os.sep}{BINARY}")

    #####################################################################################
    # FIX COMPILE_COMMANDS.JSON FOR LINUX (INTELLISENSE)
    #####################################################################################

    if os.name == "posix":
        with open("../compile_commands.json", "r") as rf:
            data = rf.read()
        data = data.replace("\\\\", os.sep)
        data = data.replace(f"C:{os.sep}", "/mnt/c/")
        with open("../compile_commands.json", "w") as wf:
            wf.write(data)
    

print(f"TOTAL TIME SPENT COMPILING: {total_time:.2f} SECONDS")