# P4G Sound Fixer

Helper library to patch Persona 4 Golden to fix audio issues (where audio plays in cutscenes but not elsewhere)

See [this steam community page for details on the issue](https://steamcommunity.com/app/1113000/discussions/0/2441462389164687905/)

Based on [pampersrocker/DInput8HookingExample](https://github.com/pampersrocker/DInput8HookingExample)

## Root cause
P4G opens audio files with the flag `FILE_FLAG_NO_BUFFERING`.
[Microsoft Docs on the flag](https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering): When opening or creating a file with the CreateFile function, the FILE_FLAG_NO_BUFFERING flag can be specified to disable system caching of data being read from or written to the file. Although this gives complete and direct control over data I/O buffering, **in the case of files and similar devices there are data alignment requirements that must be considered**.

P4G passes these files off to the XACT3 audio engine, which consistently reads 512-byte aligned chunks.  These chunks are *not* 4K aligned.  This means reads will fail on drives with 4K blocks.

[The official XACT docs recommended use of this flag](https://web.archive.org/web/20080916225210/http://msdn.microsoft.com/en-us/library/bb172331(VS.85).aspx), so P4G might not be the only game with this issue.  The patcher will probably work with any other similar games.
