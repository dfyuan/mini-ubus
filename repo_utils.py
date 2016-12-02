import sys
import os
import subprocess

def inred(s):
	return"%s[31;2m%s%s[0m"%(chr(27), s, chr(27))

def ingreen(s):
	return"%s[32;2m%s%s[0m"%(chr(27), s, chr(27))

def system_excu(cmd):
	ret = os.system(cmd)
	if ret != 0:
		print inred(cmd + " " + "failed")
		print os.getcwd()
		#exit()

def repo_status(repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd();
			os.chdir(repo_path)
			ret_buf = subprocess.check_output("git status -s", shell=True)
			if ret_buf != "":
				print ingreen(repo_path)
				print ret_buf,
			os.chdir(old_path)

all_branches = {}

class BranchInfo(object):
	def __init__(self, branch_name):
		self.branch_name = branch_name
		self.repo_list = []

def update_branch_info(branch_name, repo_name):
	if branch_name not in all_branches:
		all_branches[branch_name] = BranchInfo(branch_name)
	all_branches[branch_name].repo_list.append(repo_name)

def pretty_branchinfo_output():
	for item in all_branches:
		print ingreen("branch : %s" % item)
		for repo_item in all_branches[item].repo_list:
			print "            %s" % repo_item
		print

def repo_branch(repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd();
			os.chdir(repo_path)
			# print ingreen(repo_path)
			#os.system("git branch")
			ret_buf = subprocess.check_output("git branch | grep '*'", shell=True)
			update_branch_info(ret_buf.split(' ')[-1], repo_path.split('/')[-1])
			os.chdir(old_path)

	pretty_branchinfo_output()

def repo_clean(build_dir):
	for item in build_dir:
		if os.path.exists(item):
			old_path = os.getcwd();
			os.chdir(item)
			system_excu("make clean")
			os.chdir(old_path)

def repo_push(remote, repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd();
			os.chdir(repo_path)
			print ingreen(repo_path)
			system_excu("git push " + remote + " master:master")
			os.chdir(old_path)

def repo_build(build_dir, platform, build_type='debug'):
	for item in build_dir:
		if os.path.exists(item):
			old_path = os.getcwd();
			os.chdir(item)
			print ingreen(item + " is building")
#			system_excu("make clean");
			command = "make PLATFORM=" + platform + " " + "BUILD_TYPE=" + build_type
			system_excu(command);
			os.system("make PLATFORM=" + platform + " install")
			os.chdir(old_path)

def repo_init(repo_url, repo_manifest):
	for item in repo_manifest:
		if not os.path.exists(item[0]):
			os.mkdir(item[0])
		old_path = os.getcwd()
		os.chdir(item[0])
		if not os.path.exists(item[1][:-4]):
			print repo_url + item[1]
			os.system("git clone " + repo_url + item[1])
		else:
			print inred(item[1] + " already exists, PLS make sure you need re-down it...")
		os.chdir(old_path)

def repo_sync(remote, repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd()
			os.chdir(repo_path)
			print ingreen(repo_path)
			system_excu("git checkout master");
			system_excu("git fetch " + remote);
			system_excu("git merge " + remote + "/master")
			os.chdir(old_path)

def repo_checkout(branch, repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd()
			os.chdir(repo_path)
			print ingreen(old_path + "/" + repo_path)
			system_excu("git checkout " + branch);
			os.chdir(old_path)

def repo_forall(command, repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd()
			os.chdir(repo_path)
			print ingreen(repo_path)
			system_excu(command);
			os.chdir(old_path)
		else:
			print repo_path + " not exist"

def repo_remote(remote_name, remote_addr, repo_manifest):
	for item in repo_manifest:
		# rm the .git at each string tail
		repo_path = os.path.join(item[0], item[1][:-4])
		if os.path.exists(repo_path):
			old_path = os.getcwd()
			os.chdir(repo_path)
			print ingreen(repo_path)
			remote_path = os.path.join(remote_addr, item[1])
			command = "git remote add " + remote_name + " " + remote_path
			print command;
			system_excu(command);
			os.chdir(old_path)