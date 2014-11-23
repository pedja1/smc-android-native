;;
;;  german.nsh
;;
;;  German language strings for the Windows SMC NSIS installer.
;;  Windows Code page: 1252
;;
;;  Version 1
;;  Author: FluXy
;;




; Startup checks
!define SMC_INSTALLER_IS_RUNNING	"Der ${PRODUCT_NAME} Installer l�uft schon."
!define SMC_ALREADY_INSTALLED		"${PRODUCT_NAME} ist schon installiert und sollte entfernt werden.$\n�ber eine schon installierte Version zu installieren wird nicht unterst�tzt und kann unvorhergesehenes verhalten verursachen.$\n$\nEntferne die alte Version bevor ${PRODUCT_VERSION} installiert wird ?"
!define SMC_IS_RUNNING				"Eine Instanz von SMC l�uft momentan. Beenden Sie SMC und versuchen Sie es nochmal."
 
; Components Page
!define SMC_CORE_SECTION_TITLE					"Kern (erforderlich)"
!define SMC_CORE_SECTION_DESCRIPTION			"SMC Kern und daten"
!define SMC_SOURCE_CODE_SECTION_TITLE			"Quellcode"
!define SMC_SOURCE_CODE_SECTION_DESCRIPTION		"SMC Quellcode welcher f�r C++ Entwickler gedacht ist"
!define SMC_STARTMENU_SHORTCUT_SECTION_TITLE	"Startmen� Verkn�pfungen"
!define SMC_STARTMENU_SHORTCUT_DESC				"Erstellt einen Eintrag f�r SMC im Startmen�"
!define SMC_DESKTOP_SHORTCUT_SECTION_TITLE		"Desktop Verkn�pfung"
!define SMC_DESKTOP_SHORTCUT_DESC				"Erstellt eine Verkn�pfung zu SMC auf dem Desktop"

; Installer Finish Page
!define SMC_FINISH_WEB_PAGE_INFO		"Auf der Webseite (www.secretmaryo.org) kannst du nachschauen ob du die aktuellste SMC Version hast, lernen wie man den Editor benutzt oder spreche mit anderen Spielern im Forum. Mit einer Spende kannst du auch die Entwicklung von SMC zu unterst�tzen :)"
!define SMC_FINISH_VEW_CONTROLS			"Zeige Tastenzuordnung"
!define SMC_FINISH_VEW_CONTROLS_INFO	"Zeige die Standard-Tastenzuordnung f�r alle Spielaktionen."
!define SMC_FINISH_DONATE_INFO			"Spende um die Entwicklung zu unterst�tzen :)"

; Shortcuts
!insertmacro SMC_MACRO_DEFAULT_STRING SMC_SHORTCUT_WEB_PAGE		"Webseite"
!insertmacro SMC_MACRO_DEFAULT_STRING SMC_SHORTCUT_CONTROLS		"Tastenzuordnung"
!insertmacro SMC_MACRO_DEFAULT_STRING SMC_SHORTCUT_DONATE		"Spenden"
!insertmacro SMC_MACRO_DEFAULT_STRING SMC_SHORTCUT_UNINSTALL	"Deinstallieren"


; Uninstall Section Prompts
!define un.SMC_KEEP_USER_DATA		"Behalte Benutzer Level, Welten, Spielst�nde und Screenshot Daten ?"
