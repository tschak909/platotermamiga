PLATOTerm 1.2 for ZX Spectrum Users with an RS-232 Interface
=========================================

Changes since last release:
=================

* Support for ZX Printer! Press EXTEND 9 to print the currently visible screen.

Supported Interfaces
====================

Currently two RS-232 interfaces are supported:

* ZX Interface 1
* ZX Spectrum +, 128K, and +2, +3 RS-232 port.

You will also need a device that connects the RS-232 port to the internet. Common options here include an RS-232 wifi modem, or connecting a Raspberry Pi running TCPSER (http://github.com/tschak909/tcpser) to the port.

Loading
---------
Since this is a tape image, it should be copied to the appropriate medium for your computer. Since there are many ways to do this, it is not mentioned in the scope of this document.

Once you've inserted your created tape, you can load the software in the usual manner:

LOAD ""

Using PLATOTerm
--------------------
PLATOTerm will then display a splash screen, indicating that it is ready for use.

![image](https://i.imgur.com/xZGyEYB.png)

Once you see this splash screen, the terminal is ready for use, and you can send commands to your WIFI modem, to connect to a PLATO service, such as IRATA.ONLINE:

ATDTIRATA.ONLINE:8005

or, CYBER1.ORG

ATDTCYBERSERV.ORG:8005

For this initial testing release, the baud rate is set to 4800BPS, if this baud rate proves too problematic, please file an issue, and I will add in preferences to set the baud rate and cut a new release.

PLATO Keyboard
-------------------
The PLATO keyboard is mapped to the Spectrum keys, like so:

| PLATO KEY  	   | ZX Key  	|
|---	|---	   |
| ANS  	| EXTEND-A	|
| BACK    | EXTEND-B  	|
| SHIFT-BACK | EXTEND-SHIFT-B |
| COPY | EXTEND-C |
| SHIFT-COPY | EXTEND-SHIFT-C  |
| DATA | EXTEND-D |
| SHIFT-DATA | SHIFT-EXTEND-D |
| EDIT | EXTEND-E |
| SHIFT-EDIT | SHIFT-EXTEND-E |
| FONT | EXTEND-F |
| &#247; | EXTEND-G |
| HELP | EXTEND-I |
| SHIFT-HELP | SHIFT EXTEND-H |
| LAB | EXTEND-L |
| SHIFT-LAB | SHIFT-EXTEND-L |
| SUPER | EXTEND-P |
| SHIFT-SUPER | SHIFT EXTEND-P |
| SQUARE | EXTEND-Q |
| ACCESS | SHIFT EXTEND-Q |
| STOP | EXTEND-S |
| SHIFT-STOP | SHIFT EXTEND-S |
| TERM | EXTEND-T |
| &#215; | EXTEND-X |
| SUB | EXTEND-Y |
| SHIFT-SUB | SHIFT EXTEND-Y |
| Print screen | EXTEND 9 |
| Get Key Help | EXTEND 0 |

As mentioned in the table above, Pressing EXTEND 0 gives you a menu of keys, asking you to input which one for which you want the equivalent ZX spectrum keypress.

Problems?
-----------
This is early release software. If you have problems, please raise an issue, and I will address them and cut a new release. I am releasing this early and often so that everyone can test, and we can in the end, end up with a better piece of software, faster.
