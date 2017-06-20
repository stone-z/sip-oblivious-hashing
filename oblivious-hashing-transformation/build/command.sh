#!/bin/bash
opt-3.9 -load /usr/local/lib/libInputDependency.so -load lib/libOHTransformation.so -oh-transform-pass bubble_out.bc > /dev/null
