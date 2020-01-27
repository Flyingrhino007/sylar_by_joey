# .bashrc

# User specific aliases and functions

alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'
alias vctags='ctags -R --c-kinds=+p --fields=+aS --extra=+q'
alias cvim='LC_ALL=C vim'

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi
