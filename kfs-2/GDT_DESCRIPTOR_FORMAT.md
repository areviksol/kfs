# GDT Descriptor Format Reference

## Descriptor Byte Layout

Each GDT descriptor is exactly 8 bytes:

```
Byte:    7              6              5              4              3              2              1              0
       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
       |                         BASE ADDRESS HIGH (24-31)           |     LIMIT HIGH & FLAGS        |
       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
       |        BASE ADDRESS MID (16-23)        |     ACCESS BYTE        |     BASE ADDRESS LOW (0-15)
       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
```

### Byte 0-1: Base Address (bits 0-15)
```
Bit:  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
     +----+----+----+----+----+----+----+----+
     |        BASE ADDRESS BITS 0-15         |
     +----+----+----+----+----+----+----+----+
```

### Byte 2: Base Address (bits 16-23)
```
Bit:  7 6 5 4 3 2 1 0
     +--+--+--+--+--+--+--+--+
     |  BASE ADDRESS BITS 16-23  |
     +--+--+--+--+--+--+--+--+
```

### Byte 3: Access Byte
```
Bit:  7  6  5  4  3  2  1  0
     +--+--+--+--+--+--+--+--+
     |P |DPL|S |   TYPE    |
     +--+--+--+--+--+--+--+--+
```

- **P (Bit 7)**: Present - Descriptor is valid (1=valid, 0=invalid)
- **DPL (Bits 6-5)**: Descriptor Privilege Level
  - `00` = Ring 0 (Kernel)
  - `01` = Ring 1
  - `10` = Ring 2
  - `11` = Ring 3 (User)
- **S (Bit 4)**: System/Data flag
  - `1` = Code or Data segment (normal)
  - `0` = System segment (TSS, LDT, etc.)
- **Type (Bits 3-0)**: Segment Type
  - For Code segments: `1000` = Code-only, `1001` = Code+read, `1010` = Code-only/conforming, `1011` = Code+read/conforming
  - For Data segments: `0000` = Data-only, `0001` = Data+write, `0010` = Stack-only, `0011` = Stack+write

### Byte 4: Granularity & Limit High
```
Bit:  7  6  5  4  3  2  1  0
     +--+--+--+--+--+--+--+--+
     |G |DB|L |AVL|LIMIT16-19|
     +--+--+--+--+--+--+--+--+
```

- **G (Bit 7)**: Granularity
  - `0` = Byte granularity (limit in bytes)
  - `1` = Page granularity (limit in 4KB pages)
- **DB (Bit 6)**: Default Operation Size
  - `0` = 16-bit segment
  - `1` = 32-bit segment
- **L (Bit 5)**: Long Mode (for 64-bit, unused in 32-bit)
  - `0` = Not 64-bit mode
- **AVL (Bit 4)**: Available for use by software
- **LIMIT (Bits 3-0)**: Upper 4 bits of segment limit

### Byte 5: Base Address (bits 24-31)
```
Bit:  7 6 5 4 3 2 1 0
     +--+--+--+--+--+--+--+--+
     |  BASE ADDRESS BITS 24-31  |
     +--+--+--+--+--+--+--+--+
```

## Segment Selector Format

Segment selectors are 16-bit values used to reference GDT descriptors:

```
Bit:  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
     +----+----+----+----+----+--+--+--+
     |     INDEX (0-13)     |TI|  RPL |
     +----+----+----+----+----+--+--+--+
```

- **INDEX (Bits 15-3)**: Descriptor index in GDT/LDT (0-8191)
- **TI (Bit 2)**: Table Indicator
  - `0` = GDT (Global Descriptor Table)
  - `1` = LDT (Local Descriptor Table)
- **RPL (Bits 1-0)**: Requested Privilege Level
  - `00` = Ring 0 (Kernel)
  - `11` = Ring 3 (User)

### Example Selectors

```
GDT Index 1 (Kernel Code):
  Index = 1 → Bits 3-15: 0000_0000_0001
  TI = 0 (GDT)
  RPL = 0 (Ring 0)
  Result: 0000_0000_0000_1000 = 0x08

GDT Index 4 (User Code):
  Index = 4 → Bits 3-15: 0000_0000_0100
  TI = 0 (GDT)
  RPL = 3 (Ring 3)
  Result: 0000_0000_0010_0011 = 0x23
```

## KFS-2 GDT Descriptors

### Descriptor 0: Null Descriptor (Required)
```
Base: 0x00000000
Limit: 0x00000000
Access: 0x00 (all bits zero)
Granularity: 0x00
Selector: 0x00
```

### Descriptor 1: Kernel Code Segment
```
Base: 0x00000000
Limit: 0xFFFFF (4GB with page granularity)
Access: P=1 DPL=0 S=1 Type=0xA (Code+Read)
        = 0b1001_1010 = 0x9A
Granularity: G=1 DB=1 L=0 = 0xC0
Selector: (1 << 3) | 0 = 0x08
```

### Descriptor 2: Kernel Data Segment
```
Base: 0x00000000
Limit: 0xFFFFF (4GB with page granularity)
Access: P=1 DPL=0 S=1 Type=0x2 (Data+Write)
        = 0b1001_0010 = 0x92
Granularity: G=1 DB=1 L=0 = 0xC0
Selector: (2 << 3) | 0 = 0x10
```

### Descriptor 3: Kernel Stack Segment
```
Base: 0x00000000
Limit: 0xFFFFF (4GB with page granularity)
Access: P=1 DPL=0 S=1 Type=0x6 (Stack+Write with Direction)
        = 0b1001_0110 = 0x96
Granularity: G=1 DB=1 L=0 = 0xC0
Selector: (3 << 3) | 0 = 0x18
```

### Descriptor 4: User Code Segment
```
Base: 0x00000000
Limit: 0xFFFFF (4GB with page granularity)
Access: P=1 DPL=3 S=1 Type=0xA (Code+Read)
        = 0b1111_1010 = 0xFA
Granularity: G=1 DB=1 L=0 = 0xC0
Selector: (4 << 3) | 3 = 0x23
```

### Descriptor 5: User Data Segment
```
Base: 0x00000000
Limit: 0xFFFFF (4GB with page granularity)
Access: P=1 DPL=3 S=1 Type=0x2 (Data+Write)
        = 0b1111_0010 = 0xF2
Granularity: G=1 DB=1 L=0 = 0xC0
Selector: (5 << 3) | 3 = 0x2B
```

### Descriptor 6: User Stack Segment
```
Base: 0x00000000
Limit: 0xFFFFF (4GB with page granularity)
Access: P=1 DPL=3 S=1 Type=0x6 (Stack+Write with Direction)
        = 0b1111_0110 = 0xF6
Granularity: G=1 DB=1 L=0 = 0xC0
Selector: (6 << 3) | 3 = 0x33
```

## GDTR (GDT Register) Format

The GDTR is loaded with the LGDT instruction and contains:

```
Byte 0-1: Limit (size of GDT - 1 in bytes)
Byte 2-5: Base (linear address of first descriptor)
```

For KFS-2:
```
Limit: (7 descriptors × 8 bytes) - 1 = 55 = 0x37
Base: 0x00000800
```

## Access Byte Combinations

### For Code Segments:
```
P=1 DPL=0 S=1 Type=8 (Code, no read)   = 0x98
P=1 DPL=0 S=1 Type=10 (Code, readable)  = 0x9A
P=1 DPL=0 S=1 Type=12 (Code, conform)   = 0x9C
P=1 DPL=0 S=1 Type=14 (Code, conform+read) = 0x9E

P=1 DPL=3 S=1 Type=8 (Code, no read)   = 0xF8
P=1 DPL=3 S=1 Type=10 (Code, readable)  = 0xFA
P=1 DPL=3 S=1 Type=12 (Code, conform)   = 0xFC
P=1 DPL=3 S=1 Type=14 (Code, conform+read) = 0xFE
```

### For Data/Stack Segments:
```
P=1 DPL=0 S=1 Type=0 (Data, no write)    = 0x90
P=1 DPL=0 S=1 Type=2 (Data, writable)    = 0x92
P=1 DPL=0 S=1 Type=4 (Stack, no write)   = 0x94
P=1 DPL=0 S=1 Type=6 (Stack, writable)   = 0x96

P=1 DPL=3 S=1 Type=0 (Data, no write)    = 0xF0
P=1 DPL=3 S=1 Type=2 (Data, writable)    = 0xF2
P=1 DPL=3 S=1 Type=4 (Stack, no write)   = 0xF4
P=1 DPL=3 S=1 Type=6 (Stack, writable)   = 0xF6
```

## Bit Manipulation for Descriptors

When creating a descriptor:

```c
// Base address is split across 3 fields:
descriptor.base_low = base & 0xFFFF;           // Bits 0-15
descriptor.base_mid = (base >> 16) & 0xFF;     // Bits 16-23
descriptor.base_high = (base >> 24) & 0xFF;    // Bits 24-31

// Limit is split across 2 fields:
descriptor.limit_low = limit & 0xFFFF;                    // Bits 0-15
descriptor.granularity = ((limit >> 16) & 0x0F) | flags; // Bits 16-19
```

## Privilege Level Transitions

When setting segment selectors, the CPU checks privilege levels:

```
Current Privilege Level (CPL): Determined by CS selector's RPL
Descriptor Privilege Level (DPL): Set in GDT descriptor
Requested Privilege Level (RPL): Set in selector when loading
```

For safe operations:
- Loading code segment (CS): New RPL must be ≤ CPL (can go down in privilege)
- Loading data segments: New DPL must be ≥ CPL (can go up in restriction)
