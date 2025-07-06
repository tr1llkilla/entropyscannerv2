# Entropyscannerv2

**Author:** Cadell Richard Anderson  
**License:** Custom License: Entropyscannerv2 Attribution License (EAL) v2.0
**Version:** 0.2 
**Date:** July 2025

Licensing 

📜 Custom License: Quectobyte Attribution License (QAL) v1.0
Author & Originator: Cadell Richard Anderson Date: July 2025

1. Purpose
This license governs the use, reproduction, and distribution of the Entropyscannerv2 and any derivative works that utilize the concept of the Entropyscannerv2 as a unit of sub-byte information in computing, virtualization, or memory abstraction.

2. Permissions
You are permitted to:

Use, modify, and distribute the Software for academic, research, or commercial purposes.

Incorporate the Software or its concepts into larger systems or frameworks.

3. Conditions
Any work—academic, commercial, or otherwise—that:

Utilizes the concept of the Entropyscannerv2 as a unit of digital information,

Implements sub-byte memory modeling or telemetry inspired by this Software,

Derives from or builds upon the Entropyscannerv2,

must include clear and visible attribution to:

Cadell Richard Anderson Originator of the Entropyscannerv2 computing model and author of the Entropyscannerv2

Attribution must appear in:

Source code headers

Documentation

Academic citations (if published)

User-facing credits (if distributed as software)

4. Restrictions
You may not:

Misrepresent the origin of the concept of the Entropyscannerv2 in computing.

Remove or obscure attribution to Cadell Richard Anderson in any derivative work.

5. Disclaimer
The Software is provided “as is,” without warranty of any kind. The author shall not be liable for any damages arising from the use of this Software.

6. Citation Format
If citing in academic work, use the following format:

Anderson, C. R. (2025). Entropyscannerv2: A Sub-Byte Malware Analysis Framework for Virtualized Memory Systems. Unpublished manuscript.

Advanced Base 3+Base 4 integrated malware analysis frameworks. (next gen malware analysis and response advancement)
 
 Entropy Analyzer with Belnap's Logic
This project implements a C++ application designed to scan files on a Windows system, calculate their Shannon entropy, and classify their "suspiciousness" using a custom four-valued (Belnap's) logic system. Files identified as "suspicious" or "critical" based on their entropy are automatically copied to a designated quarantine/dump folder.

Features
File System Scanning: Recursively scans all files on the system drive (or a specified root).

Shannon Entropy Calculation: Computes the Shannon entropy for blocks of data within each file, providing an indicator of randomness (often associated with packed, encrypted, or compressed malicious code).

Four-Valued Logic (Belnap's Logic): Utilizes a custom FourValuedBoolean enum (True, False, Neither, Both) to represent and propagate uncertainty in analysis results.

True: Definitely suspicious/critical (high entropy).

False: Definitely benign (normal entropy).

Neither: Indeterminate/Suspicious (entropy in a grey area, requiring further analysis).

Both: Not currently used in entropy analysis, but available for representing contradictory evidence in broader logical contexts.

Logical Operators: Overloaded !, &&, and || operators for FourValuedBoolean to enable complex logical reasoning with uncertain states.

Automated File Copying: Automatically copies files classified as True (critical) or Neither (suspicious/indeterminate) by the entropy analysis to a configurable destination folder.

Console Output: Provides real-time feedback on file scanning, entropy calculation, and file copying status.
