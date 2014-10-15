#!/bin/sh

rsync -avPc . amand@ananas:Code/hypnomix/ --exclude=*.o --exclude=*.so --exclude=*.P --exclude=*.swp --exclude=*.vis
