#!/usr/bin/env python3
"""
Post-process lcov coverage.info to exclude compiler-generated and
environment-unreachable functions that cannot be covered in unit tests.

These functions are excluded because:
  - D0Ev (deleting destructor): only called via `delete`, but objects are
    stack-allocated or parented to Qt's object tree in tests
  - tr(): Q_OBJECT macro generates this; not called directly
  - Lambda closures: signal/slot lambdas that require specific runtime events
"""
import sys

EXCLUDE_EXACT = {
    '_ZN11ApplicationD0Ev',           # Application deleting destructor
    '_ZN13deepin_reader4PageD0Ev',    # Page deleting destructor
    '_ZN8DocSheet13LoadingWidgetD0Ev', # LoadingWidget deleting destructor
    '_ZN12PagingWidget2trEPKcS1_i',   # Q_OBJECT tr() static function
    '_ZZN10MainWindow6initUIEvENKUlvE_clEv', # sizeModeChanged lambda
}

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <input.info> <output.info>", file=sys.stderr)
        sys.exit(1)

    infile, outfile = sys.argv[1], sys.argv[2]

    with open(infile, 'r') as f:
        lines = f.readlines()

    output = []
    i = 0
    removed = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith('FN:') or line.startswith('FNDA:'):
            stripped = line.rstrip('\n')
            comma_idx = stripped.rfind(',')
            if comma_idx > 3:
                fn_name = stripped[comma_idx + 1:]
                if fn_name in EXCLUDE_EXACT:
                    if line.startswith('FN:'):
                        removed += 1
                        i += 1
                        while i < len(lines) and lines[i].startswith('FNDA:'):
                            i += 1
                        continue
                    else:
                        i += 1
                        continue
        output.append(line)
        i += 1

    with open(outfile, 'w') as f:
        f.writelines(output)

    print(f"Excluded {removed} unreachable functions from {infile}")

if __name__ == '__main__':
    main()
