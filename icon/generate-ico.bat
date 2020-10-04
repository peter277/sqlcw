
rem Using ImageMagick to generate ICO from SVG
rem See: https://superuser.com/questions/491180/how-do-i-embed-multiple-sizes-in-an-ico-file

convert workflow.svg -define icon:auto-resize=16,32,48,256 -colors 256 -transparent white out.ico
rem convert workflow.svg -define icon:auto-resize=16,32,48,256 -compress zip out.ico

pause
