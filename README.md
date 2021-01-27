# StoopidDB

StoopidDB is a simple DataBase header that can quickly be added to any project needing a quick and dirty DB solution - (Not recommended for production use (USE AT OWN RISK))<br>



StoopidDB format info Specs

  
## HEADER
Header is made up of 12 bytes<br>
```
\[0-8\] = .ascii "StoopidDB"
\[9\] = \x0
\[0-9\] = .string "StoopidDB\x0"
\[10\] = Major version i.e \0x1 = 1
\[11\] = Minor version i. e \0x0 = 0
So version would be 1.0

\[12\] = \x0  For for spacing
```

## FORMAT
The DB-file is needs to read backwards because of the way that the entry table is constructed<br>
```
\[0\]
-HEADER
-BODY (DATA) - TABLES
-ENTRY TABLE
\[EOF\]
```

## Entry Table
Last 8 bytes = \[EOF-8\] = EntryTableStartPointer

\[EntryTableStart\]<br>
every entry is starting with "E" 0xFF "N" 0xFF "T" 0xFF and is MAX 256+4+8+6 bytes long - continues with a 4 byte int representing the length of the entry, followed nullterminated string (the table name) that is never more than 256 characters long followed by a 8 byte long offset into the file acting as a pointer

The EntryTable ends with the signature:
### ENTRYTABLEENDSIG
```
\[0\] = \x0
\[1\] = \xFF
\[2\] = \xFF
\[3\] = \x0
\[4\] = \x0
\[5\] = \xFF
\[6\] = \xFF
\[7\] = \x0
```
## TABLE
```
\[0-255\]Nullterminated name with a max size of 256 (ALWAYS STOP AT 255, EVEN IF A NULL IS NOT FOUND!)
\[(max)256-263\] EOT OFFSET = End of Table offset - 8 bytes long
\[264-268\]Amount of columns - 4 bytes long
```

## COLUMN
```
\[0-(max)255\] First colum name (256 B max)
\[256-260\] Column size 4 bytes
\[261\] Type of data in column (0 = int, 1 = varchar, 2 = TIME_DATE (UnixTime), 3 = Unsigned VarChar)
\[262\] SettingsByte (00000000) 1 = (0b10000000) = "NOT NULL", 2 = AUTOINCREMENT, 4 = PRIMARY_KEY
\[263-267\] Max data size (how much the data can take up) // 4 bytes
```

## ROW
```
RowSignature = uchar\[6\]{
\[0\]R,
\[1\]\0,
\[2\]\O,
\[3\]\0,
\[4\]\W,
\[5\]\0
}

length of row = 8 bytes
Repeating - end is located at offset+len{
columnId = 4 bytes
Data = MaxSizeOfColumnByIdInBytes

}
```


//TODO - Add space for saving variables that needs to be used later (like the autoincrement tmp variable)
