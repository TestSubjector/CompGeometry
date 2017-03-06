# SelTopicsRepository
A Place To Store All Files Related To The Course CS-F441

## Instructions for the visualizer
The Visualizer is written in Python 2.7.13 and uses matplotlib as a dependency. So make sure both of these are on your system.
matplotlib can be downloaded via pip by executing
```
pip install matplotlib
```

It operates as a command-line utility and reads input from a CH file. The command syntax is like
```
PythonVisualiser path_to_ch_file
```
In case the path is omitted, `./input.ch` is the default file location that is taken.

The output is graphically displayed in a window that allows operations such as pan, zoom etc.

**Known Shortcomings:** If the graph has majority of points clustered nearby (i.e. they require a smaller scale) but there is also one point that is far away from this cluster, then the scale is taken as large in order to accommodate this point into the display. This means the cluster might not look like a hull at all unless appropriate scale is achieved by pan and zoom.
