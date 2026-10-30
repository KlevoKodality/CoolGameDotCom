@echo off
cmdkey /delete:git:https://github.com
git config --global --unset user.name
git config --global --unset user.email
echo git data cleared!
pause