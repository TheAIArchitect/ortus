#!/usr/bin/python
import os
from operator import itemgetter
from math import isnan

wbp_files = []
wbpm_files = []
skipped_params = 0
skipped_metas = 0
old_dir = os.path.dirname(os.path.realpath(__file__))

def isDesirable(utility):
	if isnan(float(utility)):
		return False
	if float(utility) > 850.0:
		return False
	return True


for (dpath, dnames, fnames) in os.walk(old_dir):
	if (old_dir.__eq__(dpath)):
		for fname in fnames:
			if fname[:21].__eq__("worm_body_params_meta"):
				continue
				#print "nice!: "+fname
				#wbpm_files.append(fname)
			elif fname[:17].__eq__("worm_body_params_"):
				#print "good.: "+fname
				wbp_files.append(fname)
	else:
		break

# this will make sure they're aligned
#wbpm_files.sort()
wbp_files.sort()

num_files = len(wbp_files)

merged_params = []
merged_metas = []
last_param_num = 0 # start at 1
total_entries = 0
last_old_id = "NOTHING" 

for i in range(0, num_files):
	print wbp_files[i]
	params = open(wbp_files[i],'r')
	new_ids = {} 
	num_entries = 0
	for param in params:
		sparams = param.split(',')
		if (not isDesirable(sparams[1])):
			skipped_params += 1
			continue
		if (sparams[0].__eq__(last_old_id)): # there is a duplicate
			if (float(merged_params[num_entries-1]) <= float(sparams[1])): # the first duplicate is better, ignore this one
				continue
			else: # the second duplicate is better, let's forget about it and use the second
				del new_ids[merged_params[num_entries-1]]
				merged_params.pop()
				num_entries -= 1
				last_param_num -= 1
		last_param_num += 1
		sparams[0] = str(last_param_num)
		new_ids[sparams[0]] = float(sparams[1])
		merged_params.append(sparams)
		num_entries += 1
	if (num_entries < 1):
		continue
	total_entries += num_entries
	keys = new_ids.keys()
	for key in keys:
		#if (len(metas[j]) == 0):
		#	continue
		merged_metas.append([key,new_ids[key]])
	params.close()


merged_metas.sort(key=itemgetter(1)) #,reverse = True)

#for mp in merged_params:
	#print mp[0],mp[1]
#print "********"
#print merged_metas

new_param_f = open("merged_wbp.txt",'w')
new_param_meta_f = open("merged_wbp_meta.txt",'w')

new_param_meta_f.write(str(total_entries)+"\n")

for i in range(0, total_entries):
	new_param_f.write(str(",".join(merged_params[i][:-1])).strip()+"\n")
	new_param_meta_f.write(str(merged_metas[i][0])+","+str(merged_metas[i][1])+"\n")
	
new_param_f.close()
new_param_meta_f.close()
