@REM "5i11tsutsuiminoruh801.c"をコンパイルするためのバッチファイルです。
@REM 本バッチはGDLから使用されるものではありません。
@REM GDLを使用しないで単体でコンパイルする際に利用してください。
@SET PATH=C:\PROGRA~2\BestTech\GCCDEV~1\GCC\H8\bin;c:\PROGRA~2\BestTech\GCCDEV~1\GCC\H8\libexec\gcc\H8300-~1\47-GNU~1.02;c:\PROGRA~2\BestTech\GCCDEV~1\GCC\H8\H8300-~1\bin
h8300-elf-gcc -I C:\PROGRA~2\BestTech\GCCDEV~1\TARGET\3052F -L C:\PROGRA~2\BestTech\GCCDEV~1\TARGET\3052F -mh -nostartfiles -fsigned-char -Wall -Wno-main -pipe -O0 -TC:\PROGRA~2\BestTech\GCCDEV~1\TARGET\3052F\h8rom.x -Xlinker -Map -Xlinker "5i11tsutsuiminoruh801.map" C:\PROGRA~2\BestTech\GCCDEV~1\TARGET\3052F\h8crt0.s "5i11tsutsuiminoruh801.c" -ladd3052 -lm -o "5i11tsutsuiminoruh801.exe" 
h8300-elf-objcopy -O srec  "5i11tsutsuiminoruh801.exe" "5i11tsutsuiminoruh801.mot"
PAUSE
