Plotter
=======

## Dependencies

* [ROOT](http://root.cern.ch) 5.28 or higher


## Install

```make``` (and add bin/plotter to the PATH)

or

```make && make install``` (this will put the plotter binary in the usual place: /usr/bin)


## Bash completion

If you are using bash, you can install the bash completion file doing this:

    cp extra/plotter_completion /etc/bash_completion.d/plotter


## Usage

    plotter [options] file1.root file2.root file3.root ...

    Options
    -m, --merge: If the input files have the same tree, the tree is merged using a TChain and is shown as a unique tree.

plotter will only read the "plotable" objects from the files.

And make plots :D!
