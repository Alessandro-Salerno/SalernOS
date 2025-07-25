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

# Rainbow line drawer — pure Bash, clean, safe
draw_rainbow() {
    local width=$(tput cols)
    local seg=$((width / 7))
    local rem=$((width % 7))
    local block=""
    local i

    # Build 1 segment of width 'seg'
    for ((i = 0; i < seg; i++)); do
        block+="█"
    done

    # Print 7 segments with color
    printf "${ANSI_RED}%s"   "$block"
    printf "${ANSI_ORANGE}%s" "$block"
    printf "${ANSI_YELLOW}%s" "$block"
    printf "${ANSI_GREEN}%s" "$block"
    printf "${ANSI_BLUE}%s"   "$block"
    printf "${ANSI_INDIGO}%s" "$block"
    # Final segment includes any leftover space
    for ((i = 0; i < seg + rem; i++)); do
        printf "${ANSI_VIOLET}█"
    done
    printf "${ANSI_RESET}\n"
}

draw_rainbow
