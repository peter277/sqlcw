[Setup]
AppName=ModPath Test
AppVerName=ModPath Test 1.0
AppVersion=1.0
AppPublisher=Jared Breland
AppPublisherURL=http://www.legroom.net/software
AppSupportURL=http://www.legroom.net/software
AppUpdatesURL=http://www.legroom.net/software
DefaultDirName={userappdata}\ModPath Test
DisableDirPage=true
DisableProgramGroupPage=true
DisableReadyPage=true
OutputBaseFilename=testpath
OutputDir=.\
SourceDir=.\
Compression=lzma2/ultra
InternalCompressLevel=ultra
SolidCompression=true
VersionInfoVersion=1.0
VersionInfoCompany=Jared Breland
VersionInfoCopyright=GNU General Public License v2
VersionInfoDescription=Package for ModPath Test
ChangesEnvironment=true
ChangesAssociations=no
AllowUNCPath=false
AllowNoIcons=true
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=lowest

[Tasks]
Name: modifypath; Description: &Add application directory to your system path
Name: dummy; Description: &Dummy task just to test multiple task handling

[Files]
Source: testpath.iss; DestDir: {app}; Flags: ignoreversion

[Icons]
Name: {userdesktop}\{cm:UninstallProgram,ModPath Test}; Filename: {uninstallexe}; Parameters: /silent

[Code]
const
	ModPathName = 'modifypath';
	ModPathType = 'user';

function ModPathDir(): TArrayOfString;
begin
	setArrayLength(Result, 2)
	Result[0] := ExpandConstant('{app}');
	Result[1] := ExpandConstant('{app}\bin');
end;
#include "modpath.iss"
