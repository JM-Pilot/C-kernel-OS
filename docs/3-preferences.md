# Preferences
## Entry
We all have preferences. Whether it's your type of coffee (or tea) or coding style, the latter being explained here. We're not here to choose between coffee or tea.

This is not a set of rules to follow; rather, it's a set of optional standards. You can just ignore this document and move on with your life. But if you don't... continue reading.

## Coding style
### C
Tabs are tabs. I don't care about how big they are in your editor. Preferably, do not use multiple spaces; they look ugly in diffs. As the Linux kernel coding style document nicely puts it:

> "The answer to that is that if you need more than 3 levels of indentation, you’re screwed anyway, and should fix your program."

Not that this kernel caps all indent levels to 3, but it's generally good advice.

Now to the ways stuff is indented. switch-case statements can be indented like this:

```c
switch (loglevel) {
    case 0:
        letter = 'e';
        break;
    case 1:
        letter = 'c';
        break;
}
```

But also like this:

```c
switch (loglevel) {
case 0:
    letter = 'e';
    break;
case 1:
    letter = 'c';
    break;
}
```

if statements with a single statements should usually be grouped into one statement:

```c
if (sc == 0x01) return 0x1B;
```

Unless there is an else or else if attached to it:

```c
if (status & 0x01) {
    inb(0x60);
} else {
    break;
}
```

There are niche cases in the kernel where this is not followed. But for any typical multi-statement `if`:

```c
if (sc == SCANCODE_EXT) {
    ext = 1;
    return 0;
}
```

Another thing about these loops and ifs is how we indent them and how we place the braces. Usually, this is done, and it's totally fine:

```c
if (sc == SCANCODE_EXT)
{
    ext = 1;
    return 0;
}
```

However it looks like an excuse to flex more LoC. Almost everywhere it's preferred to do:

```c
if (sc == SCANCODE_EXT) {
    ext = 1;
    return 0;
}
```

But it's not unlikely I'll nicely tell you to GTFO if your code is indented like this:

```c
if (sc == SCANCODE_EXT)
    {
        ext = 1;
        return 0;
    }
```

as it's just ugly, with no offense to GNU software.

Almost never force multiple statements into one line in an if. This also applies to loops. The exception is in a regular statement, where the two lines are really short:

```c
char *a; char *b = input;
```

However, this may cause us to skip statements while debugging, so be really careful what you apply this to.

### Assembly (GNU as)
The GNU assembler is often called GNU ass because of the preference of AT&T syntax over Intel. I'm not here to point out which is better.

Like in C, prefer the usage of actual tab characters. In the case of strings, don't indent them.

```asm
stat_boot_init:
.asciz "Boot stub: kernel initialized"
```

For functions, keep the order of:
1. section
2. globals
3. type
4. externs
5. the actual label

This is not a standard to follow; just a preference. Don't question it if possible.

### Assembly (NASM)
(See Assembly (GNU as))
