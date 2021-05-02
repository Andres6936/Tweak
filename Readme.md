# Tweak: an efficient hex editor

Tweak is a hex editor. It allows you to edit a file at very low level, letting
you see the full and exact binary contents of the file. It can be useful for
modifying binary files such as executables, editing disk or CD images, debugging
programs that generate binary file formats incorrectly, and many other things.

Tweak currently runs on Unix, using the ncurses library to run in a terminal.

## Why yet another hex editor?

Tweak uses a complex data structure based on B-trees, designed to make almost
all editing operations extremely fast, even when they are working on huge
amounts of data.

The data structure is described in detail on a separate page for those
interested. The bottom line is:

- Tweak supports insert mode (not particularly useful if you're editing an
  executable file or a filesystem image, but can be extremely handy in other
  file formats such as PNG).

- Cutting, copying and pasting within the file you are editing is extremely
  efficient. No matter how big the chunk of data you are moving around - even if
  it's a 200Mb section of a CD image - Tweak will always perform the operation
  effectively instantly.

- Tweak supports lazy loading of the input file: rather than sucking it all into
  memory straight away, it simply remembers which parts of the editing buffer
  are copies of which parts of the input file and refers to the file on disk
  when it needs to. Tweak only has to take significant time when you genuinely
  need it to read the entire file. The only two operations with this property
  are searching, and saving the modified version of the file to disk. Everything
  else is instant.

You might think, from reading this, that the entire purpose of publishing Tweak
was to show off my shiny data structure. You would be right :-)

### What is its current status?

The current public release, Tweak 3.02, is available as a source tarball here:

[tweak-3.02.tar.gz](https://www.chiark.greenend.org.uk/~sgtatham/tweak/tweak-3.02.tar.gz)

You can read its man page [online](https://www.chiark.greenend.org.uk/~sgtatham/tweak/manpage-3.02.html).

If you want to see the latest state of development, you can check the development sources out from my git repository:

```git clone https://git.tartarus.org/simon/tweak.git```

Alternatively, you can browse the repository on the web, here.

###### Page maintained by Simon Tatham.