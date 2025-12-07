# Tournament builder

Tournament builder is a simple desktop application that produces a set of teams, given a list of people (with their associated skill level) and a list of all the possible skill levels allowed.

It was originally made in the context of tournament organization, but its algorithm can be applied in any similar situation that requires to group a set number of weighted resources evenly.

The application is cross-platform compatible with Windows and Linux.

---

The application can be used in two ways, with a Graphical User Interface or via Command Line Interface. 

## GUI

Simply launching it on a desktop environment or on a CLI with no arguments will open the Graphical User interface:

<img src="doc_assets/GUIInterface.png">

## CLI
The application can also be used completely via Command Line Interface, passing to it the necessary arguments:

<img src="doc_assets/CLIExample.png">

## Inputs

In either way the program needs as inputs at least two files, both in csv format:

#### Players list

A csv file containing two columns, having on the very first line respectively the texts "Player" and "Rank", which I will be referring to as "headers". 
Under the Player header every non-blank text will be counted as a player (blank spaces before and after the name will not be included).
Under the Rank header, corresponding to a player name, it will be counted the respective rank. Each rank has to be present in the rank list too (see the following section). All the players with no associated Rank will be valued as if their rank was of value 0.

<img src="doc_assets/PlayerCSVExample.png">

The csv can contain other data, it's only necessary that the Player and Rank columns are present as explained.

#### Ranks list

A csv file containing at least one column and at most two. On the first column can be reported all the rank names, while on the second column, if present, have to be reported the values corresponding to each and every rank.


<img src="doc_assets/RankCSVExample2.png">

In ranks list files with one column, the first rank mentioned will be counted as if it was of value 0, while the subsequent ranks will be counted with values 1, 2, 3... and so on.

<img src="doc_assets/RankCSVExample1.png">

## Optional input (ouput directory)

Normally the program output will be created in the same directory of the program, but it is possible to specify a different directory.

## How to build from source
Clone the repository and init all the submodules.
It's simple to build from source via cmake (link here to build tutorial).
On Windows it will build without effort.
On Linux it's necessary to download the dipendencies for [GLFW](https://www.glfw.org/docs/3.3/compile.html) and for [NFDe](https://github.com/btzy/nativefiledialog-extended).

## License
Tournament builder is licensed under the MIT License, see LICENSE.txt for more information.

---

Icon copyright:
<a href="https://www.flaticon.com/free-icons/hammer" title="hammer icons">Hammer icons created by max.icons - Flaticon</a>
