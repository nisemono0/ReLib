#!/bin/bash

CLANGD_CONF='CompileFlags:
  Remove: [-mno-direct-extern-access]
Diagnostics:
    MissingIncludes: None
    UnusedIncludes: None
CompileFlags:
    CompilationDatabase: "build"'

rm -f ./.clangd
echo "$CLANGD_CONF" > .clangd

echo "[!!!] .clangd file created (for lsp)"

