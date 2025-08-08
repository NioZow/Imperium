#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "pydantic",
# ]
# ///

import argparse
import os
import re

from pydantic import BaseModel, field_validator


class MemOp(BaseModel):
    operation: str
    dst: str
    src: str
    file: str
    function: str
    line: int

    @field_validator("operation", mode="after")
    @classmethod
    def validate_operation(cls, op):
        if op not in ("free", "alloc", "realloc"):
            raise ValueError("operation must be free, alloc or realloc")
        return op


def print_memory_leaks(cmd_output: str):
    # compile the regex pattern
    pattern = re.compile(
        r"(free|alloc|realloc)(?: to (0x[0-9A-F]{8}))?(?: from (0x[0-9A-F]{8}))? at ([^:]+):([^:]+):(\d+)"
    )

    # find all matches in the output
    matches = pattern.findall(cmd_output)

    # print each match
    while len(matches) > 0:
        match = matches.pop(0)
        mem_op = MemOp(
            operation=match[0],
            dst=match[1],
            src=match[2],
            file=match[3],
            function=match[4],
            line=int(match[5]),
        )

        # free on empty memory
        if mem_op.operation == "free":
            print(
                f"Free on non-allocated memory ({mem_op.src}) : {mem_op.file}::{mem_op.function}::{mem_op.line}"
            )
            continue

        # seek where that address gets freed
        i = 0
        while i < len(matches):
            mem_op_cmp = MemOp(
                operation=matches[i][0],
                dst=matches[i][1],
                src=matches[i][2],
                file=matches[i][3],
                function=matches[i][4],
                line=int(matches[i][5]),
            )

            # check if that address ever gets freed
            if mem_op.dst == mem_op_cmp.src:
                if mem_op_cmp.operation == "free":
                    # it gets freed through a free op
                    del matches[i]

                # the other possibility is that it gets
                # freed by a realloc op
                # we will parse this case later
                break

            i += 1
        else:
            # never freed
            print(f"Memory leak : {mem_op.file}::{mem_op.function}::{mem_op.line}")


def main():
    parser = argparse.ArgumentParser(description="Memory leak checker")

    group = parser.add_mutually_exclusive_group(required=True)

    group.add_argument("--exec", "-e", help="File to execute")
    group.add_argument("--file", "-f", help="File to read output from")

    args = parser.parse_args()

    # check which option was provided
    output = ""
    if args.exec:
        # run the process and read the output
        with os.popen(args.exec) as proc:
            output = proc.read()
    elif args.file:
        with open(args.file) as file:
            output = file.read()

    print_memory_leaks(output)


if __name__ == "__main__":
    main()
