#define MyAppName "sqlcw"
#define MyAppVersion "1.1"
#define MyAppURL "https://sourceforge.net/projects/sqlcw/"
#define MyAppExeName "sqlcw.exe"
#define ProjectDir "X:\Projects\sqlcw"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{0D0F4C50-15D5-4CDF-A7C7-316375B5359A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile={#ProjectDir}\doc\LICENSE_GPL-3.0.txt
; Remove the following line to run in administrative install mode (install for all users.)
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog commandline
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-win64-installer
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ChangesEnvironment=true

; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#ProjectDir}\bin\Release\sqlcw.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#ProjectDir}\doc\README.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#ProjectDir}\doc\examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
;Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\Examples"; Filename: "{app}\examples"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{group}\{#MyAppName} Readme"; Filename: "{app}\README.txt"

[Tasks]
Name: modifypath; Description: &Add application executable to environmental path

[Code]

// ----------------------------------------------------------------------------
//	modpath logic - should be at the end of the [Code] section
//
//	ModPathName defines the name of the task defined above
//	ModPathType() function defines whether the 'user' or 'system' path will
//		be modified; this will default to user if anything other than 'system'
//    is set
//	setArrayLength must specify the total number of dirs to be added
//	Result[0] contains first directory, Result[1] contains second, etc.
// ----------------------------------------------------------------------------
const
	ModPathName = 'modifypath';

function ModPathType(): String;
  begin
    if IsAdminInstallMode() then begin
      Result := 'system';
    end else begin
      Result := 'user';
    end;
  end;

function ModPathDir(): TArrayOfString;
	begin
		setArrayLength(Result, 1);
		Result[0] := ExpandConstant('{app}\bin');
	end;
#include "modpath.iss"
