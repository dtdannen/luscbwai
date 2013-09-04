# git_add_script.sh
#
# Author: Dustin Dannenhauer
#
# This file runs a series of add and rm commands ensuring that any new files
# are automatically added, and that certain files never get added. Basically
# use this script to add every file, and then remove the ones that need to 
# always be removed. Note: they are only being removed from the git repo, not locally.

# now add all files
git add --all
git add git_add_script.sh # can I git add myself?

# now remove unnecessary files
#git rm --cached core/StarcraftBot/BWSAL_0.9.12/Debug/*
#git rm --cached core/StarcraftBot/BWSAL_0.9.12/HierarchicalGDABot.sdf
#git rm --cached core/StarcraftBot/GameStateOutput/*
#clear
echo "--- finished adding files ---"
