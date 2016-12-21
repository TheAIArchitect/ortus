delegans

Keyboard Controls:

Quit:
- Escape will close the window

Camera Controls:
- Rotate scene around target point: scroll up/down and left/right
- Pan target point: hold shift while scrolling; up/down and left/right will adjust target point accordingly
- Zoom in/out: hold control while scrolling;  up/down and left/right will adjust target point accordingly
- Save current camera configuration/orientation as new default (and print to console): space bar

Worm Controls:

- Left/Right keys: move forward/backward through voltage windows. 
    => To move more than one window per press (i.e. to run through windows), hold the SUPER/command key.

- Number key (1): Path length control. Only draw connections up to set number of layers from set of starting neurons. Default is (currently) 3.
- Number key (2): Path weight control. Only draw connections with weight greater than or equal to set number. Default is (currently) 20.
- Number key (3): Toggle show all neurons. If off, only neurons with connections that satisfy the constraints from (1) and (2) will be shown. I.e., no non-connected neurons will be drawn. Default is (currently) off.
- Number key (4): Toggle show muscles. If off, no muscles will be drawn. Default is (currently) off. 
- Number key (5): Toggle show activation. If on, next to neuron name will be its current activation level. Default is (currently) on.
    => NOTE: This option may significantly slow things down, especially if all neurons are shown.
- Character 'c': Toggle show chemical synapses
- Character 'g': Toggle show gap junctions
- Character 'w': Toggle show connection weights
    => NOTE: This option may significantly slow things down, especially if all neurons are shown.


Voltage/Activation Probe:

- Right/Left arrow: Print out the list of neurons and muscles giving input to the selected muscle or neuron
    => Holding the SUPER/command key will play windows in quick succession.
- Character 'p': Input desired neuron or muscle to Probe. See below for details on input.

Worm Control Instructions:
- Number keys (1) and (2):
    => Press once, followed by new number, followed by ENTER to set value
    => Pressing DELETE key prior to pressing ENTER will cancel the command
- Number keys (3), (4), and (5):
    => On/off toggles. Pressing them will invert their current state.
- Character 'p':
    => Press once, followed by name of neuron or muscle, followed by ENTER to set value
    => Pressing DELETE key prior to pressing ENTER will cancel the command
    => This operation *IS NOT* case-sensitive

