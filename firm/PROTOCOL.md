# VoxBox MIDI/SysEx protocol

* 2023/04/10: initial revision

## 3rd-party Sysex

See [AIDES eVY1 Docs](https://evy1.aides-tech.com/midi.htm).

The supervisor IC only parses some of them to display data for purely aestheric purposes.

## Supervisor IC sysex

All must be encapsulated in standard `F0...F7` and be prefixed with `7B` as the manufacturer ID.

### eVY Reset

Resets the eVY and its uC by pulling the module's #RESET line.

Data: `7F`

### eVY ROM Select

Switches the ROM bank. To apply use the `7F` command or `F0 43 79 09 01 01 00 F7`

Data: `00 nn`
Where: `nn`: `00` — Main ROM, `01` — Sub ROM

### eVY ROM Set Default

Save the current ROM mode of eVY as the startup mode. Don't execute too often, as it consumes EEPROM cycles of the SV IC.

Data: `01`

### Show message (1 line)

Display a message on the LCD/VFD.

Data: `02 tt dd dd dd... 00`
Where: `dd`: ASCII symbol (max 16)

*Notice:* Empty message or 0 time cancels display. (Return to default mode)

### Show message (2 lines)

Display a message on the LCD/VFD.

Data: `02 dd dd dd... 00 xx xx xx... 00`
Where:
* `dd`: ASCII symbol (max 16) of line 1
* `xx`: ASCII symbol (max 16) of line 2
