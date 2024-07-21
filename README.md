## Benign Hunter

This tool was made as a precursor to a tool that I am going to add to my C2 project that I am working on. I will release the tool individually. Until then I won't talk much about it because I think it's a cool idea and I want to explore / flesh it out more.

However, this tool is a simple tool to try and identify which native api's are deemed benign by EDRs and are therefore not hooked. Now, this isn't a perfect solution because every EDR may have differences of opinion. But, if you know what EDRs your target is running or might be running this could be useful for the second tool I will release within the next few weeks.

Here is sample output for the tool that was generated under the `FREE` trial version of SOPHOS EDR solution:

```
Syscall Name: ZwAcceptConnectPort
Syscall Hash: 0x5483E5E5
SSN 2
Syscall Name: ZwAccessCheck
Syscall Hash: 0xDBC33963
SSN 0
Syscall Name: ZwAccessCheckAndAuditAlarm
Syscall Hash: 0x890E4CAA
SSN 41
Syscall Name: ZwAccessCheckByType
Syscall Hash: 0xCD4D934B
SSN 99
Syscall Name: ZwAccessCheckByTypeAndAuditAlarm
Syscall Hash: 0xFD95677D
SSN 89
Syscall Name: ZwAccessCheckByTypeResultList
Syscall Hash: 0xE8DA39D8
SSN 100
... <shortened for brevity>
```

## How it works
The tool is pretty simple:
1) Opens a handle to NTDLL
2) Parses the exported functions for anything that starts with 'Zw'
3) Uses the exported address as a pointer to the bytes for the function to check the first bytes. If it's a jmp call we know it's hooked and monitored.
4) Parses the rest of the function for the SSN.
5) Prints Info in a format that should be easily managable using Notepad++ for whatever you need. Or you could simply change it yourself in the code when you run it on your machine to match for example the format for a structure in C for your own code.
