#ifndef DEFINES_H
#define DEFINES_H

#define StatusRed   "rgb(255,0,0)"
#define StatusGreen "rgb(0,128,0)"

/* Variables defines */
    #define Emulation   1
    #define Operational 2

    #define FullDebbuging    1
    #define ParcialDebbuging 2
    #define LightDebbuging   3

    #define ConsoleDebugOn  1
    #define ConsoleDebugOff 2


#define isRelease 1
#define debugMode ParcialDebbuging

#if isRelease == 1
    #define MODE_ Operational
    #define CONSOLEDEBUGMODE ConsoleDebugOff
#else
    #define MODE_ Emulation
    #if debugMode == FullDebbuging
        #define CONSOLEDEBUGMODE ConsoleDebugOn
    #elif debugMode == ParcialDebbuging
        #define DEBUGMODE ConsoleDebugOn
    #elif debugMode == LightDebbuging
        #define CONSOLEDEBUGMODE ConsoleDebugOn
    #endif
#endif
#endif // DEFINES_H
