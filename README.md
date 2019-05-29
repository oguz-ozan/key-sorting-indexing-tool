# key-sorting-indexing-tool
## Introduction

This is a simple key-sorting indexing tool, which creates an index file of a binary data file, with the properties of given JSON file.

## Using

It displays a menu, in which you can select
1- Parsing JSON file
2- Creating index file
3- Search with value(returns index of that record)
4- Free memory and exit

JSON File parameters can be:

dataFileName: [some file path] (file to be indexed)
indexFileName: [some empty or non-existing file path] (index file)
recordLength: [some integer value] (this is simply needed, when we read the file as records)
keyEncoding: BIN or CHR (this determines how keys are interpreted and sorted)
keyStart: [some integer value] (index starting position in a record)
keyEnd: [some integer value] (index ending position in a record)
order: ASC or DESC (ordering of values)
