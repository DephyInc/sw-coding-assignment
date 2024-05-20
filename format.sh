FORMAT=clang-format-12
find . -type f \
    \( -iname '*.h' -o -iname '*.c' -o -iname '*.cc' \) \
| xargs -t $FORMAT -i
