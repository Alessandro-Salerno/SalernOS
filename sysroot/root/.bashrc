# Bright ANSI Colors
BRIGHT_RED="\[\033[1;91m\]"
BRIGHT_GREEN="\[\033[1;92m\]"
BRIGHT_YELLOW="\[\033[1;93m\]"
BRIGHT_BLUE="\[\033[1;94m\]"
BRIGHT_MAGENTA="\[\033[1;95m\]"
BRIGHT_CYAN="\[\033[1;96m\]"

# Muted ANSI Colors
TEAL="\[\033[0;38;5;23m\]"
AMBER="\[\033[0;38;5;130m\]"
SLATE="\[\033[0;38;5;24m\]"
GRAY="\[\033[0;38;5;244m\]"
RESET="\[\033[0m\]"

eval "$(dircolors -b /.dircolors)"

sync_winsize() {
    case "${TERM}" in
        xterm* | rxvt* | screen*) ;;
        *) return ;;
    esac

    command -v resize > /dev/null 2>&1 || return

    _winsize="$(resize 2> /dev/null)"
    if [ -n "${_winsize}" ]; then
        eval "${_winsize}"
    fi
    unset _winsize
}

export_winsize() {
    export COLUMNS LINES
}

sync_winsize
export_winsize
PROMPT_COMMAND="export_winsize"

alias fixsize="sync_winsize; export_winsize"

export PS1="${TEAL}\w ${AMBER}=>${RESET} "
export PS2="${GRAY}..${RESET} "

export EDITOR="vi"
export PAGER="less"
export LESS="-R"

shopt -s checkwinsize
shopt -s histappend

export HISTSIZE=2000
export HISTFILESIZE=2000
export HISTCONTROL="ignoreboth"

alias ls="ls --color=auto"
alias ll="ls -lh --color=auto"
alias la="ls -lha --color=auto"
alias grep="grep --color=auto"
alias python="PYTHONHASHSEED=0 python"
alias xeyes="DISPLAY=:0 xeyes"
alias perf="time /root/tests/runmany 500 /root/tests/testpipe 2048"
