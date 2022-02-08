import git_describe
import os
import sys

git_description_cleaned, branch_type, product_name, version_main, version_minor, version_micro, head_distance, sha_short, commit_year, commit_month, commit_day = git_describe.get_git_info()

project_dir = sys.argv[1]
build_config_name = sys.argv[2]

# generate git_describe.h file in project directory
f = open(os.path.join(project_dir, 'git_describe.h'), 'w')
f.write('/* This is an auto generated file by git-describe.py */\n\n')
f.write('#ifndef _GIT_DESCRIBE_H\n')
f.write('#define _GIT_DESCRIBE_H\n\n')
f.write('/*****************************************************************/\n\n')
f.write('#define GIT_VERSION_MAIN              '+str(int(version_main))+'\n\n')
f.write('#define GIT_VERSION_MINOR             '+str(int(version_minor))+'\n\n')
f.write('#define GIT_VERSION_MICRO             '+str(int(version_micro))+'\n\n')
f.write('#define GIT_SHA                       "'+sha_short+'"\n\n')
f.write('#define GIT_FULL_DESCRIPTION          "'+git_description_cleaned+'"\n\n')
f.write('#define GIT_COMMIT_YEAR               '+str(int(commit_year))+'\n\n')
f.write('#define GIT_COMMIT_YEAR_2_DIGITS      '+str(int(commit_year) % 100)+'\n\n')
f.write('#define GIT_COMMIT_MONTH              '+str(int(commit_month))+'\n\n')
f.write('#define GIT_COMMIT_DAY                '+str(int(commit_day))+'\n\n')
f.write('#define BUILD_CONFIG_NAME             "'+build_config_name+'"\n\n')
f.write('#endif\n')
f.close()
