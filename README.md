# Simpler CRYSTALS-Kyber integration

This shows an example of easier embed for Win32 projects.

## Measurements on 7950X

```
AVX2
SKB 2400
PKB 1184
CTB 1088
B 32
Bob's Session Key: CD 5F 70 D6 A3 34 0F 15 63 30 2B 0C C5 1B E4 90 3A 53 C5 77 49 DC 30 36 47 F9 D6 9D 86 03 33 CF
Timings:
KP: 1.03ms 4660424cy
ENC: 0.10ms 451709cy
DEC: 0.07ms 293490cy
Kyber768 epic success!
```

```
CLEAN
SKB 2400
PKB 1184
CTB 1088
B 32
Bob's Session Key: D6 B6 AB FB E4 2D E3 C8 03 95 62 5C B0 33 65 25 B3 22 85 5C A5 E1 7B 42 0A DE 6C 07 B9 70 81 38
Timings:
KP: 0.78ms 3515715cy
ENC: 0.08ms 344340cy
DEC: 0.04ms 165645cy
Kyber768 epic success!
```
