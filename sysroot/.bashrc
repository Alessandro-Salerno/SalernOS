# Bright ANSI Colors
BRIGHT_RED="\[\033[1;91m\]"
BRIGHT_GREEN="\[\033[1;92m\]"
BRIGHT_YELLOW="\[\033[1;93m\]"
BRIGHT_BLUE="\[\033[1;94m\]"
BRIGHT_MAGENTA="\[\033[1;95m\]"
BRIGHT_CYAN="\[\033[1;96m\]"
RESET="\[\033[0m\]"

# ANSI colors (no \[ \] — for non-PS1 output)
ANSI_RED="\033[1;91m"
ANSI_ORANGE="\033[38;5;208m"
ANSI_YELLOW="\033[1;93m"
ANSI_GREEN="\033[1;92m"
ANSI_BLUE="\033[1;94m"
ANSI_INDIGO="\033[38;5;57m"
ANSI_VIOLET="\033[1;95m"
ANSI_RESET="\033[0m"

# Prompt-safe versions (\[ \] around them)
PS1_RED="\[\033[1;91m\]"
PS1_ORANGE="\[\033[38;5;208m\]"
PS1_YELLOW="\[\033[1;93m\]"
PS1_GREEN="\[\033[1;92m\]"
PS1_BLUE="\[\033[1;94m\]"
PS1_INDIGO="\[\033[38;5;57m\]"
PS1_VIOLET="\[\033[1;95m\]"
PS1_RESET="\[\033[0m\]"

# Prompt format: user@host dir  →
export PS1="${BRIGHT_RED}root${BRIGHT_MAGENTA}@${BRIGHT_YELLOW}\h ${BRIGHT_GREEN}\w  ${BRIGHT_CYAN}→ ${RESET}"

