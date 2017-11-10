git remote set-url origin https://bitbucket.org/h3nd24/CCD.git
git remote set-url --push origin https://github.com/h3nd24/BroCCaDe.git
git pull
git filter-branch -f --prune-empty --tree-filter "$(cat blacklist)" master
git push -u origin master
