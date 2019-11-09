# Advanced Strategic Command

A turn based strategy game in the tradition of Battle Isle

It can be played
 - as single player game against the computer, offering several campaigns
 - sequential multiplayer, either hot-seat of play-by-email
 - in the multi-player universe [Project Battle Planets](http://www.battle-planet.de) (German language community)

## The website

http://www.asc-hq.org
 
## Getting in contact

The ASC community is active in this Forum: 
http://battle-planet.de/pbp/main/news.php?forum


## Variants of ASC

### ASC 2.6

ASC 2.6 (on Git ` master` branch) is the traditional game engine, for which the campaigns have been designed. This is the best single-player experience available, and the version that is published on SourceForge.

### ASC 2.8

ASC 2.8 (on Git `newattack` branch) has a couple of changes to the game mechanics that were made for [Project Battle Planets](http://www.battle-planet.de). These changes broke the campaigns and nobody bothered to fix them so far. For that reason, this version is recommended for participants of [Project Battle Planets](http://www.battle-planet.de) only.


## Downloading binaries

Get them from https://sourceforge.net/projects/asc-hq/files/


## Building ASC

### Linux

Straight forward:
    
    ./configure
    make
    sudo make install
    
`configure` will check for required libraries, which you may need to install. The scripts `scripts/getUbuntuPackages` and `scripts/getFedoraPackages` will install the required libraries with a single command, but may not be up to date. 


### Windows

Total pain in the a**

check this [short description](doc/compile-win32.md)
