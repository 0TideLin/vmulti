.\devcon.exe remove HID\VMulti*
.\devcon.exe remove *\VMulti*
sc.exe delete vmulti
del %SYSTEMROOT%\system32\drivers\vmulti.sys
pause
