#!/usr/bin/env python3

env = Environment()
env.Append(CPPFLAGS=["-O3", "-fno-omit-frame-pointer", "-g", "-ggdb", "-Wall", "-fopenmp"])
env.Program("main", Glob("*.cc") + Glob("*.h"), LIBS=['stxxl_debug', "pthread", "gomp"], LIBPATH='.')
