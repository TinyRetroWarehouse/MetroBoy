#include "CoreLib/Constants.h"

#if 0

	LD SP,$fffe		; $0000  Setup Stack

	XOR A			; $0003  Zero the memory from $8000-$9FFF (VRAM)
	LD HL,$9fff		; $0004
Addr_0007:
	LD (HL-),A		; $0007
	BIT 7,H		; $0008
	JR NZ, Addr_0007	; $000a

	LD HL,$ff26		; $000c  Setup Audio
	LD C,$11		; $000f
	LD A,$80		; $0011 
	LD (HL-),A		; $0013
	LD ($FF00+C),A	; $0014
	INC C			; $0015
	LD A,$f3		; $0016
	LD ($FF00+C),A	; $0018
	LD (HL-),A		; $0019
	LD A,$77		; $001a
	LD (HL),A		; $001c

	LD A,$fc		; $001d  Setup BG palette
	LD ($FF47),A	; $001f

	LD DE,$0104		; $0021  Convert and load logo data from cart into Video RAM
	LD HL,$8010		; $0024
Addr_0027:
	LD A,(DE)		; $0027
	CALL $0095		; $0028
	CALL $0096		; $002b
	INC DE		; $002e
	LD A,E		; $002f
	CP $34		; $0030
	JR NZ, Addr_0027	; $0032

	LD DE,$00d8		; $0034  Load 8 additional bytes into Video RAM
	LD B,$08		; $0037
Addr_0039:
	LD A,(DE)		; $0039
	INC DE		; $003a
	LD (HL+),A		; $003b
	INC HL		; $003c
	DEC B			; $003d
	JR NZ, Addr_0039	; $003e

	LD A,$19		; $0040  Setup background tilemap
	LD ($9910),A	; $0042
	LD HL,$992f		; $0045
Addr_0048:
	LD C,$0c		; $0048

Addr_004A:
	DEC A			; $004a
	JR Z, Addr_0055	; $004b
	LD (HL-),A		; $004d
	DEC C			; $004e
	JR NZ, Addr_004A	; $004f

	LD L,$0f		; $0051
	JR Addr_0048	; $0053

	; === Scroll logo on screen, and play logo sound===

Addr_0055:
	LD H,A		; $0055  Initialize scroll count, H=0
	LD A,$64		; $0056
	LD D,A		; $0058  set loop count, D=$64
	LD (SCY),A	; $0059  Set vertical scroll register
	LD A,$91		; $005b
	LD ($FF40),A	; $005d  Turn on LCD, showing Background
	INC B			; $005f  Set B=1

Addr_0060:
	LD E,$02		; $0060
Addr_0062:
	LD C,$0c		; $0062
Addr_0064:
	LD A,($FF44)	; $0064  wait for screen frame
	CP $90		; $0066
	JR NZ, Addr_0064	; $0068
	DEC C			; $006a
	JR NZ, Addr_0064	; $006b
	DEC E			; $006d
	JR NZ, Addr_0062	; $006e

	LD C,$13		; $0070
	INC H			; $0072  increment scroll count
	LD A,H		; $0073
	LD E,$83		; $0074
	CP $62		; $0076  $62 counts in, play sound #1
	JR Z, Addr_0080	; $0078
	LD E,$c1		; $007a
	CP $64		; $007c
	JR NZ, Addr_0086	; $007e  $64 counts in, play sound #2
Addr_0080:
	LD A,E		; $0080  play sound
	LD ($FF00+C),A	; $0081
	INC C			; $0082
	LD A,$87		; $0083
	LD ($FF00+C),A	; $0085
Addr_0086:
	LD A,(SCY)	; $0086
	SUB B			; $0088
	LD (SCY),A	; $0089  scroll logo up if B=1
	DEC D			; $008b  
	JR NZ, Addr_0060	; $008c

	DEC B			; $008e  set B=0 first time
	JR NZ, Addr_00E0	; $008f    ... next time, cause jump to "Nintendo Logo check"

	LD D,$20		; $0091  use scrolling loop to pause
	JR Addr_0060	; $0093

	; ==== Graphic routine ====

	LD C,A		; $0095  "Double up" all the bits of the graphics data
	LD B,$04		; $0096     and store in Video RAM
Addr_0098:
	PUSH BC		; $0098
	RL C			; $0099
	RLA			; $009b
	POP BC		; $009c
	RL C			; $009d
	RLA			; $009f
	DEC B			; $00a0
	JR NZ, Addr_0098	; $00a1
	LD (HL+),A		; $00a3
	INC HL		; $00a4
	LD (HL+),A		; $00a5
	INC HL		; $00a6
	RET			; $00a7

Addr_00A8:
	;Nintendo Logo
	.DB $CE,$ED,$66,$66,$CC,$0D,$00,$0B,$03,$73,$00,$83,$00,$0C,$00,$0D 
	.DB $00,$08,$11,$1F,$88,$89,$00,$0E,$DC,$CC,$6E,$E6,$DD,$DD,$D9,$99 
	.DB $BB,$BB,$67,$63,$6E,$0E,$EC,$CC,$DD,$DC,$99,$9F,$BB,$B9,$33,$3E 

Addr_00D8:
	;More video data
	.DB $3C,$42,$B9,$A5,$B9,$A5,$42,$3C

	; ===== Nintendo logo comparison routine =====

Addr_00E0:	
	LD HL,$0104		; $00e0	; point HL to Nintendo logo in cart
	LD DE,$00a8		; $00e3	; point DE to Nintendo logo in DMG rom

Addr_00E6:
	LD A,(DE)		; $00e6
	INC DE		; $00e7
	CP (HL)		; $00e8	;compare logo data in cart to DMG rom
	JR NZ,$fe		; $00e9	;if not a match, lock up here
	INC HL		; $00eb
	LD A,L		; $00ec
	CP $34		; $00ed	;do this for $30 bytes
	JR NZ, Addr_00E6	; $00ef

	LD B,$19		; $00f1
	LD A,B		; $00f3
Addr_00F4:
	ADD (HL)		; $00f4
	INC HL		; $00f5
	DEC B			; $00f6
	JR NZ, Addr_00F4	; $00f7
	ADD (HL)		; $00f9
	JR NZ,$fe		; $00fa	; if $19 + bytes from $0134-$014D  don't add to $00
						;  ... lock up

	LD A,$01		; $00fc
	LD ($FF50),A	; $00fe	;turn off DMG rom

#endif
//-----------------------------------------------------------------------------
// the boot rom that goes "baBING"

extern const uint8_t DMG_ROM_bin[] = {
  0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32,
  0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e,
  0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
  0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0,
  0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a,
  0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
  0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06,
  0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9,
  0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99,
  0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20,
  0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64,
  0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
  0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90,
  0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2,
  0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62,
  0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06,
  0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42,
  0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
  0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04,
  0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17,
  0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,

  // the logo @ 0x00A8
  0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b,
  0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
  0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
  0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
  0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc,
  0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e,

  0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c,
  0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13,
  0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
  0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20,
  0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};


//-----------------------------------------------------------------------------

extern const uint32_t gb_colors[8] = {
  0xFF7F7F7F,
  0xFF5F5F5F,
  0xFF3F3F3F,
  0xFF1F1F1F,
  0xFF9F7F7F,
  0xFF7F5F5F,
  0xFF5F3F3F,
  0xFF3F1F1F,
};

//-----------------------------------------------------------------------------

const char* op_strings2[256] = {
  "nop",             "ld bc, d16",  "ld (bc), a",     "inc bc",      "inc b",        "dec b",        "ld b, d8",     "rlca",
  "ld (d16), sp",    "add hl, bc",  "ld a, (bc)",     "dec bc",      "inc c",        "dec c",        "ld c, d8",     "rrca",
  "stop",            "ld de, d16",  "ld (de), a",     "inc de",      "inc d",        "dec d",        "ld d, d8",     "rla",
  "jr d16",          "add hl, de",  "ld a, (de)",     "dec de",      "inc e",        "dec e",        "ld e, d8",     "rra",
  "jr nz, d16",      "ld hl, d16",  "ld (hl+), a",    "inc hl",      "inc h",        "dec h",        "ld h, d8",     "daa",
  "jr z, d16",       "add hl, hl",  "ld a, (hl+)",    "dec hl",      "inc l",        "dec l",        "ld l, d8",     "cpl",
  "jr nc, d16",      "ld sp, d16",  "ld (hl-), a",    "inc sp",      "inc (hl)",     "dec (hl)",     "ld (hl), d8",  "scf",
  "jr c, d16",       "add hl, sp",  "ld a, (hl-)",    "dec sp",      "inc a",        "dec a",        "ld a, d8",     "ccf",
                     
  "ld b, b",         "ld b, c",     "ld b, d",        "ld b, e",     "ld b, h",      "ld b, l",      "ld b, (hl)",   "ld b, a",
  "ld c, b",         "ld c, c",     "ld c, d",        "ld c, e",     "ld c, h",      "ld c, l",      "ld c, (hl)",   "ld c, a",
  "ld d, b",         "ld d, c",     "ld d, d",        "ld d, e",     "ld d, h",      "ld d, l",      "ld d, (hl)",   "ld d, a",
  "ld e, b",         "ld e, c",     "ld e, d",        "ld e, e",     "ld e, h",      "ld e, l",      "ld e, (hl)",   "ld e, a",
  "ld h, b",         "ld h, c",     "ld h, d",        "ld h, e",     "ld h, h",      "ld h, l",      "ld h, (hl)",   "ld h, a",
  "ld l, b",         "ld l, c",     "ld l, d",        "ld l, e",     "ld l, h",      "ld l, l",      "ld l, (hl)",   "ld l, a",
  "ld (hl), b",      "ld (hl), c",  "ld (hl), d",     "ld (hl), e",  "ld (hl), h",   "ld (hl), l",   "halt",         "ld (hl), a",
  "ld a, b",         "ld a, c",     "ld a, d",        "ld a, e",     "ld a, h",      "ld a, l",      "ld a, (hl)",   "ld a, a",
                                    
  "add b",           "add c",       "add d",          "add e",       "add h",        "add l",        "add (hl)",     "add a",
  "adc b",           "adc c",       "adc d",          "adc e",       "adc h",        "adc l",        "adc (hl)",     "adc a",
  "sub b",           "sub c",       "sub d",          "sub e",       "sub h",        "sub l",        "sub (hl)",     "sub a",
  "sbc b",           "sbc c",       "sbc d",          "sbc e",       "sbc h",        "sbc l",        "sbc (hl)",     "sbc a",
  "and b",           "and c",       "and d",          "and e",       "and h",        "and l",        "and (hl)",     "and a",
  "xor b",           "xor c",       "xor d",          "xor e",       "xor h",        "xor l",        "xor (hl)",     "xor a",
  "or b",            "or c",        "or d",           "or e",        "or h",         "or l",         "or  (hl)",     "or a",
  "cp b",            "cp c",        "cp d",           "cp e",        "cp h",         "cp l",         "cp  (hl)",     "cp a",
                                    
  "ret nz",          "pop bc",      "jp nz, d16",     "jp d16",      "call nz, d16", "push bc",      "add d8",       "rst 0x00",
  "ret z",           "ret",         "jp z, d16",      "prefix cb",   "call z, d16",  "call d16",     "adc d8",       "rst 0x08",
  "ret nc",          "pop de",      "jp nc, d16",     "undefined1",  "call nc, d16", "push de",      "sub d8",       "rst 0x10",
  "ret c",           "reti",        "jp c, d16",      "undefined2",  "call c, d16",  "undefined3",   "sbc d8",       "rst 0x18",
  "ldh (d8), a",     "pop hl",      "ldh (c), a",     "undefined4",  "undefined5",   "push hl",      "and d8",       "rst 0x20",
  "add sp, d8",      "jp hl",       "ld (d16), a",    "undefined6",  "undefined7",   "undefined8",   "xor d8",       "rst 0x28",
  "ldh a, (d8)",     "pop af",      "ldh a, (c)",     "di",          "(isr)",        "push af",      "or d8",        "rst 0x30",
  "ld hl, sp + d8",  "ld sp, hl",   "ld a, (d16)",    "ei",          "undefined10",  "undefined11",  "cp d8",        "rst 0x38",
};


const char* op_strings[256] = {
  "nop",                "ld bc, $%04hx",  "ld (bc), a",      "inc bc",      "inc b",           "dec b",        "ld b, $%02hhx",    "rlca",
  "ld ($%04hx), sp",    "add hl, bc",     "ld a, (bc)",      "dec bc",      "inc c",           "dec c",        "ld c, $%02hhx",    "rrca",
  "stop",               "ld de, $%04hx",  "ld (de), a",      "inc de",      "inc d",           "dec d",        "ld d, $%02hhx",    "rla",
  "jr %hhd",            "add hl, de",     "ld a, (de)",      "dec de",      "inc e",           "dec e",        "ld e, $%02hhx",    "rra",
  "jr nz, %hhd",        "ld hl, $%04hx",  "ld (hl+), a",     "inc hl",      "inc h",           "dec h",        "ld h, $%02hhx",    "daa",
  "jr z, %hhd",         "add hl, hl",     "ld a, (hl+)",     "dec hl",      "inc l",           "dec l",        "ld l, $%02hhx",    "cpl",
  "jr nc, %hhd",        "ld sp, $%04hx",  "ld (hl-), a",     "inc sp",      "inc (hl)",        "dec (hl)",     "ld (hl), $%02hhx", "scf",
  "jr c, %hhd",         "add hl, sp",     "ld a, (hl-)",     "dec sp",      "inc a",           "dec a",        "ld a, $%02hhx",    "ccf",
                                                                                               
  "ld b, b",            "ld b, c",        "ld b, d",         "ld b, e",     "ld b, h",         "ld b, l",      "ld b, (hl)",       "ld b, a",
  "ld c, b",            "ld c, c",        "ld c, d",         "ld c, e",     "ld c, h",         "ld c, l",      "ld c, (hl)",       "ld c, a",
  "ld d, b",            "ld d, c",        "ld d, d",         "ld d, e",     "ld d, h",         "ld d, l",      "ld d, (hl)",       "ld d, a",
  "ld e, b",            "ld e, c",        "ld e, d",         "ld e, e",     "ld e, h",         "ld e, l",      "ld e, (hl)",       "ld e, a",
  "ld h, b",            "ld h, c",        "ld h, d",         "ld h, e",     "ld h, h",         "ld h, l",      "ld h, (hl)",       "ld h, a",
  "ld l, b",            "ld l, c",        "ld l, d",         "ld l, e",     "ld l, h",         "ld l, l",      "ld l, (hl)",       "ld l, a",
  "ld (hl), b",         "ld (hl), c",     "ld (hl), d",      "ld (hl), e",  "ld (hl), h",      "ld (hl), l",   "halt",             "ld (hl), a",
  "ld a, b",            "ld a, c",        "ld a, d",         "ld a, e",     "ld a, h",         "ld a, l",      "ld a, (hl)",       "ld a, a",
                                                                                                                                   
  "add b",              "add c",          "add d",           "add e",       "add h",           "add l",        "add (hl)",         "add a",
  "adc b",              "adc c",          "adc d",           "adc e",       "adc h",           "adc l",        "adc (hl)",         "adc a",
  "sub b",              "sub c",          "sub d",           "sub e",       "sub h",           "sub l",        "sub (hl)",         "sub a",
  "sbc b",              "sbc c",          "sbc d",           "sbc e",       "sbc h",           "sbc l",        "sbc (hl)",         "sbc a",
  "and b",              "and c",          "and d",           "and e",       "and h",           "and l",        "and (hl)",         "and a",
  "xor b",              "xor c",          "xor d",           "xor e",       "xor h",           "xor l",        "xor (hl)",         "xor a",
  "or b",               "or c",           "or d",            "or e",        "or h",            "or l",         "or  (hl)",         "or a",
  "cp b",               "cp c",           "cp d",            "cp e",        "cp h",            "cp l",         "cp  (hl)",         "cp a",
                                          
  "ret nz",             "pop bc",         "jp nz, $%04hx",   "jp $%04hx",   "call nz, $%04hx", "push bc",      "add $%02hhx",      "rst $00",
  "ret z",              "ret",            "jp z, $%04hx",    "prefix cb",   "call z, $%04hx",  "call $%04hx",  "adc $%02hhx",      "rst $08",
  "ret nc",             "pop de",         "jp nc, $%04hx",   "undefined1",  "call nc, $%04hx", "push de",      "sub $%02hhx",      "rst $10",
  "ret c",              "reti",           "jp c, $%04hx",    "undefined2",  "call c, $%04hx",  "undefined3",   "sbc $%02hhx",      "rst $18",
  "ldh ($%02hhx), a",   "pop hl",         "ldh (c), a",      "undefined4",  "undefined5",      "push hl",      "and $%02hhx",      "rst $20",
  "add sp, %d",         "jp hl",          "ld ($%04hx), a",  "undefined6",  "undefined7",      "undefined8",   "xor $%02hhx",      "rst $28",
  "ldh a, ($%02hhx)",   "pop af",         "ldh a, (c)",      "di",          "(isr)",           "push af",      "or $%02hhx",       "rst $30",
  "ld hl, sp + %d",     "ld sp, hl",      "ld a, ($%04hx)",  "ei",          "undefined10",     "undefined11",  "cp $%02hhx",       "rst $38",
};

const char* cb_strings[256] = {
  "rlc b",    "rlc c",    "rlc d",    "rlc e",    "rlc h",    "rlc l",    "rlc (hl)",    "rlc a",
  "rrc b",    "rrc c",    "rrc d",    "rrc e",    "rrc h",    "rrc l",    "rrc (hl)",    "rrc a",
  "rl b",     "rl c",     "rl d",     "rl e",     "rl h",     "rl l",     "rl (hl)",     "rl a",
  "rr b",     "rr c",     "rr d",     "rr e",     "rr h",     "rr l",     "rr (hl)",     "rr a",
  "sla b",    "sla c",    "sla d",    "sla e",    "sla h",    "sla l",    "sla (hl)",    "sla a",
  "sra b",    "sra c",    "sra d",    "sra e",    "sra h",    "sra l",    "sra (hl)",    "sra a",
  "swap b",   "swap c",   "swap d",   "swap e",   "swap h",   "swap l",   "swap (hl)",   "swap a",
  "srl b",    "srl c",    "srl d",    "srl e",    "srl h",    "srl l",    "srl (hl)",    "srl a",

  "bit 0, b", "bit 0, c", "bit 0, d", "bit 0, e", "bit 0, h", "bit 0, l", "bit 0, (hl)", "bit 0, a",
  "bit 1, b", "bit 1, c", "bit 1, d", "bit 1, e", "bit 1, h", "bit 1, l", "bit 1, (hl)", "bit 1, a",
  "bit 2, b", "bit 2, c", "bit 2, d", "bit 2, e", "bit 2, h", "bit 2, l", "bit 2, (hl)", "bit 2, a",
  "bit 3, b", "bit 3, c", "bit 3, d", "bit 3, e", "bit 3, h", "bit 3, l", "bit 3, (hl)", "bit 3, a",
  "bit 4, b", "bit 4, c", "bit 4, d", "bit 4, e", "bit 4, h", "bit 4, l", "bit 4, (hl)", "bit 4, a",
  "bit 5, b", "bit 5, c", "bit 5, d", "bit 5, e", "bit 5, h", "bit 5, l", "bit 5, (hl)", "bit 5, a",
  "bit 6, b", "bit 6, c", "bit 6, d", "bit 6, e", "bit 6, h", "bit 6, l", "bit 6, (hl)", "bit 6, a",
  "bit 7, b", "bit 7, c", "bit 7, d", "bit 7, e", "bit 7, h", "bit 7, l", "bit 7, (hl)", "bit 7, a",

  "res 0, b", "res 0, c", "res 0, d", "res 0, e", "res 0, h", "res 0, l", "res 0, (hl)", "res 0, a",
  "res 1, b", "res 1, c", "res 1, d", "res 1, e", "res 1, h", "res 1, l", "res 1, (hl)", "res 1, a",
  "res 2, b", "res 2, c", "res 2, d", "res 2, e", "res 2, h", "res 2, l", "res 2, (hl)", "res 2, a",
  "res 3, b", "res 3, c", "res 3, d", "res 3, e", "res 3, h", "res 3, l", "res 3, (hl)", "res 3, a",
  "res 4, b", "res 4, c", "res 4, d", "res 4, e", "res 4, h", "res 4, l", "res 4, (hl)", "res 4, a",
  "res 5, b", "res 5, c", "res 5, d", "res 5, e", "res 5, h", "res 5, l", "res 5, (hl)", "res 5, a",
  "res 6, b", "res 6, c", "res 6, d", "res 6, e", "res 6, h", "res 6, l", "res 6, (hl)", "res 6, a",
  "res 7, b", "res 7, c", "res 7, d", "res 7, e", "res 7, h", "res 7, l", "res 7, (hl)", "res 7, a",

  "set_ff9 0, b", "set_ff9 0, c", "set_ff9 0, d", "set_ff9 0, e", "set_ff9 0, h", "set_ff9 0, l", "set_ff9 0, (hl)", "set_ff9 0, a",
  "set_ff9 1, b", "set_ff9 1, c", "set_ff9 1, d", "set_ff9 1, e", "set_ff9 1, h", "set_ff9 1, l", "set_ff9 1, (hl)", "set_ff9 1, a",
  "set_ff9 2, b", "set_ff9 2, c", "set_ff9 2, d", "set_ff9 2, e", "set_ff9 2, h", "set_ff9 2, l", "set_ff9 2, (hl)", "set_ff9 2, a",
  "set_ff9 3, b", "set_ff9 3, c", "set_ff9 3, d", "set_ff9 3, e", "set_ff9 3, h", "set_ff9 3, l", "set_ff9 3, (hl)", "set_ff9 3, a",
  "set_ff9 4, b", "set_ff9 4, c", "set_ff9 4, d", "set_ff9 4, e", "set_ff9 4, h", "set_ff9 4, l", "set_ff9 4, (hl)", "set_ff9 4, a",
  "set_ff9 5, b", "set_ff9 5, c", "set_ff9 5, d", "set_ff9 5, e", "set_ff9 5, h", "set_ff9 5, l", "set_ff9 5, (hl)", "set_ff9 5, a",
  "set_ff9 6, b", "set_ff9 6, c", "set_ff9 6, d", "set_ff9 6, e", "set_ff9 6, h", "set_ff9 6, l", "set_ff9 6, (hl)", "set_ff9 6, a",
  "set_ff9 7, b", "set_ff9 7, c", "set_ff9 7, d", "set_ff9 7, e", "set_ff9 7, h", "set_ff9 7, l", "set_ff9 7, (hl)", "set_ff9 7, a",
};

extern const int op_sizes[256] = {
  1, 3, 1, 1, 1, 1, 2, 1,
  3, 1, 1, 1, 1, 1, 2, 1,
  2, 3, 1, 1, 1, 1, 2, 1,
  2, 1, 1, 1, 1, 1, 2, 1,
  2, 3, 1, 1, 1, 1, 2, 1,
  2, 1, 1, 1, 1, 1, 2, 1,
  2, 3, 1, 1, 1, 1, 2, 1,
  2, 1, 1, 1, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,

  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,

  1, 1, 3, 3, 3, 1, 2, 1,
  1, 1, 3, 1, 3, 3, 2, 1,
  1, 1, 3, 1, 3, 1, 2, 1,
  1, 1, 3, 1, 3, 1, 2, 1,
  2, 1, 1, 1, 1, 1, 2, 1,
  2, 1, 3, 1, 1, 1, 2, 1,
  2, 1, 1, 1, 1, 1, 2, 1,
  2, 1, 3, 1, 1, 1, 2, 1,
};

extern const int op_times_min[256] = {
  1, 3, 2, 2, 1, 1, 2, 1,
  5, 2, 2, 2, 1, 1, 2, 1,
  1, 3, 2, 2, 1, 1, 2, 1,
  3, 2, 2, 2, 1, 1, 2, 1,
  2, 3, 2, 2, 1, 1, 2, 1,
  2, 2, 2, 2, 1, 1, 2, 1,
  2, 3, 2, 2, 3, 3, 3, 1,
  2, 2, 2, 2, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  2, 2, 2, 2, 2, 2, 1, 2,
  1, 1, 1, 1, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,

  2, 3, 3, 4, 3, 4, 2, 4,
  2, 4, 3, 1, 3, 6, 2, 4,
  2, 3, 3, 0, 3, 4, 2, 4,
  2, 4, 3, 0, 3, 0, 2, 4,
  3, 3, 2, 0, 0, 4, 2, 4,
  4, 1, 4, 0, 0, 0, 2, 4,
  3, 3, 2, 1, 0, 4, 2, 4,
  3, 2, 4, 1, 0, 0, 2, 4,
};

extern const int op_times_max[256] = {
  1, 3, 2, 2, 1, 1, 2, 1,
  5, 2, 2, 2, 1, 1, 2, 1,
  1, 3, 2, 2, 1, 1, 2, 1,
  3, 2, 2, 2, 1, 1, 2, 1,
  3, 3, 2, 2, 1, 1, 2, 1,
  3, 2, 2, 2, 1, 1, 2, 1,
  3, 3, 2, 2, 3, 3, 3, 1,
  3, 2, 2, 2, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  2, 2, 2, 2, 2, 2, 1, 2,
  1, 1, 1, 1, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1,

  5, 3, 4, 4, 6, 4, 2, 4,
  5, 4, 4, 1, 6, 6, 2, 4,
  5, 3, 4, 0, 6, 4, 2, 4,
  5, 4, 4, 0, 6, 0, 2, 4,
  3, 3, 2, 0, 0, 4, 2, 4,
  4, 1, 4, 0, 0, 0, 2, 4,
  3, 3, 2, 1, 0, 4, 2, 4,
  3, 2, 4, 1, 0, 0, 2, 4,
};

//-----------------------------------------------------------------------------

uint8_t flip(uint8_t x) {
  x = ((x & 0b10101010) >> 1) | ((x & 0b01010101) << 1);
  x = ((x & 0b11001100) >> 2) | ((x & 0b00110011) << 2);
  x = ((x & 0b11110000) >> 4) | ((x & 0b00001111) << 4);
  return x;
}

//-----------------------------------------------------------------------------

