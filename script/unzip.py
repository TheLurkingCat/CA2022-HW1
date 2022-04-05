import os
import shutil
cwd = os.getcwd()
hw1zip = os.path.join(cwd, "1102.5281-HW1 - Particle system-263670.zip")
hw1folder = os.path.join(cwd, "HW1")
reportfolder = os.path.join(hw1folder, "report")
codefolder = os.path.join(hw1folder, "code")
shutil.unpack_archive(hw1zip, hw1folder)
try:
    os.mkdir(reportfolder)
    os.mkdir(codefolder)
except FileExistsError:
    pass
for filename in os.listdir(hw1folder):
    currentid = filename.split()[0]
    if filename.endswith("pdf"):
        shutil.move(os.path.join(hw1folder, filename), os.path.join(reportfolder, currentid + ".pdf"))
    elif filename.endswith("zip"):
        stufolder = os.path.join(codefolder, currentid)
        try:
            os.mkdir(stufolder)
        except FileExistsError:
            pass
        shutil.unpack_archive(os.path.join(hw1folder, filename), stufolder)
        os.remove(os.path.join(hw1folder, filename))
