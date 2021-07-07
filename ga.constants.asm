; Equates section - names for variables


; ---------------------------------------------------------------------------
; Address equates
; ---------------------------------------------------------------------------

; Z80 addresses
Z80_RAM = $A00000 ; start of Z80 RAM
Z80_RAM_end = $A02000 ; end of non-reserved Z80 RAM
Z80_bus_request = $A11100
Z80_reset = $A11200

Security_addr = $A14000
; ---------------------------------------------------------------------------

; I/O Area 
HW_Version = $A10001
HW_Port_1_Data = $A10003
HW_Port_2_Data = $A10005
HW_Expansion_Data = $A10007
HW_Port_1_Control = $A10009
HW_Port_2_Control = $A1000B
HW_Expansion_Control = $A1000D
HW_Port_1_TxData = $A1000F
HW_Port_1_RxData = $A10011
HW_Port_1_SCtrl = $A10013
HW_Port_2_TxData = $A10015
HW_Port_2_RxData = $A10017
HW_Port_2_SCtrl = $A10019
HW_Expansion_TxData = $A1001B
HW_Expansion_RxData = $A1001D
HW_Expansion_SCtrl = $A1001F
; ---------------------------------------------------------------------------

; VDP addresses
VDP_data_port = $C00000
VDP_control_port = $C00004
PSG_input = $C00011
; ---------------------------------------------------------------------------

; sign-extends a 32-bit integer to 64-bit
; all RAM addresses are run through this function to allow them to work in both 16-bit and 32-bit addressing modes
ramaddr function x,(-(x&$80000000)<<1)|x

; RAM addresses
RAM_start = ramaddr($FFFF0000)


Game_mode = ramaddr($FFFFC170) ; word
Saved_game_mode = ramaddr($FFFFC172) ; word
Ctrl_1 = ramaddr($FFFFC176) ; word ; both held and pressed
Ctrl_1_held = ramaddr($FFFFC176) ; byte ; all held buttons
Ctrl_1_pressed = ramaddr($FFFFC177) ; byte ; buttons being pressed newly this frame
Ctrl_2 = ramaddr($FFFFC178) ; word ; both held and pressed
Ctrl_2_held = ramaddr($FFFFC178) ; byte
Ctrl_2_pressed = ramaddr($FFFFC179) ; byte
V_int_run_count = ramaddr($FFFFC17A) ; word
General_timer = ramaddr($FFFFC180) ; word