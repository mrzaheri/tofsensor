import git_describe
import os
import shutil
import sys
import subprocess
import time

def copy_file( input, output ):
    if os.path.isfile(input):
        try:
            shutil.copy2( input, output )
        except WindowsError:
            os.remove( output )
            shutil.copy2( input, output )

project_dir = sys.argv[1]
configuration_name = sys.argv[2]
project_name = sys.argv[3]
build_output_dir_name = None
if( len(sys.argv) > 4 ):
    build_output_dir_name = sys.argv[4]

git_description_cleaned, branch_type, product_name, version_main, version_minor, version_micro, head_distance, sha_short, commit_year, commit_month, commit_day = git_describe.get_git_info()
output_file_name = product_name + "-" + configuration_name + "_V"+ version_main + "." + version_minor + "." + version_micro + "_D" + commit_year + "-" + commit_month + "-" + commit_day + "_g" + sha_short
output_folder = os.path.join( project_dir, "out" )
if build_output_dir_name:
    input_folder = os.path.join( project_dir, build_output_dir_name )
else:
    input_folder = os.path.join( project_dir, project_name + " " + configuration_name )
#always start with a clean output_folder:
if os.path.exists( output_folder ):
    shutil.rmtree(output_folder)
    time.sleep(0.0001) # to prevent permision denied error on mkdir command
os.mkdir(output_folder)


# copy and rename files:
extensions = ["bin", "map", "elf"]
for ext in extensions:
    file_name_in = os.path.join( input_folder, project_name + "." + ext )
    file_name_out = os.path.join( output_folder, output_file_name + "." + ext )
    copy_file( file_name_in, file_name_out )
    file_name_out = os.path.join( output_folder, project_name + "." + ext )
    copy_file( file_name_in, file_name_out )
