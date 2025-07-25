# Bright ANSI Colors
BRIGHT_RED="\[\033[1;91m\]"
BRIGHT_GREEN="\[\033[1;92m\]"
BRIGHT_YELLOW="\[\033[1;93m\]"
BRIGHT_BLUE="\[\033[1;94m\]"
BRIGHT_MAGENTA="\[\033[1;95m\]"
BRIGHT_CYAN="\[\033[1;96m\]"
RESET="\[\033[0m\]"

bash /rainbow.sh
echo "SalernOS Kernel: Copyright (C) 2021 - 2025 Alessandro Salerno (GPL v3)"
echo "SalernOS: Copyright (C) 2021 - 2025 Alessandro Salerno (GPL v3)"
echo "flanterm: Copyright (C) 2022 - 2025 mintsuki and contributors (BSD 2-clause)"
echo "limine: Copyright (C) 2019 - 2025 mintsuki and contributors (BSD 2-clause)"
echo "mlibc: Copyright (C) 2015 - 2025 mlibc contributors (MIT)"
echo ""
echo "This distribution comes with GNU software. Copyright (C) Free Software Foundation, Inc."
echo ""
echo "Welcome to SalernOS! Thanks for checking out my project!"
echo ""

# Prompt format: user@host dir  →
export PS1="${BRIGHT_RED}root${BRIGHT_MAGENTA}@${BRIGHT_YELLOW}\h ${BRIGHT_GREEN}\w  ${BRIGHT_CYAN}→ ${RESET}"

