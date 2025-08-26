# Bright ANSI Colors
BRIGHT_RED="\[\033[1;91m\]"
BRIGHT_GREEN="\[\033[1;92m\]"
BRIGHT_YELLOW="\[\033[1;93m\]"
BRIGHT_BLUE="\[\033[1;94m\]"
BRIGHT_MAGENTA="\[\033[1;95m\]"
BRIGHT_CYAN="\[\033[1;96m\]"
RESET="\[\033[0m\]"

bash /rainbow.sh

# Prompt format: user@host dir  →
export PS1="${BRIGHT_RED}root${BRIGHT_MAGENTA}@${BRIGHT_YELLOW}\h ${BRIGHT_GREEN}\w  ${BRIGHT_CYAN}→ ${RESET}"

eval "$(dircolors -b /.dircolors)"
alias ls="ls --color=auto"
alias python="PYTHONHASHSEED=0 python"
