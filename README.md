# Tachometer pcb and arduino
A small reference of my tachometer project for my Volvo 340.
This project uses the raw ignition signal and displays the rpm on the screen. The arduino code has 5 screens (splash, RPM, short time graph, long time graph, highest rpm).

The high (275v) voltage of the ignition signal is clamped (<5v) so it's readable for the ATmega328P. I've used JLCPCB to develop my PCB. All gerber files and BOM is available.

## Example
Image
<img src="https://raw.githubusercontent.com/dickverbunt/Tachometer/master/Example.jpg" style="max-width:100%;" alt="Example"  title="Example" />


Raw Ignition signal
<img src="https://raw.githubusercontent.com/dickverbunt/Tachometer/master/Signal.jpg" style="max-width:100%;" alt="Raw ignition signal"  title="Raw ignition signal" />

PCB's
<img src="https://raw.githubusercontent.com/dickverbunt/Tachometer/master/PCBs.jpg" style="max-width:100%;" alt="Raw ignition signal"  title="Raw ignition signal" />
