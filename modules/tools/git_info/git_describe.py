import subprocess
import re

def get_git_commit_date():
    output = str(subprocess.check_output(['git', 'show', '-s', '--format=%ci']))
    match = re.search(r"b'(?P<output_str>[a-z,A-Z,0-9,/,.,-]+)", output)
    if match:
        return match.group('output_str')
    else:
        return False
    
def get_git_sha_short():
    try:
        output = str(subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']))
        match = re.search(r"b'(?P<output_str>[a-z,A-Z,0-9,/,.,-]+)", output)
    except:
        return "no_git_repo"

    if match:
        return match.group('output_str')
    else:
        return False

def get_git_description():
    #Out is in format of TAG-0-g6781eea. Tag is in format of debug/rov/00.01.07
    try:
        output = str(subprocess.check_output(['git', 'describe', '--dirty', '--always', '--abbrev=7', '--tags', '--long']))
        match = re.search(r"b'(?P<output_str>[a-z,A-Z,0-9,/,.,-]+)", output)
        if match:
            return match.group('output_str')
        else:
            return False
    except:
        return False

def get_git_info():
    regex = r"(?P<brnach_type>\w+)/(?P<product_name>[a-z,A-Z,0-9,/,.,-]+)/(?P<version_main>[0-9]{2,4})\.(?P<version_minor>[0-9]{2})\.(?P<version_micro>[0-9]{2})-(?P<head_distance>[0-9])-g(?P<sha_short>\w{7})(?P<is_dirty>(-dirty)?)"
    git_description = get_git_description()
    if git_description:
        matches = re.search(regex, git_description)
        match_date = re.search(r"(?P<commit_year>[0-9]{4})-(?P<commit_month>[0-9]{2})-(?P<commit_day>[0-9]{2})", get_git_commit_date())
    else:
        matches = False
        match_date = False
        git_description = "invalid"
    dirty_load = False
    if matches and match_date:
        branch_type = matches.group('brnach_type')
        product_name = matches.group('product_name')
        version_main = matches.group('version_main')
        version_minor = matches.group('version_minor')
        version_micro = matches.group('version_micro')
        head_distance = matches.group('head_distance')
        is_dirty = matches.group('is_dirty')
        commit_year = match_date.group('commit_year')
        commit_month = match_date.group('commit_month')
        commit_day = match_date.group('commit_day')
        if int(head_distance) != 0 or is_dirty != '':
            dirty_load = True
    else:
        dirty_load = True

    if dirty_load:
        branch_type = 'xxx'
        product_name = 'xxx'
        version_main = '00'
        version_minor = '00'
        version_micro = '00'
        head_distance = '0'
        commit_year = '00'
        commit_month = '00'
        commit_day = '00'
        if matches:
            branch_type = matches.group('brnach_type')
            product_name = matches.group('product_name')

        if match_date:
            commit_year = match_date.group('commit_year')
            commit_month = match_date.group('commit_month')
            commit_day = match_date.group('commit_day')

    sha_short = get_git_sha_short()
    git_description_cleaned = git_description.replace("/","-")
    return git_description_cleaned, branch_type, product_name, version_main, version_minor, version_micro, head_distance, sha_short, commit_year, commit_month, commit_day

