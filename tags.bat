@echo off
ctags -R --extra=+f --exclude=scratch *
del csearchindex
set CSEARCHINDEX=csearchindex
cindex .
