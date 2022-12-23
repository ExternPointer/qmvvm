#pragma once


class CommandLineArgs {
public:
    inline static void Setup( int argc, char** argv ) {
        s_argc = argc;
        s_argv = argv;
    }

    inline int& GetArgc() {
        return s_argc;
    }

    inline char** GetArgv() {
        return s_argv;
    }

private:
    inline static int s_argc;
    inline static char** s_argv;
};