#include "Hacks/MemoryPatch.h"
#include "ZenGin/zGothicAPI.h"

// Always enable opening another instance of the game
static MemoryPatch patch_disable_instance_check(zSwitch(0x004F3FA7, 0x0050685A, 0x00500287, 0x00502F17),
{
	0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, 0
	0x90 // nop
});

// Disable error during copying default miles sound provider into System dir
static MemoryPatch patch_disable_error_msssoft_1(zSwitch(0x004DE4C8, 0x004EF9C8, 0x004E8DCA, 0x004EBA4A),
{
	0xB0, 0x00 // mov al, 0
});

// Disable error during copying selected miles sound provider into System dir
static MemoryPatch patch_disable_error_msssoft_2(zSwitch(0x004DE6AD, 0x004EFBED, 0x004E8FB3, 0x004EBC33),
{
	0xB0, 0x00 // mov al, 0
});

// Set sysStopProcessingMessages to `TRUE` to enable window management
static MemoryPatch patch_set_sysStopProcessingMessages(zSwitch(0x0086F4C0, 0x008B50DC, 0x008C5BFC, 0x008D4234), { TRUE });

// Disable gameAbnormalExit default initialization to `1`
static MemoryPatch patch_init_gameAbnormalExit(zSwitch(0x0046B080, 0x00473303, 0x0046FB12, 0x00470AE1), {
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // nop -> mov ecx, ?zoptions@@3PAVzCOption@@A ; this
	0x90, 0x90, // nop -> push 0 ; int
	0x90, 0x90, // nop -> push 1 ; int
	0x90, 0x90, 0x90, 0x90, 0x90, // nop -> push offset ??_C@_0BB@NIPB@gameAbnormalExit?$AA@ ; "gameAbnormalExit"
	0x90, 0x90, 0x90, 0x90, 0x90, // nop -> push offset ?zOPT_SEC_INTERNAL@@3VzSTRING@@B ; struct zSTRING *
	0x90, 0x90, 0x90, 0x90, 0x90, // nop -> call ?WriteBool@zCOption@@QAEHABVzSTRING@@PBDHH@Z ; zCOption::WriteBool(zSTRING const &,char const *,int,int)
});